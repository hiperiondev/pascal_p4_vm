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

#ifndef _INTERNAL_H
#define _INTERNAL_H

#include <stdbool.h>
#include <stdint.h>
#include <setjmp.h>
#include <stdio.h>

#include "p4_file.h"
#include "p4_assembler.h"

typedef struct local_jmp_buf_s {
    struct local_jmp_buf_s *next;
    jmp_buf jbuf;
} local_jmp_buf_t;

typedef long settype[3];

extern file_t prd, prr; // prd for read only, prr for write only
extern jmp_buf _JL1;

char* _ShowEscape(char *buf, int code, int ior, char *prefix);
int _Escape(int code);
int _EscIO(int code);
void _errorl(char *string, loc_load_t *LINK);
void _pt(p4_vm_t p4vm, loc_pmd_t *LINK);
void pmd(p4_vm_t p4vm, uint8_t op);

#endif /* _INTERNAL_H */
