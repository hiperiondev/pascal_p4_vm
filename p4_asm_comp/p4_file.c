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

#include <stdio.h>

int p4_file_peek(FILE *f) {
    int ch;

    ch = fgetc(f);
    if (ch == EOF)
        return EOF;
    ungetc(ch, f);
    return (ch == '\n') ? ' ' : ch;
}

// Check if at end of file, using Pascal "eof" semantics. End-of-file for stdin is broken; remove the special case for it to be broken in a different way.
int p4_file_eof(FILE *f) {
    register int ch;

    if (feof(f))
        return 1;
    ch = fgetc(f);
    if (ch == EOF)
        return 1;
    ungetc(ch, f);
    return 0;
}

// Check if at end of line (or end of entire file).
int p4_file_eoln(FILE *f) {
    register int ch;

    ch = fgetc(f);
    if (ch == EOF)
        return 1;
    ungetc(ch, f);
    return (ch == '\n');
}
