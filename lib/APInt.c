#include "APInt.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* You code to define APInt here. */

APInt *uint64_to_APInt(uint64_t value) {
  int leading_zeros = 0;
  for (int i = 63; i >= 0; i--) {
    if ((value >> i) & 1) {
      break;
    }
    leading_zeros++;
  }
  leading_zeros /= 8;

  int size = sizeof(uint64_t) - leading_zeros;

  APInt *apint = (APInt *)malloc(sizeof(APInt));
  apint->size = size;
  apint->bytes = (uint8_t *)calloc(size, sizeof(uint8_t));
  for (int i = size - 1; i >= 0; i--) { // Currently Big Endian. Depends on direction of iteration.
      apint->bytes[i] = value & 0xFF;
      value >>= 8;
  }
  return apint;
}

APInt *hex_to_APInt(char *hex_in) {
  int hex_len = strlen(hex_in);
  int size;
  bool isOdd;
  if (hex_len % 2) { // is odd?
    isOdd = true;
    size = (strlen(hex_in) + 1) / 2;
  } else { // is even?
    isOdd = false;
    size = strlen(hex_in) / 2;
  }
  
  APInt *apint = (APInt *)malloc(sizeof(APInt));
  apint->size = size;
  apint->bytes = (uint8_t *)calloc(size, sizeof(uint8_t));
  for (int i = size - 1; i >= 0; i--) {
    char byte[3] = {'\0', 0, 0};
    if (isOdd) {
      if (i == 0) {
        strncat(byte, (const char*)&(hex_in[2*i]), 1);
        apint->bytes[i] = (uint8_t)strtoul(byte,NULL,16);
      } else {
        strncat(byte, (const char*)&(hex_in[2*i - 1]), 2);
        apint->bytes[i] = (uint8_t)strtoul(byte,NULL,16);
      }
    } else {
      strncat(byte, (const char*)&(hex_in[2*i]), 2);
      apint->bytes[i] = (uint8_t)strtoul(byte,NULL,16);
    }
  }
  return apint;
}

APInt *clone_APInt(APInt *src) {
  APInt *dst = (APInt *)malloc(sizeof(APInt));
  dst->size = src->size;
  dst->bytes = (uint8_t *)calloc(src->size, sizeof(uint8_t));
  memcpy(dst->bytes, src->bytes, src->size * sizeof(uint8_t));
  return dst;
}

void left_shift_APInt(APInt *dst, APInt *src, uint64_t k) {
  int byteS = k / 8; //previously uint64_t
  int bitS = k % 8; //same^
  
  // Copy source into temporary buffer / register
  APInt *buf = (APInt *)malloc(sizeof(APInt));
  buf->size = src->size + byteS + 1; // + 1 leaves space for bit runover
  buf->bytes = (uint8_t *)calloc(buf->size, sizeof(uint8_t));
  memcpy(buf->bytes + byteS + 1, src->bytes, src->size * sizeof(uint8_t));

  // Perform left shift
  // Bytes
  if (byteS) {
    for (int i = 0; i < buf->size - byteS; i++) {
      buf->bytes[i] = buf->bytes[i + byteS];
    }
    for (int i = buf->size - byteS; i < buf->size; i++) {
      buf->bytes[i] = 0;
    }
  }
  // Bits
  for (int i = 0; i < buf->size - 1; i++) {
    buf->bytes[i] <<= bitS;
    buf->bytes[i] |= buf->bytes[i + 1] >> (8 - bitS);
  }
  buf->bytes[buf->size - 1] <<= bitS;

  // Removes leading zeros / bytes from shift
  int leading_zeros = 0;
  for (int i = 0; i < buf->size; i++) {
    if (buf->bytes[i] != 0) {
        break;
    }
    leading_zeros++;
  }
  if (leading_zeros > 0 && leading_zeros < buf->size) {
    int new_size = buf->size - leading_zeros;
    uint8_t *new_bytes = (uint8_t *)malloc(new_size * sizeof(uint8_t));
    memcpy(new_bytes, buf->bytes + leading_zeros, new_size);
    free(buf->bytes);
    buf->bytes = new_bytes;
    buf->size = new_size;
  }

  // Copy temporary buffer into dest-th place
  free(dst->bytes);
  dst->size = buf->size;
  dst->bytes = (uint8_t *)calloc(buf->size, sizeof(uint8_t));
  memcpy(dst->bytes, buf->bytes, buf->size * sizeof(uint8_t));

  // Free buffer
  free(buf->bytes);
  free(buf);
}

