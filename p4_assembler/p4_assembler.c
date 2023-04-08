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

#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "p4_assembler.h"
#include "p4_internal.h"
#include "p4_functions.h"
#include "p4_file.h"
#include "p4_vm.h"

static uint8_t op;
static short q; // instruction register
static short pc; // program address register
static uint8_t p;
static alfa_ instr[128]; // mnemonic instruction codes
static long cop[128];
static alfa_ sptable[21]; // standard functions and procedures

static void getname(loc_assemble_t *LINK) {
    LINK->LINK->word[0] = LINK->LINK->ch;
    LINK->LINK->word[1] = getc(prd.f);
    LINK->LINK->word[2] = getc(prd.f);
    if (LINK->LINK->word[1] == '\n')
        LINK->LINK->word[1] = ' ';
    if (LINK->LINK->word[2] == '\n')
        LINK->LINK->word[2] = ' ';
    if (!p4_file_eoln(prd.f)) {
        LINK->LINK->ch = getc(prd.f); // next character
        if (LINK->LINK->ch == '\n')
            LINK->LINK->ch = ' ';
    }
    memcpy(LINK->name, LINK->LINK->word, sizeof(alfa_));
} // getname

static void typesymbol(loc_assemble_t *LINK) {
    long i = 0;

    // typesymbol
    if (LINK->LINK->ch == 'i')
        return;
    switch (LINK->LINK->ch) {

        case 'a':
            i = 0;
            break;

        case 'r':
            i = 1;
            break;

        case 's':
            i = 2;
            break;

        case 'b':
            i = 3;
            break;

        case 'c':
            i = 4;
            break;
    }
    op = cop[op] + i;
}

static void lookup(p4_vm_t p4vm, short x, loc_assemble_t *LINK) {
    // search in label table
    switch (LINK->LINK->labeltab[x].st) {

        case ENTERED:
            q = LINK->LINK->labeltab[x].val;
            LINK->LINK->labeltab[x].val = pc;
            break;

        case DEFINED:
            q = LINK->LINK->labeltab[x].val;
            break;
    } // case label..
} // lookup

static void labelsearch(p4_vm_t p4vm, loc_assemble_t *LINK) {
    short x;

    while ((LINK->LINK->ch != 'l') & (!p4_file_eoln(prd.f))) {
        LINK->LINK->ch = getc(prd.f);
        if (LINK->LINK->ch == '\n')
            LINK->LINK->ch = ' ';
    }
    fscanf(prd.f, "%hd", &x);
    lookup(p4vm, x, LINK);
} // labelsearch

