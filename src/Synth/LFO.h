/*
    LFO.h - LFO implementation

    Original ZynAddSubFX author Nasca Octavian Paul
    Copyright (C) 2002-2005 Nasca Octavian Paul
    Copyright 2009-2011, Alan Calvert
    Copyright 2017, Will Godfrey & others

    This file is part of yoshimi, which is free software: you can redistribute
    it and/or modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either version 2 of
    the License, or (at your option) any later version.

    yoshimi is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.   See the GNU General Public License (version 2 or
    later) for more details.

    You should have received a copy of the GNU General Public License along with
    yoshimi; if not, write to the Free Software Foundation, Inc., 51 Franklin
    Street, Fifth Floor, Boston, MA  02110-1301, USA.

    This file is derivative of ZynAddSubFX original code
    Modified February 2017
*/

#ifndef LFO_H
#define LFO_H

#include "Params/LFOParams.h"

class SynthEngine;

class LFO
{
            // constructor is bogus. use prep C-STYLE WOOO
    // they get malloced. they sit around.
    // i run LFOtank_>grab(nodeID) where nodeID is the address of this LFO spot
    //  in all of the ADsynth screens. probably need to take part of the addy
    //    away and dedicate it to the *notes* maybe 8 bits is enough. UGH FIXME
    // LFOtank finds a good slot and writes my nodeid into it. and sends me
    // an ID to submit later to get a pointer? a pointer to have to worry about
    //   updating later? not quite sure
    // i run ptr->prepLFO() from the FOOnote that would've new'd it before.
    // (or in C, prepLFO(ptr, ...) )
    // the nodeid that gets passed
    public:
        LFO(LFOParams *_lfopars, float basefreq, SynthEngine *_synth);
        ~LFO() {}
        void prepLFO(uint32_t nodeID, LFOParams *_lfopars, float basefreq, SynthEngine *_synth);
        float lfoout(void);
        float amplfoout(void);
    private:
        LFOParams *lfopars;
        void Recompute(void);
        void RecomputeFreq(void);
        void computenextincrnd(void);
        float x;
        float basefreq;
        float incx, incrnd, nextincrnd;
        float amp1, amp2; // used for randomness
        float lfointensity;
        float lfornd;
        float lfofreqrnd;
        float lfodelay;
        char lfotype;
        int freqrndenabled;

        SynthEngine *synth;
};

#endif
