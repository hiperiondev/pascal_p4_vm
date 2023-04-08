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

#include <stddef.h>
#include <setjmp.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "p4_compiler.h"
#include "p4_assembler.h"
#include "p4_internal.h"
#include "p4_vm.h"
#include "p4_file.h"

int main(int argc, char *argv[]) {
    if (strcmp(argv[1], "-c") == 0){
        p4_compiler(argv[2]);
        exit(0);
    }

    p4_vm_t p4vm = malloc(sizeof(struct p4_vm_s));
    uint8_t err;

    printf("- intepreter -\n");

    if (setjmp(_JL1))
        goto _L1;

    prr.f = NULL;
    prd.f = NULL;
    strcpy(prd.name, argv[1]);
    sprintf(prr.name, "%s.p4", prd.name);
    printf("execute: %s (output: %s)\n", prd.name, prr.name);
    if (*prr.name != '\0') {
        if (prr.f != NULL)
            prr.f = freopen(prr.name, "w", prr.f);
        else
            prr.f = fopen(prr.name, "w");
    } else {
        if (prr.f != NULL)
            rewind(prr.f);
        else
            prr.f = tmpfile();
    }
    if (prr.f == NULL)
        _EscIO(FileNotFound);
    prr.f_BFLAGS = 0;
    p4_assembler(p4vm); // assembles and stores code

    p4vm->pc = 0;
    p4vm->sp = -1;
    p4vm->mp = 0;
    p4vm->np = MAXSTK + 1;
    p4vm->ep = 5;

    p4vm->store[INPUTADR].vc = ' ';
    p4vm->store[PRDADR].vc = p4_file_peek(prd.f);
    p4vm->run = true;

    while (p4vm->run) {
        if ((err = p4_vm_interpret(p4vm)) != 255) {
            printf("ERROR op: %d\n", err);
            break;
        }
    } // while interpreting

    _L1:
    if (prd.f != NULL)
        fclose(prd.f);
    if (prr.f != NULL)
        fclose(prr.f);

    free(p4vm);
    return 0;
}