static void assemble(p4_vm_t p4vm, loc_load_t *LINK) {
    // translate symbolic code into machine code and store
    loc_assemble_t V;
    double r;
    settype s;
    long i, s1, lb, ub;
    int TEMP;
    rec_code_t *WITH;

    V.LINK = LINK;
    p = 0;
    q = 0;
    op = 0;
    getname(&V);
    memcpy(instr[DUMINST], V.name, sizeof(alfa_));
    while (strncmp(instr[op], V.name, sizeof(alfa_)))
        op++;
    if (op == DUMINST)
        _errorl(" illegal instruction     ", LINK);

    switch (op) { // get parameters p,q

        // equ,neq,geq,grt,leq,les
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
            switch (LINK->ch) {

                case 'a': // p = 0
                    break;

                case 'i':
                    p = 1;
                    break;

                case 'r':
                    p = 2;
                    break;

                case 'b':
                    p = 3;
                    break;

                case 's':
                    p = 4;
                    break;

                case 'c':
                    p = 6;
                    break;

                case 'm':
                    p = 5;
                    fscanf(prd.f, "%hd", &q);
                    break;
            }
            break;

            // lod,str
        case 0:
        case 2:
            typesymbol(&V);
            fscanf(prd.f, "%d%hd", &TEMP, &q);
            p = TEMP;
            break;

        case 4: // lda
            fscanf(prd.f, "%d%hd", &TEMP, &q);
            p = TEMP;
            break;

        case 12: // cup
            fscanf(prd.f, "%d", &TEMP);
            p = TEMP;
            labelsearch(p4vm, &V);
            break;

        case 11: // mst
            fscanf(prd.f, "%d", &TEMP);
            p = TEMP;
            break;

        case 14: // ret
            switch (LINK->ch) {

                case 'p':
                    p = 0;
                    break;

                case 'i':
                    p = 1;
                    break;

                case 'r':
                    p = 2;
                    break;

                case 'c':
                    p = 3;
                    break;

                case 'b':
                    p = 4;
                    break;

                case 'a':
                    p = 5;
                    break;
            }
            break;

            // lao,ixa,mov
        case 5:
        case 16:
        case 55:
            fscanf(prd.f, "%hd", &q);
            break;

            // ldo,sro,ind,inc,dec
        case 1:
        case 3:
        case 9:
        case 10:
        case 57:
            typesymbol(&V);
            fscanf(prd.f, "%hd", &q);
            break;

            // ujp,fjp,xjp
        case 23:
        case 24:
        case 25:
            labelsearch(p4vm, &V);
            break;

        case 13: // ent
            fscanf(prd.f, "%d", &TEMP);
            p = TEMP;
            labelsearch(p4vm, &V);
            break;

        case 15: // csp
            for (i = 1; i <= 9; i++) {
                LINK->ch = getc(prd.f);
                if (LINK->ch == '\n')
                    LINK->ch = ' ';
            }
            getname(&V);
            while (strncmp(V.name, sptable[q], sizeof(alfa_)))
                q++;
            break;

        case 7: // ldc
            switch (LINK->ch) { // get q

                case 'i':
                    p = 1;
                    fscanf(prd.f, "%ld", &i);
                    if (labs(i) >= LARGEINT) {
                        op = 8;
                        p4vm->store[LINK->icp].vi = i;
                        q = MAXSTK;
                        do {
                            q++;
                        } while (p4vm->store[q].vi != i);
                        if (q == LINK->icp) {
                            LINK->icp++;
                            if (LINK->icp == OVERI)
                                _errorl(" integer table overflow  ", LINK);
                        }
                    } else
                        q = i;
                    break;

                case 'r':
                    op = 8;
                    p = 2;
                    fscanf(prd.f, "%lg", &r);
                    p4vm->store[LINK->rcp].vr = r;
                    q = OVERI;
                    do {
                        q++;
                    } while (p4vm->store[q].vr != r);
                    if (q == LINK->rcp) {
                        LINK->rcp++;
                        if (LINK->rcp == OVERR)
                            _errorl(" real table overflow     ", LINK);
                    }
                    break;

                case 'n': // p,q = 0
                    break;

                case 'b':
                    p = 3;
                    fscanf(prd.f, "%hd", &q);
                    break;

                case 'c':
                    p = 6;
                    do {
                        LINK->ch = getc(prd.f);
                        if (LINK->ch == '\n')
                            LINK->ch = ' ';
                    } while (LINK->ch == ' ');
                    if (LINK->ch != '\'')
                        _errorl(" illegal character       ", LINK);
                    LINK->ch = getc(prd.f);
                    if (LINK->ch == '\n')
                        LINK->ch = ' ';
                    q = LINK->ch;
                    LINK->ch = getc(prd.f);
                    if (LINK->ch == '\n')
                        LINK->ch = ' ';
                    if (LINK->ch != '\'')
                        _errorl(" illegal character       ", LINK);
                    break;

                case '(':
                    op = 8;
                    p = 4;
                    p4_fn_expset(s, 0);
                    LINK->ch = getc(prd.f);
                    if (LINK->ch == '\n')
                        LINK->ch = ' ';
                    while (LINK->ch != ')') {
                        fscanf(prd.f, "%ld%c", &s1, &LINK->ch);
                        if (LINK->ch == '\n')
                            LINK->ch = ' ';
                        p4_fn_addset(s, s1);
                    }
                    p4_fn_setcpy(p4vm->store[LINK->scp].vs, s);
                    q = OVERR;
                    do {
                        q++;
                    } while (!p4_fn_setequal(p4vm->store[q].vs, s));
                    if (q == LINK->scp) {
                        LINK->scp++;
                        if (LINK->scp == OVERS)
                            _errorl(" set table overflow      ", LINK);
                    }
                    break;
            } // case
            break;

        case 26: // chk
            typesymbol(&V);
            fscanf(prd.f, "%ld%ld", &lb, &ub);
            if (op == 95)
                q = lb;
            else {
                p4vm->store[LINK->bcp - 1].vi = lb;
                p4vm->store[LINK->bcp].vi = ub;
                q = OVERS;
                do {
                    q += 2;
                } while (p4vm->store[q - 1].vi != lb || p4vm->store[q].vi != ub);
                if (q == LINK->bcp) {
                    LINK->bcp += 2;
                    if (LINK->bcp == OVERB)
                        _errorl(" boundary table overflow ", LINK);
                }
            }
            break;

        case 56: // lca
            if (LINK->mcp + 16 >= OVERM)
                _errorl(" multiple table overflow ", LINK);
            LINK->mcp += 16;
            q = LINK->mcp;
            for (i = 0; i <= 15; i++) { // stringlgth
                LINK->ch = getc(prd.f);
                if (LINK->ch == '\n')
                    LINK->ch = ' ';
                p4vm->store[q + i].vc = LINK->ch;
            }
            break;

        case 6: // sto
            typesymbol(&V);
            break;

        case 27:
        case 28:
        case 29:
        case 30:
        case 31:
        case 32:
        case 33:
        case 34:
        case 35:
        case 36:
        case 37:
        case 38:
        case 39:
        case 40:
        case 41:
        case 42:
        case 43:
        case 44:
        case 45:
        case 46:
        case 47:
        case 48:
        case 49:
        case 50:
        case 51:
        case 52:
        case 53:
        case 54:
        case 58:
            break;

            // ord,chr
        case 59:
        case 60:
            goto _L1;
            break;

        case 61: // ujc
            break;
            // must have same length as ujp

    } // case

    WITH = &(p4vm->code[pc / 2]);
    // store instruction
    if (pc & 1) {
        WITH->op2 = op;
        WITH->p2 = p;
        WITH->q2 = q;
    } else {
        WITH->op1 = op;
        WITH->p1 = p;
        WITH->q1 = q;
    }
    pc++;
    _L1:
    fscanf(prd.f, "%*[^\n]");
    getc(prd.f);
} // assemble