void add_APInts(APInt *dst, APInt *op1, APInt *op2) {
  int max_size = op1->size > op2->size ? op1->size : op2->size;
  int min_size = op1->size < op2->size ? op1->size : op2->size;

  APInt *res = (APInt *)malloc(sizeof(APInt));
  res->size = max_size + 1;
  res->bytes = (uint8_t *)calloc(res->size, sizeof(uint8_t));

  uint8_t carry = 0;
  for (int i = 0; i < min_size; i++) {
    int i1 = op1->size - 1 - i;
    int i2 = op2->size - 1 - i;
    int ir = res->size - 1 - i;

    uint16_t sum = op1->bytes[i1] + op2->bytes[i2] + carry;
    carry = (sum > 0xFF) ? 1 : 0;

    res->bytes[ir] = (uint8_t)(sum & 0xFF);
  }

  const APInt *longer = op1->size > op2->size ? op1 : op2;
  for (int i = min_size; i < max_size; i++) {
    int il = longer->size - 1 - i;
    int ir = res->size - 1 - i;

    uint16_t sum = longer->bytes[il] + carry;
    carry = (sum > 0xFF) ? 1 : 0;

    res->bytes[ir] = (uint8_t)(sum & 0xFF);
  }

  // If exists carry, else dne
  if (carry) {
    res->bytes[0] = 1;
  } else {
    res->size--;
    memmove(res->bytes, res->bytes + 1, res->size * sizeof(uint8_t));
    uint8_t *tempByte = (uint8_t*)realloc(res->bytes, res->size * sizeof(uint8_t));
    if (tempByte) {
      res->bytes = tempByte;
    }
  }

  // Copy result into dest-th place
  free(dst->bytes);
  dst->size = res->size;
  dst->bytes = (uint8_t *)calloc(res->size, sizeof(uint8_t));
  memcpy(dst->bytes, res->bytes, res->size * sizeof(uint8_t));

  // Free buffers
  free(res->bytes);
  free(res);
}

void mul_UInt(APInt *dst, APInt *src, uint64_t k) {
  // Allocate empty APInt
  APInt *res = (APInt *)malloc(sizeof(APInt));
  res->size = src->size + sizeof(uint64_t);
  res->bytes = (uint8_t *)calloc(res->size, sizeof(uint8_t));
  
  uint64_t carry = 0;
  for (int i = 0; i < src->size; i++) {
    uint64_t prod = ((uint64_t)src->bytes[src->size - 1 - i] & (uint8_t)0xFF) * (uint64_t)k + carry;
    res->bytes[res->size - 1 - i] = (uint8_t)(prod & (uint8_t)0xFF);
    carry = prod >> 8;
  }
  for (int i = 0; i < (int)sizeof(uint64_t) && carry; i++) {
    res->bytes[res->size - src->size - 1 - i] = carry & (uint8_t)0xFF;
    carry >>= 8;
  }

  free(dst->bytes);
  dst->size = res->size;
  dst->bytes = (uint8_t *)calloc(res->size, sizeof(uint8_t));
  memcpy(dst->bytes, res->bytes, res->size * sizeof(uint8_t));

  free(res->bytes);
  free(res);
}

void mul_APInts(APInt *dst, APInt *op1, APInt *op2) {
  // Allocate empty APInt
  APInt *res = (APInt *)malloc(sizeof(APInt));
  res->size = op1->size + op2->size;
  res->bytes = (uint8_t *)calloc(res->size, sizeof(uint8_t));
  
  uint64_t carry = 0;
  for (int i = op1->size - 1; i >= 0; i--) {
    for (int j = op2->size - 1; j >= 0; j--) {
      uint64_t prod = (uint64_t)op1->bytes[i] * (uint64_t)op2->bytes[j] + res->bytes[i + j + 1] + carry;
      res->bytes[i + j + 1] = prod & 0xFF;
      carry = prod >> 8;
    }
    res->bytes[i] += carry;
    carry = 0;
  }
  // Removes leading zeros from multiplication
  int leading_zeros = 0;
  for (int i = 0; i < res->size; i++) {
    if (res->bytes[i] != 0) {
        break;
    }
    leading_zeros++;
  }
  if (leading_zeros > 0 && leading_zeros < res->size) {
    int new_size = res->size - leading_zeros;
    uint8_t *new_bytes = (uint8_t *)malloc(new_size * sizeof(uint8_t));
    memcpy(new_bytes, res->bytes + leading_zeros, new_size);
    free(res->bytes);
    res->bytes = new_bytes;
    res->size = new_size;
  }

  free(dst->bytes);
  dst->size = res->size;
  dst->bytes = (uint8_t *)calloc(res->size, sizeof(uint8_t));
  memcpy(dst->bytes, res->bytes, res->size * sizeof(uint8_t));

  free(res->bytes);
  free(res);
}

