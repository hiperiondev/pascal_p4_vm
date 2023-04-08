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

#include <string.h>

#include "p4_internal.h"

#define SETBITS  32

// Sets are stored as an array of longs.  S[0] is the size of the set;
// S[N] is the N'th 32-bit chunk of the set.  S[0] equals the maximum
// I such that S[I] is nonzero.  S[0] is zero for an empty set.  Within
// each long, bits are packed from lsb to msb.  The first bit of the
// set is the element with ordinal value 0.  (Thus, for a "set of 5..99",
// the lowest five bits of the first long are unused and always zero.)
// (Sets with 32 or fewer elements are normally stored as plain longs.)
long* p4_fn_setunion(register long *d, register long *s1, register long *s2) {
    // d := s1 + s2
    long *dbase = d++;
    register int sz1 = *s1++, sz2 = *s2++;
    while (sz1 > 0 && sz2 > 0) {
        *d++ = *s1++ | *s2++;
        sz1--, sz2--;
    }
    while (--sz1 >= 0)
        *d++ = *s1++;
    while (--sz2 >= 0)
        *d++ = *s2++;
    *dbase = d - dbase - 1;
    return dbase;
}

long* p4_fn_setint(register long *d, register long *s1, register long *s2) {
    // d := s1 * s2
    long *dbase = d++;
    register int sz1 = *s1++, sz2 = *s2++;
    while (--sz1 >= 0 && --sz2 >= 0)
        *d++ = *s1++ & *s2++;
    while (--d > dbase && !*d)
        ;
    *dbase = d - dbase;
    return dbase;
}

long* p4_fn_setdiff(register long *d, register long *s1, register long *s2) {
    // d := s1 - s2
    long *dbase = d++;
    register int sz1 = *s1++, sz2 = *s2++;
    while (--sz1 >= 0 && --sz2 >= 0)
        *d++ = *s1++ & ~*s2++;
    if (sz1 >= 0) {
        while (sz1-- >= 0)
            *d++ = *s1++;
    }
    while (--d > dbase && !*d)
        ;
    *dbase = d - dbase;
    return dbase;
}

long* p4_fn_setxor(register long *d, register long *s1, register long *s2) {
    // d := s1 / s2
    long *dbase = d++;
    register int sz1 = *s1++, sz2 = *s2++;
    while (sz1 > 0 && sz2 > 0) {
        *d++ = *s1++ ^ *s2++;
        sz1--, sz2--;
    }
    while (--sz1 >= 0)
        *d++ = *s1++;
    while (--sz2 >= 0)
        *d++ = *s2++;
    while (--d > dbase && !*d)
        ;
    *dbase = d - dbase;
    return dbase;
}

int p4_fn_inset(register unsigned val, register long *s) {
    // val IN s
    register int bit;
    bit = val % SETBITS;
    val /= SETBITS;
    if (val < *s++ && ((1L << bit) & s[val]))
        return 1;
    return 0;
}

long* p4_fn_addset(register long *s, register unsigned val) {
    // s := s + [val]
    register long *sbase = s;
    register int bit, size;
    bit = val % SETBITS;
    val /= SETBITS;
    size = *s;
    if (++val > size) {
        s += size;
        while (val > size)
            *++s = 0, size++;
        *sbase = size;
    } else
        s += val;
    *s |= 1L << bit;
    return sbase;
}

long* p4_fn_addsetr(register long *s, register unsigned v1, register unsigned v2) {
    // s := s + [v1..v2]
    register long *sbase = s;
    register int b1, b2, size;
    if ((int) v1 > (int) v2)
        return sbase;
    b1 = v1 % SETBITS;
    v1 /= SETBITS;
    b2 = v2 % SETBITS;
    v2 /= SETBITS;
    size = *s;
    v1++;
    if (++v2 > size) {
        while (v2 > size)
            s[++size] = 0;
        s[v2] = 0;
        *s = v2;
    }
    s += v1;
    if (v1 == v2) {
        *s |= (~((-2L) << (b2 - b1))) << b1;
    } else {
        *s++ |= (-1L) << b1;
        while (++v1 < v2)
            *s++ = -1;
        *s |= ~((-2L) << b2);
    }
    return sbase;
}

long* p4_fn_remset(register long *s, register unsigned val) {
    // s := s - [val]
    register int bit;
    bit = val % SETBITS;
    val /= SETBITS;
    if (++val <= *s) {
        if (!(s[val] &= ~(1L << bit)))
            while (*s && !s[*s])
                (*s)--;
    }
    return s;
}

int p4_fn_setequal(register long *s1, register long *s2) {
    // s1 = s2
    register int size = *s1++;
    if (*s2++ != size)
        return 0;
    while (--size >= 0) {
        if (*s1++ != *s2++)
            return 0;
    }
    return 1;
}

int p4_fn_subset(register long *s1, register long *s2) {
    // s1 <= s2
    register int sz1 = *s1++, sz2 = *s2++;
    if (sz1 > sz2)
        return 0;
    while (--sz1 >= 0) {
        if (*s1++ & ~*s2++)
            return 0;
    }
    return 1;
}

long* p4_fn_setcpy(register long *d, register long *s) {
    // d := s
    register long *save_d = d;
    memcpy(d, s, (*s + 1) * sizeof(long));

    return save_d;
}

// s is a "smallset", i.e., a 32-bit or less set stored directly in a long.
long* p4_fn_expset(register long *d, register long s) {
    // d := s
    if (s) {
        d[1] = s;
        *d = 1;
    } else
        *d = 0;
    return d;
}