static void update(p4_vm_t p4vm, short x, loc_load_t *LINK) {
    // when a label definition lx is found
    short curr, succ;
    // resp. current element and successor element of a list of future references
    bool endlist;
    rec_code_t *WITH;

    if (LINK->labeltab[x].st == DEFINED) {
        _errorl(" duplicated label\t       ", LINK);
        return;
    }
    if (LINK->labeltab[x].val != -1) { // forward reference(s)
        curr = LINK->labeltab[x].val;
        endlist = false;
        while (!endlist) {
            WITH = &(p4vm->code[curr / 2]);
            if (curr & 1) {
                succ = WITH->q2;
                WITH->q2 = LINK->labelvalue;
            } else {
                succ = WITH->q1;
                WITH->q1 = LINK->labelvalue;
            }
            if (succ == -1)
                endlist = true;
            else
                curr = succ;
        }
    }
    LINK->labeltab[x].st = DEFINED;
    LINK->labeltab[x].val = LINK->labelvalue;
} // update

static void generate(p4_vm_t p4vm, loc_load_t *LINK) {
    // generate segment of code
    long x; // label number
    bool again;

    again = true;
    while (again) {
        LINK->ch = getc(prd.f); // first character of line
        if (LINK->ch == '\n')
            LINK->ch = ' ';
        switch (LINK->ch) {

            case 'i':
                fscanf(prd.f, "%*[^\n]");
                getc(prd.f);
                break;

            case 'l':
                fscanf(prd.f, "%ld", &x);
                if (!p4_file_eoln(prd.f)) {
                    LINK->ch = getc(prd.f);
                    if (LINK->ch == '\n')
                        LINK->ch = ' ';
                }
                if (LINK->ch == '=')
                    fscanf(prd.f, "%hd", &LINK->labelvalue);
                else
                    LINK->labelvalue = pc;
                update(p4vm, x, LINK);
                fscanf(prd.f, "%*[^\n]");
                getc(prd.f);
                break;

            case 'q':
                again = false;
                fscanf(prd.f, "%*[^\n]");
                getc(prd.f);
                break;

            case ' ':
                LINK->ch = getc(prd.f);
                if (LINK->ch == '\n')
                    LINK->ch = ' ';
                assemble(p4vm, LINK);
                break;
        }
    }
} // generate

