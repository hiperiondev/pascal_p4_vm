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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "p4_internal.h"
#include "p4_file.h"

jmp_buf _JL1;

static local_jmp_buf_t *__top_jb;
file_t prd, prr; // prd for read only, prr for write only
static short P_escapecode;
static int P_ioresult;

// The following is suitable for the HP Pascal operating system. It might want to be revised when emulating another system.
char* _ShowEscape(char *buf, int code, int ior, char *prefix) {
    char *bufp;

    if (prefix && *prefix) {
        strcpy(buf, prefix);
        strcat(buf, ": ");
        bufp = buf + strlen(buf);
    } else {
        bufp = buf;
    }
    if (code == -10) {
        sprintf(bufp, "Pascal system I/O error %d", ior);
        switch (ior) {
            case 3:
                strcat(buf, " (illegal I/O request)");
                break;
            case 7:
                strcat(buf, " (bad file name)");
                break;
            case FileNotFound: //10
                strcat(buf, " (file not found)");
                break;
            case FileNotOpen: //13
                strcat(buf, " (file not open)");
                break;
            case BadInputFormat: //14
                strcat(buf, " (bad input format)");
                break;
            case 24:
                strcat(buf, " (not open for reading)");
                break;
            case 25:
                strcat(buf, " (not open for writing)");
                break;
            case 26:
                strcat(buf, " (not open for direct access)");
                break;
            case 28:
                strcat(buf, " (string subscript out of range)");
                break;
            case EndOfFile: //30
                strcat(buf, " (end-of-file)");
                break;
            case FileWriteError: //38
                strcat(buf, " (file write error)");
                break;
        }
    } else {
        sprintf(bufp, "Pascal system error %d", code);
        switch (code) {
            case -2:
                strcat(buf, " (out of memory)");
                break;
            case -3:
                strcat(buf, " (reference to NIL pointer)");
                break;
            case -4:
                strcat(buf, " (integer overflow)");
                break;
            case -5:
                strcat(buf, " (divide by zero)");
                break;
            case -6:
                strcat(buf, " (real math overflow)");
                break;
            case -8:
                strcat(buf, " (value range error)");
                break;
            case -9:
                strcat(buf, " (CASE value range error)");
                break;
            case -12:
                strcat(buf, " (bus error)");
                break;
            case -20:
                strcat(buf, " (stopped by user)");
                break;
        }
    }
    return buf;
}

int _Escape(int code) {
    char buf[100];

    P_escapecode = code;
    if (__top_jb) {
        local_jmp_buf_t *jb = __top_jb;
        __top_jb = jb->next;
        longjmp(jb->jbuf, 1);
    }
    if (code == 0)
        exit(EXIT_SUCCESS);
    if (code == -1)
        exit(EXIT_FAILURE);
    fprintf(stderr, "%s\n", _ShowEscape(buf, P_escapecode, P_ioresult, ""));
    exit(EXIT_FAILURE);
}

int _EscIO(int code) {
    P_ioresult = code;
    return _Escape(-10);
}
