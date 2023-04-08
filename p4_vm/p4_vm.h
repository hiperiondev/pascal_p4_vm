/*
 * Copyright 2023 Emiliano Gonzalez LU3VEA (lu3vea @ gmail . com))
 * * Project Site: https://github.com/hiperiondev/pascal_p4_vm *
 *
 * This is based on other projects:
 *      I.J.A.vanGeel@twi.tudelft.nl (August 22 1996) - https://github.com/hiperiondev/pascal_p4_vm/tree/main/original/p4
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

#ifndef P4_VM_H_
#define P4_VM_H_

#include <stdint.h>
#include <stdbool.h>

#define CODEMAX    8650
#define PCMAX      17500
#define MAXSTK     13650   // size of variable store
#define OVERI      13655   // size of integer constant table = 5
#define OVERR      13660   // size of real constant table = 5
#define OVERS      13730   // size of set constant table = 70
#define OVERB      13820
#define OVERM      18000
#define MAXSTR     18001
#define LARGEINT   26144
#define BEGINCODE  3
#define INPUTADR   5
#define OUTPUTADR  6
#define PRDADR     7
#define PRRADR     8
#define DUMINST    62

typedef long settype[3];

typedef struct rec_code_s {
    uint8_t op1 :7;
    uint8_t p1  :4;
    int16_t q1  :16;
    uint8_t op2 :7;
    uint8_t p2  :4;
    int16_t q2;
} rec_code_t;

// store access
typedef union rec_store_s {
    int32_t vi;
     double vr;
       bool vb;
    settype vs;
    int16_t vc;
    int16_t va;
    int32_t vm; // address in store
} rec_store_t;

typedef struct p4_vm_s {
     rec_code_t code[CODEMAX + 1];
       uint32_t pc; // program address register
           bool run;
    rec_store_t store[OVERM + 1];
        int16_t mp; // points to beginning of a data segment
        int16_t sp; // points to top of the stack
        int16_t np; // points to the maximum extent of the stack
        int16_t ep; // points to top of the dynamically allocated area
} *p4_vm_t;

uint8_t p4_vm_interpret(p4_vm_t p4vm);

#endif /* P4_VM_H_ */