static void init(p4_vm_t p4vm, loc_load_t *LINK) {
    long i;
    labelrec_t *WITH;

    memcpy(instr[0], "lod       ", sizeof(alfa_));
    memcpy(instr[1], "ldo       ", sizeof(alfa_));
    memcpy(instr[2], "str       ", sizeof(alfa_));
    memcpy(instr[3], "sro       ", sizeof(alfa_));
    memcpy(instr[4], "lda       ", sizeof(alfa_));
    memcpy(instr[5], "lao       ", sizeof(alfa_));
    memcpy(instr[6], "sto       ", sizeof(alfa_));
    memcpy(instr[7], "ldc       ", sizeof(alfa_));
    memcpy(instr[8], "...       ", sizeof(alfa_));
    memcpy(instr[9], "ind       ", sizeof(alfa_));
    memcpy(instr[10], "inc       ", sizeof(alfa_));
    memcpy(instr[11], "mst       ", sizeof(alfa_));
    memcpy(instr[12], "cup       ", sizeof(alfa_));
    memcpy(instr[13], "ent       ", sizeof(alfa_));
    memcpy(instr[14], "ret       ", sizeof(alfa_));
    memcpy(instr[15], "csp       ", sizeof(alfa_));
    memcpy(instr[16], "ixa       ", sizeof(alfa_));
    memcpy(instr[17], "equ       ", sizeof(alfa_));
    memcpy(instr[18], "neq       ", sizeof(alfa_));
    memcpy(instr[19], "geq       ", sizeof(alfa_));
    memcpy(instr[20], "grt       ", sizeof(alfa_));
    memcpy(instr[21], "leq       ", sizeof(alfa_));
    memcpy(instr[22], "les       ", sizeof(alfa_));
    memcpy(instr[23], "ujp       ", sizeof(alfa_));
    memcpy(instr[24], "fjp       ", sizeof(alfa_));
    memcpy(instr[25], "xjp       ", sizeof(alfa_));
    memcpy(instr[26], "chk       ", sizeof(alfa_));
    memcpy(instr[27], "eof       ", sizeof(alfa_));
    memcpy(instr[28], "adi       ", sizeof(alfa_));
    memcpy(instr[29], "adr       ", sizeof(alfa_));
    memcpy(instr[30], "sbi       ", sizeof(alfa_));
    memcpy(instr[31], "sbr       ", sizeof(alfa_));
    memcpy(instr[32], "sgs       ", sizeof(alfa_));
    memcpy(instr[33], "flt       ", sizeof(alfa_));
    memcpy(instr[34], "flo       ", sizeof(alfa_));
    memcpy(instr[35], "trc       ", sizeof(alfa_));
    memcpy(instr[36], "ngi       ", sizeof(alfa_));
    memcpy(instr[37], "ngr       ", sizeof(alfa_));
    memcpy(instr[38], "sqi       ", sizeof(alfa_));
    memcpy(instr[39], "sqr       ", sizeof(alfa_));
    memcpy(instr[40], "abi       ", sizeof(alfa_));
    memcpy(instr[41], "abr       ", sizeof(alfa_));
    memcpy(instr[42], "not       ", sizeof(alfa_));
    memcpy(instr[43], "and       ", sizeof(alfa_));
    memcpy(instr[44], "ior       ", sizeof(alfa_));
    memcpy(instr[45], "dif       ", sizeof(alfa_));
    memcpy(instr[46], "int       ", sizeof(alfa_));
    memcpy(instr[47], "uni       ", sizeof(alfa_));
    memcpy(instr[48], "inn       ", sizeof(alfa_));
    memcpy(instr[49], "mod       ", sizeof(alfa_));
    memcpy(instr[50], "odd       ", sizeof(alfa_));
    memcpy(instr[51], "mpi       ", sizeof(alfa_));
    memcpy(instr[52], "mpr       ", sizeof(alfa_));
    memcpy(instr[53], "dvi       ", sizeof(alfa_));
    memcpy(instr[54], "dvr       ", sizeof(alfa_));
    memcpy(instr[55], "mov       ", sizeof(alfa_));
    memcpy(instr[56], "lca       ", sizeof(alfa_));
    memcpy(instr[57], "dec       ", sizeof(alfa_));
    memcpy(instr[58], "stp       ", sizeof(alfa_));
    memcpy(instr[59], "ord       ", sizeof(alfa_));
    memcpy(instr[60], "chr       ", sizeof(alfa_));
    memcpy(instr[61], "ujc       ", sizeof(alfa_));

    memcpy(sptable[0], "get       ", sizeof(alfa_));
    memcpy(sptable[1], "put       ", sizeof(alfa_));
    memcpy(sptable[2], "rst       ", sizeof(alfa_));
    memcpy(sptable[3], "rln       ", sizeof(alfa_));
    memcpy(sptable[4], "new       ", sizeof(alfa_));
    memcpy(sptable[5], "wln       ", sizeof(alfa_));
    memcpy(sptable[6], "wrs       ", sizeof(alfa_));
    memcpy(sptable[7], "eln       ", sizeof(alfa_));
    memcpy(sptable[8], "wri       ", sizeof(alfa_));
    memcpy(sptable[9], "wrr       ", sizeof(alfa_));
    memcpy(sptable[10], "wrc       ", sizeof(alfa_));
    memcpy(sptable[11], "rdi       ", sizeof(alfa_));
    memcpy(sptable[12], "rdr       ", sizeof(alfa_));
    memcpy(sptable[13], "rdc       ", sizeof(alfa_));
    memcpy(sptable[14], "sin       ", sizeof(alfa_));
    memcpy(sptable[15], "cos       ", sizeof(alfa_));
    memcpy(sptable[16], "exp       ", sizeof(alfa_));
    memcpy(sptable[17], "log       ", sizeof(alfa_));
    memcpy(sptable[18], "sqt       ", sizeof(alfa_));
    memcpy(sptable[19], "atn       ", sizeof(alfa_));
    memcpy(sptable[20], "sav       ", sizeof(alfa_));

    cop[0] = 105;
    cop[1] = 65;
    cop[2] = 70;
    cop[3] = 75;
    cop[6] = 80;
    cop[9] = 85;
    cop[10] = 90;
    cop[26] = 95;
    cop[57] = 100;

    pc = BEGINCODE;
    LINK->icp = MAXSTK + 1;
    LINK->rcp = OVERI + 1;
    LINK->scp = OVERR + 1;
    LINK->bcp = OVERS + 2;
    LINK->mcp = OVERB + 1;
    for (i = 0; i <= 9; i++)
        LINK->word[i] = ' ';
    for (i = 0; i <= MAXLABEL; i++) {
        WITH = &LINK->labeltab[i];
        WITH->val = -1;
        WITH->st = ENTERED;
    }
    if (*prd.name != '\0') {
        if (prd.f != NULL)
            prd.f = freopen(prd.name, "r", prd.f);
        else
            prd.f = fopen(prd.name, "r");
    } else
        rewind(prd.f);
    if (prd.f == NULL)
        _EscIO(FileNotFound);
    prd.f_BFLAGS = 1;
} // init

void p4_assembler(p4_vm_t p4vm) {
    loc_load_t V;

    init(p4vm, &V);
    generate(p4vm, &V);
    pc = 0;
    generate(p4vm, &V);
} // load
