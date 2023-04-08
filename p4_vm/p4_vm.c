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

#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "p4_vm.h"
#include "p4_functions.h"
#include "p4_file.h"

/* Note for the implementation.
 ===========================
 This interpreter is written for the case where all the fundamental types
 take one storage unit.
 In an actual implementation, the handling of the sp pointer has to take
 into account the fact that the types may have lengths different from one:
 in push and pop operations the sp has to be increased and decreased not
 by 1, but by a number depending on the type concerned.
 However, where the number of units of storage has been computed by the
 compiler, the value must not be corrected, since the lengths of the types
 involved have already been taken into account.
 */

static short ad;
static bool b;
static long i, i1, i2;

static short base(p4_vm_t p4vm, long ld) {
    short ad;

    ad = p4vm->mp;
    while (ld > 0) {
        ad = p4vm->store[ad + 1].vm;
        ld--;
    }
    return ad;
} // base

static void compare(p4_vm_t p4vm, int16_t q) {
    // comparing is only correct if result by comparing integers will be
    i1 = p4vm->store[p4vm->sp].va;
    i2 = p4vm->store[p4vm->sp + 1].va;
    i = 0;
    b = true;
    while (b && i != q) {
        if (p4vm->store[i1 + i].vi == p4vm->store[i2 + i].vi)
            i++;
        else
            b = false;
    }
} // compare

