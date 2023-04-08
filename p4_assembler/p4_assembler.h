/*
 * Copyright 2023 Emiliano Gonzalez LU3VEA (lu3vea @ gmail . com))
 * * Project Site: https://github.com/hiperiondev/pascal_p4_vm *
 *
 * This is based on other projects:
 *      I.J.A.vanGeel@twi.tudelft.nl (August 22 1996) - https://github.com/hiperiondev/pascal_p4_vm/original
 *      - Assembler and interpreter of Pascal code: K. Jensen, N. Wirth, Ch. Jacobi, ETH May 76
 *
 *    please contact their authors for more information.
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef P4_ASSEMBLER_H_
#define P4_ASSEMBLER_H_

#include "p4_vm.h"

#define MAXLABEL 1850

typedef char alfa_[10];

typedef enum LABELST {
    ENTERED, //
    DEFINED  //
} labelst_t; // label situation

// label range
typedef struct labelrec {
    short val;
    labelst_t st;
} labelrec_t;

// static variables for load:
typedef struct loc_load_s {
    short icp, rcp, scp, bcp, mcp; // pointers to next free position
    char word[10];
    char ch;
    labelrec_t labeltab[MAXLABEL + 1];
    short labelvalue;
} loc_load_t;

// static variables for pmd:
typedef struct loc_pmd_s {
    long s, i;
} loc_pmd_t;

// static variables for assemble:
typedef struct loc_assemble_s {
    loc_load_t *LINK;
    // goto 1 for instructions without code generation
    alfa_ name;
} loc_assemble_t;

void p4_assembler(p4_vm_t p4vm);

#endif /* P4_ASSEMBLER_H_ */