void pow_APInt(APInt *dst, APInt *src, uint64_t k) {
  // Declare empty APInt
  // Allocating bytes in res is dependent on k, and is mostly handled by mult
  // Thanks to class discussion, I know that free(NULL) is legitimate **must be initialized**
  APInt *res = (APInt *)malloc(sizeof(APInt));
  res->bytes = NULL;

  // To power is to multiply repetitively. log(n) can be achieved by creating a recursive tree
  // x^4 == x * x * x * x = x^2 * x * x == x^3 * x == x^4   or
  // x^4 == x * x * x * x = x^2 * x^2 == x^4

  if (k == 0) {
    res->size = 1;
    res->bytes = (uint8_t *)calloc(res->size, sizeof(uint8_t));
    res->bytes[0] = 1;
  } else if (k == 1) { // Probably unnecessary but I don't want to convolute things
    res->size = src->size;
    res->bytes = (uint8_t *)calloc(res->size, sizeof(uint8_t));
    memcpy(res->bytes, src->bytes, src->size * sizeof(uint8_t));
  } else {
    APInt *temp = (APInt *)malloc(sizeof(APInt));
    temp->bytes = NULL;
    pow_APInt(temp, src, k/2);
    mul_APInts(res, temp, temp);
    destroy_APInt(temp);

    if (k % 2) { // if odd
      APInt *temp = (APInt *)malloc(sizeof(APInt));
      temp->bytes = NULL;
      mul_APInts(temp, res, src);
      destroy_APInt(res);
      res = temp;
    }
  }

  free(dst->bytes);
  dst->size = res->size;
  dst->bytes = (uint8_t *)calloc(res->size, sizeof(uint8_t));
  memcpy(dst->bytes, res->bytes, res->size * sizeof(uint8_t));

  free(res->bytes);
  free(res);
}

void cmp_APInts(APInt *op1, APInt *op2, FILE *output) {
  int cmp = 0;
  if (op1->size > op2->size) {
    cmp = 1;
  } else if (op1->size < op2->size) {
    cmp = -1;
  } else { // Same size
    for(int i = 0; i < op1->size && !cmp; i++) {
      if (op1->bytes[i] > op2->bytes[i]) {
        cmp = 1;
      } else if (op1->bytes[i] < op2->bytes[i]) {
        cmp = -1;
      }
    }
  }
    fprintf(output, "%d\n", cmp);
}

void destroy_APInt(APInt *apint) {
  if (apint) {
      free(apint->bytes);
      free(apint);
  }
}

int print_APInt(APInt *apint, FILE *output) {
  if (!apint)
      return -1;
  int charcount = 0;
  bool lsb_zero_flag = false;
  bool all_zero_flag = true;
  charcount += fprintf(output, "0x");
  for (int i = 0; i < apint->size; i++) {
    if (apint->bytes[i]) {
      all_zero_flag = false;
    }
    if (apint->bytes[i] || lsb_zero_flag) {
      lsb_zero_flag = true;
      charcount += fprintf(output, "%02x", apint->bytes[i]);
    }
  }
  if (all_zero_flag) {
    charcount += fprintf(output, "00");
  }
  charcount += fprintf(output, "\n");
  return charcount;
}

bool clean_input_str(char *input) {
  int len = strlen(input);
  bool isEmpty = false;
  if (len == 0) {
    isEmpty = true;
  }
  if (len > 0 && input[len - 1] == '\n') {
      input[len - 1] = '\0';
  }
  return isEmpty;
}

//char *endptr;
//uint64_t value = strtoull(HEX_STRING, &endptr, 16);