static uint8_t callsp(p4_vm_t p4vm, int16_t q, uint8_t op) {

    //////////// file access //////////////

    void readi(p4_vm_t p4vm, file_t *f) {
        short ad;

        ad = p4vm->store[p4vm->sp - 1].va;
        fscanf(f->f, "%ld", (long int*) &(p4vm->store[ad].vi));
        p4vm->store[p4vm->store[p4vm->sp].va].vc = p4_file_peek(f->f);
        p4vm->sp -= 2;
    } // readi

    void readr(p4_vm_t p4vm, file_t *f) {
        short ad;

        ad = p4vm->store[p4vm->sp - 1].va;
        fscanf(f->f, "%lg", &(p4vm->store[ad].vr));
        p4vm->store[p4vm->store[p4vm->sp].va].vc = p4_file_peek(f->f);
        p4vm->sp -= 2;
    } // readr

    void readc(p4_vm_t p4vm, file_t *f) {
        char c;
        short ad;

        c = getc(f->f);
        if (c == '\n')
            c = ' ';
        ad = p4vm->store[p4vm->sp - 1].va;
        p4vm->store[ad].vc = c;
        p4vm->store[p4vm->store[p4vm->sp].va].vc = p4_file_peek(f->f);
        p4vm->store[p4vm->store[p4vm->sp].va].vi = p4_file_peek(f->f);
        p4vm->sp -= 2;
    } // readc

    void writestr(p4_vm_t p4vm, file_t *f) {
        long i, j, k;
        short ad;
        long FORLIM;

        ad = p4vm->store[p4vm->sp - 3].va;
        k = p4vm->store[p4vm->sp - 2].vi;
        j = p4vm->store[p4vm->sp - 1].vi;
        // j and k are numbers of characters
        if (k > j) {
            FORLIM = k - j;
            for (i = 1; i <= FORLIM; i++)
                putc(' ', f->f);
        } else
            j = k;
        for (i = 0; i < j; i++)
            putc(p4vm->store[ad + i].vc, f->f);
        p4vm->sp -= 4;
    } // writestr

    void getfile(p4_vm_t p4vm, file_t *f) {
        short ad;

        ad = p4vm->store[p4vm->sp].va;
        getc(f->f);
        p4vm->store[ad].vc = p4_file_peek(f->f);
        p4vm->sp--;
    } // getfile

    void putfile(p4_vm_t p4vm, file_t *f) {
        short ad;

        ad = p4vm->store[p4vm->sp].va;
        putc(p4vm->store[ad].vc, f->f);
        p4vm->sp--;
    } // putfile

    ///////////////////////////////

    bool line = false;
    file_t TEMP;

    switch (q) {
        case 0: // get
            switch (p4vm->store[p4vm->sp].va) {
                case 5:
                    TEMP.f = stdin;
                    *TEMP.name = '\0';
                    getfile(p4vm, &TEMP);
                    break;

                case 6:
                    return op;
                    break;

                case 7:
                    getfile(p4vm, &(p4vm->prd));
                    break;

                case 8:
                    return op;
                    break;
            }
            break;

        case 1: // put
            switch (p4vm->store[p4vm->sp].va) {

                case 5:
                    return op;
                    break;

                case 6:
                    TEMP.f = stdout;
                    *TEMP.name = '\0';
                    putfile(p4vm, &TEMP);
                    break;

                case 7:
                    return op;
                    break;

                case 8:
                    putfile(p4vm, &(p4vm->prr));
                    break;
            }
            break;

        case 2: // rst
            // for testphase
            p4vm->np = p4vm->store[p4vm->sp].va;
            p4vm->sp--;
            break;

        case 3: // rln
            switch (p4vm->store[p4vm->sp].va) {

                case 5:
                    scanf("%*[^\n]");
                    getchar();
                    p4vm->store[INPUTADR].vc = p4_file_peek(stdin);
                    break;

                case 6:
                    return op;
                    break;

                case 7:
                    scanf("%*[^\n]");
                    getchar();
                    p4vm->store[INPUTADR].vc = p4_file_peek(stdin);
                    break;

                case 8:
                    return op;
                    break;
            }
            p4vm->sp--;
            break;

        case 4: // new
            ad = p4vm->np - p4vm->store[p4vm->sp].va;
            // top of stack gives the length in units of storage
            if (ad <= p4vm->ep)
                return op;
            p4vm->np = ad;
            ad = p4vm->store[p4vm->sp - 1].va;
            p4vm->store[ad].va = p4vm->np;
            p4vm->sp -= 2;
            break;

        case 5: // wln
            switch (p4vm->store[p4vm->sp].va) {

                case 5:
                    return op;
                    break;

                case 6:
                    putchar('\n');
                    break;

                case 7:
                    return op;
                    break;

                case 8:
                    putc('\n', p4vm->prr.f);
                    break;
            }
            p4vm->sp--;
            break;

        case 6: // wrs
            switch (p4vm->store[p4vm->sp].va) {

                case 5:
                    return op;
                    break;

                case 6:
                    TEMP.f = stdout;
                    *TEMP.name = '\0';
                    writestr(p4vm, &TEMP);
                    break;

                case 7:
                    return op;
                    break;

                case 8:
                    writestr(p4vm, &(p4vm->prr));
                    break;
            }
            break;

        case 7: // eln
            switch (p4vm->store[p4vm->sp].va) {

                case 5:
                    line = p4_file_eoln(stdin);
                    break;

                case 6:
                    return op;
                    break;

                case 7:
                    line = p4_file_eoln(p4vm->prd.f);
                    break;

                case 8:
                    return op;
                    break;
            }
            p4vm->store[p4vm->sp].vb = line;
            break;

        case 8: // wri
            switch (p4vm->store[p4vm->sp].va) {

                case 5:
                    return op;
                    break;

                case 6:
                    printf("%*ld", (int) p4vm->store[p4vm->sp - 1].vi, (long int) p4vm->store[p4vm->sp - 2].vi);
                    break;

                case 7:
                    return op;
                    break;

                case 8:
                    fprintf(p4vm->prr.f, "%*ld", (int) p4vm->store[p4vm->sp - 1].vi, (long int) p4vm->store[p4vm->sp - 2].vi);
                    break;
            }
            p4vm->sp -= 3;
            break;

        case 9: // wrr
            switch (p4vm->store[p4vm->sp].va) {

                case 5:
                    return op;
                    break;

                case 6:
                    printf("% .*E", (((int) p4vm->store[p4vm->sp - 1].vi - 7) > (1) ? ((int) p4vm->store[p4vm->sp - 1].vi - 7) : (1)),
                            p4vm->store[p4vm->sp - 2].vr);
                    break;

                case 7:
                    return op;
                    break;

                case 8:
                    fprintf(p4vm->prr.f, "% .*E", (((int) p4vm->store[p4vm->sp - 1].vi - 7) > (1) ? ((int) p4vm->store[p4vm->sp - 1].vi - 7) : (1)),
                            p4vm->store[p4vm->sp - 2].vr);
                    break;
            }
            p4vm->sp -= 3;
            break;

        case 10: // wrc
            switch (p4vm->store[p4vm->sp].va) {

                case 5:
                    return op;
                    break;

                case 6:
                    printf("%*c", (int) p4vm->store[p4vm->sp - 1].vi, p4vm->store[p4vm->sp - 2].vc);
                    break;

                case 7:
                    return op;
                    break;

                case 8:
                    fprintf(p4vm->prr.f, "%*c", (int) p4vm->store[p4vm->sp - 1].vi, (char) p4vm->store[p4vm->sp - 2].vi);
                    break;
            }
            p4vm->sp -= 3;
            break;

        case 11: // rdi
            switch (p4vm->store[p4vm->sp].va) {

                case 5:
                    TEMP.f = stdin;
                    *TEMP.name = '\0';
                    readi(p4vm, &TEMP);
                    break;

                case 6:
                    return op;
                    break;

                case 7:
                    readi(p4vm, &(p4vm->prd));
                    break;

                case 8:
                    return op;
                    break;
            }
            break;

        case 12: // rdr
            switch (p4vm->store[p4vm->sp].va) {

                case 5:
                    TEMP.f = stdin;
                    *TEMP.name = '\0';
                    readr(p4vm, &TEMP);
                    break;

                case 6:
                    return op;
                    break;

                case 7:
                    readr(p4vm, &(p4vm->prd));
                    break;

                case 8:
                    return op;
                    break;
            }
            break;

        case 13: // rdc
            switch (p4vm->store[p4vm->sp].va) {

                case 5:
                    TEMP.f = stdin;
                    *TEMP.name = '\0';
                    readc(p4vm, &TEMP);
                    break;

                case 6:
                    return op;
                    break;

                case 7:
                    readc(p4vm, &(p4vm->prd));
                    break;

                case 8:
                    return op;
                    break;
            }
            break;

        case 14: // sin
            p4vm->store[p4vm->sp].vr = sin(p4vm->store[p4vm->sp].vr);
            break;

        case 15: // cos
            p4vm->store[p4vm->sp].vr = cos(p4vm->store[p4vm->sp].vr);
            break;

        case 16: // exp
            p4vm->store[p4vm->sp].vr = exp(p4vm->store[p4vm->sp].vr);
            break;

        case 17: // log
            p4vm->store[p4vm->sp].vr = log(p4vm->store[p4vm->sp].vr);
            break;

        case 18: // sqt
            p4vm->store[p4vm->sp].vr = sqrt(p4vm->store[p4vm->sp].vr);
            break;

        case 19: // atn
            p4vm->store[p4vm->sp].vr = atan(p4vm->store[p4vm->sp].vr);
            break;

        case 20: // sav
            ad = p4vm->store[p4vm->sp].va;
            p4vm->store[ad].va = p4vm->np;
            p4vm->sp--;
            break;
    } // case q

    return 255;
} // callsp

