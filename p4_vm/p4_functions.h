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

#ifndef P4_FUNCTIONS_H_
#define P4_FUNCTIONS_H_

long* p4_fn_setunion(register long *d, register long *s1, register long *s2);
long* p4_fn_setint(register long *d, register long *s1, register long *s2);
long* p4_fn_setdiff(register long *d, register long *s1, register long *s2);
long* p4_fn_setxor(register long *d, register long *s1, register long *s2);
int p4_fn_inset(register unsigned val, register long *s);
long* p4_fn_addset(register long *s, register unsigned val);
long* p4_fn_addsetr(register long *s, register unsigned v1, register unsigned v2);
long* p4_fn_remset(register long *s, register unsigned val);
int p4_fn_setequal(register long *s1, register long *s2);
int p4_fn_subset(register long *s1, register long *s2);
long* p4_fn_setcpy(register long *d, register long *s);
long* p4_fn_expset(register long *d, register long s);

#endif /* P4_FUNCTIONS_H_ */
