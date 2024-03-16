/*============================================================================

This C source file is part of the SoftFloat IEEE Floating-Point Arithmetic
Package, Release 3e, by John R. Hauser.

Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 The Regents of the
University of California.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
    this list of conditions, and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions, and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

 3. Neither the name of the University nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS", AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ARE
DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

#include <stdbool.h>
#include <stdint.h>
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

uint32_t f32_to_ui32(float32_t a, uint8_t roundingMode, bool exact, struct softfloat_status_t *status)
{
    uint32_t uiA;
    bool sign;
    int16_t exp;
    uint32_t sig;
    uint64_t sig64;
    int16_t shiftDist;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    sign = signF32UI(a);
    exp  = expF32UI(a);
    sig  = fracF32UI(a);
    if (softfloat_denormalsAreZeros(status))
        if (!exp && sig) sig = 0;
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
#if (ui32_fromNaN != ui32_fromPosOverflow) || (ui32_fromNaN != ui32_fromNegOverflow)
    if ((exp == 0xFF) && sig) {
#if (ui32_fromNaN == ui32_fromPosOverflow)
        sign = 0;
#elif (ui32_fromNaN == ui32_fromNegOverflow)
        sign = 1;
#else
        softfloat_raiseFlags(status, softfloat_flag_invalid);
        return ui32_fromNaN;
#endif
    }
#endif
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if (exp) sig |= 0x00800000;
    else if (softfloat_denormalsAreZeros(status)) sig = 0;
    sig64 = (uint64_t) sig<<32;
    shiftDist = 0xAA - exp;
    if (0 < shiftDist) sig64 = softfloat_shiftRightJam64(sig64, shiftDist);
    return softfloat_roundToUI32(sign, sig64, roundingMode, exact, status);
}
