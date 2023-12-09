#ifndef APINT_H
#define APINT_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct APInt {
  int size;
  uint8_t *bytes;
} APInt;

/* You code to declare any methods you deem necessary here. */

// Constructor-like functions
APInt *uint64_to_APInt(uint64_t value);
APInt *hex_to_APInt(char *hex_in);
APInt *clone_APInt(APInt *apint);

// Destructor-like function
void destroy_APInt(APInt *apint);

// Operations
void left_shift_APInt(APInt *dst, APInt *src, uint64_t k);
void add_APInts(APInt *dst, APInt *op1, APInt *op2);
void mul_UInt(APInt *dst, APInt *src, uint64_t k);
void mul_APInts(APInt *dst, APInt *op1, APInt *op2);
void pow_APInt(APInt *dst, APInt *src, uint64_t k);
void cmp_APInts(APInt *op1, APInt *op2, FILE *output);

int print_APInt(APInt *apint, FILE *output);

bool clean_input_str(char *input);


#endif