uint8_t p4_vm_interpret(p4_vm_t p4vm) {
    rec_code_t *WITH;
    settype SET;
    long TEMP;
    double TEMP1;
    long FORLIM;

    uint8_t op; //
    uint8_t p;  //
    int16_t q;  // instruction register

    WITH = &(p4vm->code[p4vm->pc / 2]);
    // fetch
    if (p4vm->pc & 1) {
        op = WITH->op2;
        p = WITH->p2;
        q = WITH->q2;
    } else {
        op = WITH->op1;
        p = WITH->p1;
        q = WITH->q1;
    }
    p4vm->pc++;

    // execute
    switch (op) {

        case 105:
        case 106:
        case 107:
        case 108:
        case 109:
        case 0: // lod
            ad = base(p4vm, p) + q;
            p4vm->sp++;
            p4vm->store[p4vm->sp] = p4vm->store[ad];
            break;

        case 65:
        case 66:
        case 67:
        case 68:
        case 69:
        case 1: // ldo
            p4vm->sp++;
            p4vm->store[p4vm->sp] = p4vm->store[q];
            break;

        case 70:
        case 71:
        case 72:
        case 73:
        case 74:
        case 2: // str
            p4vm->store[base(p4vm, p) + q] = p4vm->store[p4vm->sp];
            p4vm->sp--;
            break;

        case 75:
        case 76:
        case 77:
        case 78:
        case 79:
        case 3: // sro
            p4vm->store[q] = p4vm->store[p4vm->sp];
            p4vm->sp--;
            break;

        case 4: // lda
            p4vm->sp++;
            p4vm->store[p4vm->sp].va = base(p4vm, p) + q;
            break;

        case 5: // lao
            p4vm->sp++;
            p4vm->store[p4vm->sp].va = q;
            break;

        case 80:
        case 81:
        case 82:
        case 83:
        case 84:
        case 6: // sto
            p4vm->store[p4vm->store[p4vm->sp - 1].va] = p4vm->store[p4vm->sp];
            p4vm->sp -= 2;
            break;

        case 7: // ldc
            p4vm->sp++;
            if (p == 1)
                p4vm->store[p4vm->sp].vi = q;
            else {
                if (p == 6)
                    p4vm->store[p4vm->sp].vc = q;
                else {
                    if (p == 3)
                        p4vm->store[p4vm->sp].vb = (q == 1);
                    else
                        // load nil
                        p4vm->store[p4vm->sp].va = MAXSTR;
                }
            }
            break;

        case 8: // lci
            p4vm->sp++;
            p4vm->store[p4vm->sp] = p4vm->store[q];
            break;

        case 85:
        case 86:
        case 87:
        case 88:
        case 89:
        case 9: // ind
            ad = p4vm->store[p4vm->sp].va + q;
            // q is a number of storage units
            p4vm->store[p4vm->sp] = p4vm->store[ad];
            break;

        case 90:
        case 91:
        case 92:
        case 93:
        case 94:
        case 10: // inc
            p4vm->store[p4vm->sp].vi += q;
            break;

        case 11: // mst
            // p=level of calling procedure minus level of called procedure + 1;  set dl and sl, increment sp
            // then length of this element is max(intsize,realsize,boolsize,charsize,ptrsize
            p4vm->store[p4vm->sp + 2].vm = base(p4vm, p);
            // the length of this element is ptrsize
            p4vm->store[p4vm->sp + 3].vm = p4vm->mp;
            // idem
            p4vm->store[p4vm->sp + 4].vm = p4vm->ep;
            // idem
            p4vm->sp += 5;
            break;

        case 12: // cup
            // p=no of locations for parameters, q=entry point
            p4vm->mp = p4vm->sp - p - 4;
            p4vm->store[p4vm->mp + 4].vm = p4vm->pc;
            p4vm->pc = q;
            break;

        case 13: // ent
            if (p == 1) {
                p4vm->sp = p4vm->mp + q; // q = length of dataseg
                if (p4vm->sp > p4vm->np)
                    return op;
            } else {
                p4vm->ep = p4vm->sp + q;
                if (p4vm->ep > p4vm->np)
                    return op;
            }
            break;
            // q = max space required on stack

        case 14: // ret
            switch (p) {

                case 0:
                    p4vm->sp = p4vm->mp - 1;
                    break;

                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                    p4vm->sp = p4vm->mp;
                    break;
            }
            p4vm->pc = p4vm->store[p4vm->mp + 4].vm;
            p4vm->ep = p4vm->store[p4vm->mp + 3].vm;
            p4vm->mp = p4vm->store[p4vm->mp + 2].vm;
            break;

        case 15: // csp
            if (callsp(p4vm, q, op) != 255)
                return op;
            break;

        case 16: // ixa
            i = p4vm->store[p4vm->sp].vi;
            p4vm->sp--;
            p4vm->store[p4vm->sp].va += q * i;
            break;

        case 17: // equ
            p4vm->sp--;
            switch (p) {

                case 1:
                    p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].vi == p4vm->store[p4vm->sp + 1].vi);
                    break;

                case 0:
                    p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].va == p4vm->store[p4vm->sp + 1].va);
                    break;

                case 6:
                    p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].vc == p4vm->store[p4vm->sp + 1].vc);
                    break;

                case 2:
                    p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].vr == p4vm->store[p4vm->sp + 1].vr);
                    break;

                case 3:
                    p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].vb == p4vm->store[p4vm->sp + 1].vb);
                    break;

                case 4:
                    p4vm->store[p4vm->sp].vb = p4_fn_setequal(p4vm->store[p4vm->sp].vs, p4vm->store[p4vm->sp + 1].vs);
                    break;

                case 5:
                    compare(p4vm, q);
                    p4vm->store[p4vm->sp].vb = b;
                    break;
            } // case p
            break;

        case 18: // neq
            p4vm->sp--;
            switch (p) {

                case 0:
                    p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].va != p4vm->store[p4vm->sp + 1].va);
                    break;

                case 1:
                    p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].vi != p4vm->store[p4vm->sp + 1].vi);
                    break;

                case 6:
                    p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].vc != p4vm->store[p4vm->sp + 1].vc);
                    break;

                case 2:
                    p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].vr != p4vm->store[p4vm->sp + 1].vr);
                    break;

                case 3:
                    p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].vb != p4vm->store[p4vm->sp + 1].vb);
                    break;

                case 4:
                    p4vm->store[p4vm->sp].vb = !p4_fn_setequal(p4vm->store[p4vm->sp].vs, p4vm->store[p4vm->sp + 1].vs);
                    break;

                case 5:
                    compare(p4vm, q);
                    p4vm->store[p4vm->sp].vb = !b;
                    break;
            } // case p
            break;

        case 19: // geq
            p4vm->sp--;
            switch (p) {

                case 0:
                    return op;
                    break;

                case 1:
                    p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].vi >= p4vm->store[p4vm->sp + 1].vi);
                    break;

                case 6:
                    p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].vc >= p4vm->store[p4vm->sp + 1].vc);
                    break;

                case 2:
                    p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].vr >= p4vm->store[p4vm->sp + 1].vr);
                    break;

                case 3:
                    p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].vb >= p4vm->store[p4vm->sp + 1].vb);
                    break;

                case 4:
                    p4vm->store[p4vm->sp].vb = p4_fn_subset(p4vm->store[p4vm->sp + 1].vs, p4vm->store[p4vm->sp].vs);
                    break;

                case 5:
                    compare(p4vm, q);
                    p4vm->store[p4vm->sp].vb = (b || p4vm->store[i1 + i].vi >= p4vm->store[i2 + i].vi);
                    break;
            } // case p
            break;

        case 20: // grt
            p4vm->sp--;
            switch (p) {

                case 0:
                    return op;
                    break;

                case 1:
                    p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].vi > p4vm->store[p4vm->sp + 1].vi);
                    break;

                case 6:
                    p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].vc > p4vm->store[p4vm->sp + 1].vc);
                    break;

                case 2:
                    p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].vr > p4vm->store[p4vm->sp + 1].vr);
                    break;

                case 3:
                    p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].vb > p4vm->store[p4vm->sp + 1].vb);
                    break;

                case 4:
                    return op;
                    break;

                case 5:
                    compare(p4vm, q);
                    p4vm->store[p4vm->sp].vb = (!b && p4vm->store[i1 + i].vi > p4vm->store[i2 + i].vi);
                    break;
            } // case p
            break;

        case 21: // leq
            p4vm->sp--;
            switch (p) {

                case 0:
                    return op;
                    break;

                case 1:
                    p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].vi <= p4vm->store[p4vm->sp + 1].vi);
                    break;

                case 6:
                    p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].vc <= p4vm->store[p4vm->sp + 1].vc);
                    break;

                case 2:
                    p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].vr <= p4vm->store[p4vm->sp + 1].vr);
                    break;

                case 3:
                    p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].vb <= p4vm->store[p4vm->sp + 1].vb);
                    break;

                case 4:
                    p4vm->store[p4vm->sp].vb = p4_fn_subset(p4vm->store[p4vm->sp].vs, p4vm->store[p4vm->sp + 1].vs);
                    break;

                case 5:
                    compare(p4vm, q);
                    p4vm->store[p4vm->sp].vb = (b || p4vm->store[i1 + i].vi <= p4vm->store[i2 + i].vi);
                    break;
            } // case p
            break;

        case 22: // les
            p4vm->sp--;
            switch (p) {

                case 0:
                    return op;
                    break;

                case 1:
                    p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].vi < p4vm->store[p4vm->sp + 1].vi);
                    break;

                case 6:
                    p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].vc < p4vm->store[p4vm->sp + 1].vc);
                    break;

                case 2:
                    p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].vr < p4vm->store[p4vm->sp + 1].vr);
                    break;

                case 3:
                    p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].vb < p4vm->store[p4vm->sp + 1].vb);
                    break;

                case 5:
                    compare(p4vm, q);
                    p4vm->store[p4vm->sp].vb = (!b && p4vm->store[i1 + i].vi < p4vm->store[i2 + i].vi);
                    break;
            } // case p
            break;

        case 23: // ujp
            p4vm->pc = q;
            break;

        case 24: // fjp
            if (!p4vm->store[p4vm->sp].vb)
                p4vm->pc = q;
            p4vm->sp--;
            break;

        case 25: // xjp
            p4vm->pc = p4vm->store[p4vm->sp].vi + q;
            p4vm->sp--;
            break;

        case 95: // chka
            if (p4vm->store[p4vm->sp].va < p4vm->np || p4vm->store[p4vm->sp].va > MAXSTR - q)
                return op;
            break;

        case 96:
        case 97:
        case 98:
        case 99:
        case 26: // chk
            if (p4vm->store[p4vm->sp].vi < p4vm->store[q - 1].vi || p4vm->store[p4vm->sp].vi > p4vm->store[q].vi)
                return op;
            break;

        case 27: // eof
            i = p4vm->store[p4vm->sp].vi;
            if (i == INPUTADR)
                p4vm->store[p4vm->sp].vb = p4_file_eof(stdin);
            else
                return op;
            break;

        case 28: // adi
            p4vm->sp--;
            p4vm->store[p4vm->sp].vi += p4vm->store[p4vm->sp + 1].vi;
            break;

        case 29: // adr
            p4vm->sp--;
            p4vm->store[p4vm->sp].vr += p4vm->store[p4vm->sp + 1].vr;
            break;

        case 30: // sbi
            p4vm->sp--;
            p4vm->store[p4vm->sp].vi -= p4vm->store[p4vm->sp + 1].vi;
            break;

        case 31: // sbr
            p4vm->sp--;
            p4vm->store[p4vm->sp].vr -= p4vm->store[p4vm->sp + 1].vr;
            break;

        case 32: // sgs
            p4_fn_setcpy(p4vm->store[p4vm->sp].vs, p4_fn_addset(p4_fn_expset(SET, 0), p4vm->store[p4vm->sp].vi));
            break;

        case 33: // flt
            p4vm->store[p4vm->sp].vr = p4vm->store[p4vm->sp].vi;
            break;

        case 34: // flo
            p4vm->store[p4vm->sp - 1].vr = p4vm->store[p4vm->sp - 1].vi;
            break;

        case 35: // trc
            p4vm->store[p4vm->sp].vi = (long) p4vm->store[p4vm->sp].vr;
            break;

        case 36: // ngi
            p4vm->store[p4vm->sp].vi = -p4vm->store[p4vm->sp].vi;
            break;

        case 37: // ngr
            p4vm->store[p4vm->sp].vr = -p4vm->store[p4vm->sp].vr;
            break;

        case 38: // sqi
            TEMP = p4vm->store[p4vm->sp].vi;
            p4vm->store[p4vm->sp].vi = TEMP * TEMP;
            break;

        case 39: // sqr
            TEMP1 = p4vm->store[p4vm->sp].vr;
            p4vm->store[p4vm->sp].vr = TEMP1 * TEMP1;
            break;

        case 40: // abi
            p4vm->store[p4vm->sp].vi = labs(p4vm->store[p4vm->sp].vi);
            break;

        case 41: // abr
            p4vm->store[p4vm->sp].vr = fabs(p4vm->store[p4vm->sp].vr);
            break;

        case 42: // not
            p4vm->store[p4vm->sp].vb = !p4vm->store[p4vm->sp].vb;
            break;

        case 43: // and
            p4vm->sp--;
            p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].vb && p4vm->store[p4vm->sp + 1].vb);
            break;

        case 44: // ior
            p4vm->sp--;
            p4vm->store[p4vm->sp].vb = (p4vm->store[p4vm->sp].vb || p4vm->store[p4vm->sp + 1].vb);
            break;

        case 45: // dif
            p4vm->sp--;
            p4_fn_setdiff(p4vm->store[p4vm->sp].vs, p4vm->store[p4vm->sp].vs, p4vm->store[p4vm->sp + 1].vs);
            break;

        case 46: // int
            p4vm->sp--;
            p4_fn_setint(p4vm->store[p4vm->sp].vs, p4vm->store[p4vm->sp].vs, p4vm->store[p4vm->sp + 1].vs);
            break;

        case 47: // uni
            p4vm->sp--;
            p4_fn_setunion(p4vm->store[p4vm->sp].vs, p4vm->store[p4vm->sp].vs, p4vm->store[p4vm->sp + 1].vs);
            break;

        case 48: // inn
            p4vm->sp--;
            i = p4vm->store[p4vm->sp].vi;
            p4vm->store[p4vm->sp].vb = p4_fn_inset(i, p4vm->store[p4vm->sp + 1].vs);
            break;

        case 49: // mod
            p4vm->sp--;
            p4vm->store[p4vm->sp].vi %= p4vm->store[p4vm->sp + 1].vi;
            break;

        case 50: // odd
            p4vm->store[p4vm->sp].vb = p4vm->store[p4vm->sp].vi & 1;
            break;

        case 51: // mpi
            p4vm->sp--;
            p4vm->store[p4vm->sp].vi *= p4vm->store[p4vm->sp + 1].vi;
            break;

        case 52: // mpr
            p4vm->sp--;
            p4vm->store[p4vm->sp].vr *= p4vm->store[p4vm->sp + 1].vr;
            break;

        case 53: // dvi
            p4vm->sp--;
            p4vm->store[p4vm->sp].vi /= p4vm->store[p4vm->sp + 1].vi;
            break;

        case 54: // dvr
            p4vm->sp--;
            p4vm->store[p4vm->sp].vr /= p4vm->store[p4vm->sp + 1].vr;
            break;

        case 55: // mov
            i1 = p4vm->store[p4vm->sp - 1].va;
            i2 = p4vm->store[p4vm->sp].va;
            p4vm->sp -= 2;
            FORLIM = q;
            for (i = 0; i < FORLIM; i++) {
                p4vm->store[i1 + i] = p4vm->store[i2 + i];
                // q is a number of storage units
            }
            break;

        case 56: // lca
            p4vm->sp++;
            p4vm->store[p4vm->sp].va = q;
            break;

        case 100:
        case 101:
        case 102:
        case 103:
        case 104:
        case 57: // dec
            p4vm->store[p4vm->sp].vi -= q;
            break;

        case 58: // stp
            p4vm->run = false;
            break;

        case 59: // ord
            break;
            // only used to change the tagfield

        case 60: // chr
            break;

        case 61: // ujc
            return op;
            break;
    }

    return 255;
}
