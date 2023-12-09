# security-ap-int
This project was completed by myself for my computer security class assignment. The following information was provided by the instructor as guidelines for the assignment. `CMakeLists.txt` was also given by the instructor.

### [Arbitrary precision](https://en.wikipedia.org/wiki/Arbitrary-precision_arithmetic)
We often seen fixed-precision integers. For example, `int64_t`, `uint32_t` in C/C++, `i64`, `u32` in Rust. 
These integers have a fixed width and a limited numbers they can represent. 
If the number were any larger (or smaller), the representation is incorrect. For example, in C `(uint32_t) 0xffffffff + 1 == 0`.
This may be very in-convenient and error prone when we are carrying out large scale computations.

However, integer in Python doesn't seem to have this limitation and can represent arbitrary precisions. 
You can calculate any integer operations without worrying the upper bound or lower bound.

In this homework, we implement an arbitrary precision int (`APInt`).
For simplicity, `APInt` only stores non-negative numbers.

### Input specification

The input consists multiple lines.
The first line has only one number `n` (`n < 10000`), indicating we should initialize an array of `APInt`s.

The following `2n` lines describes these `n` `APInt`s.
First line would be one of the following three: `UINT64`, `HEX_STRING`, `CLONE`.
- `UINT64`, the following line will be an integer with type `uint64_t`. Assignment grading guaranteed that the integer is valid.
- `HEX_STRING`, the following line will be a string that satisfies [regular expression](https://en.wikipedia.org/wiki/Regular_expression): `[0-9,a-f]*`. Simply put, it is a number represented using hexadecimal. It may be very large. Assignment grading also guaranteed the string to be valid.
- `CLONE`, followed by an index. This number will be a clone of a previous integer. Assignment grading guaranteed the index to be valid.
- Any other inputs are considered illegal and the program terminates immediately.

The following lines represent the operations we have for the array of `APInt`s.
Following are the types of commands:
- `DUMP`, `DUMP` has no arguments. It will print the `APInt`s one by one. Each `APInt` should be printed with a leading `0x`, then it's hexadecimal representation. If the number takes odd number of digits, a zero should be padded before it. (e.g. `0x01` instead of `0x1`) Each `APInt` should take ONE line, and an extra line should be printed at the end.
- `END` indicates that this is the final command, the program should quit now.
- `SHL` has three operands in the next line: `dst`, `src`, `k`, seperated by a space. This takes `src`-th `APInt` in the array, left shifts it by `k` bits, and stores it back to `dst`-th place in the array. `k` is `uint64_t`.
- `ADD` has three operands in the next line: `dst`, `op1`, `op2`, seperated by a space. All three operands are indices. This takes `op1` and `op2` from the array, adds them and places the result back to `dst`.
- `MUL_UINT64` has three operands in the next line: `dst`, `src`, `k`, seperated by a space. This calculates `src * k`, and stores it back to `dst`-th place in the array. `k` is `uint64_t`.
- `MUL_APINT` has three operands in the next line: `dst`, `op1`, `op2`, seperated by a space. All three operands are indices. This takes `op1` and `op2` from the array, multiplies them and places the result back to `dst`.
- `POW` has three operands in the next line: `dst`, `src`, `k`, seperated by a space. This takes `src`-th `APInt` in the array, calculates `src ^ k`, and stores it back to `dst`-th place in the array. `k` is `uint64_t`.
  - Hint: This task has performance requirements. `O(k)` solution is intuitive, can you think of an `O(log k)` one?
- `CMP` has two operands in the next line: `op1`, `op2`, seperated by a space. Both operands are indices. This takes `op1` and `op2` from the array, compares them. Print -1 if `op1` is less than `op1`, 0 if equal, 1 if greater.
- Any other inputs are considered illegal and the program terminates immediately. 

All numbers are `uint64_t` typed, i.e. some of the constants can be really large.

### File description

- `APInt.h` defines the struct.
- `APInt.c` is used to implement the functions.
- `main.c` carries out the read and write, including processing commands.
- `CMakeLists.txt` specifies how the project is built. `cmake <path-to-your-projec> -DCMAKE_BUILD_TYPE=asan` will enable [Address sanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer), which checks for memory usages.

## compilation instructions
Download repository contents into your desired directory.

`mkdir build` From this main directory, create a build folder to `cd` into.

`cmake -DCMAKE_BUILD_TYPE=asan ../src` configures CMake with ASan enabled.

`make`, then `./Main [input_filename.txt]` or `./Main [arguments...]` to build then run the code.
