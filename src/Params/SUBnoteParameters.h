/*
    SUBnoteParameters.h - Parameters for SUBnote (SUBsynth)

    Original ZynAddSubFX author Nasca Octavian Paul
    Copyright (C) 2002-2005 Nasca Octavian Paul
    Copyright 2009, Alan Calvert

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

    This file is derivative of ZynAddSubFX original code, modified February 2017
*/

#ifndef SUB_NOTE_PARAMETERS_H
#define SUB_NOTE_PARAMETERS_H

#include "Misc/XMLwrapper.h"
#include "Params/EnvelopeParams.h"
#include "Params/FilterParams.h"
#include "Params/Presets.h"

class SynthEngine;
class LFOParams;

class SUBnoteParameters : public Presets
{
    public:
        SUBnoteParameters(SynthEngine *_synth);
        ~SUBnoteParameters();
        void setPan(char pan);
        bool randomPan(void) { return !PPanning; }
        void add2XML(XMLwrapper *xml);
        void defaults(void);
        void getfromXML(XMLwrapper *xml);
        void getLimits(CommandBlock *getData);
        void updateFrequencyMultipliers(void);
        void postrender(void);

        // Amplitude Parametrers
        bool Pstereo; // true = stereo, false = mono
        unsigned char PVolume;
        unsigned char PPanning;
        float pangainL;         // derived from PPanning
        float pangainR;         // ^^
        unsigned char PAmpVelocityScaleFunction;
        EnvelopeParams *AmpEnvelope;
        LFOParams *AmpLfo;

        unsigned char PPunchStrength, PPunchTime, PPunchStretch, PPunchVelocitySensing;

        // Frequency Parameters
        unsigned short int PDetune;
        unsigned short int PCoarseDetune;
        unsigned char PDetuneType;
        EnvelopeParams *FreqEnvelope;
        unsigned char PFreqEnvelopeEnabled;
        LFOParams *FreqLfo;
        unsigned char PFreqLfoEnabled;
        EnvelopeParams *BandWidthEnvelope;
        unsigned char PBandWidthEnvelopeEnabled;
        LFOParams *BandWidthLfo;
        unsigned char PBandWidthLfoEnabled;

        unsigned char PBendAdjust; // Pitch Bend
        unsigned char POffsetHz;

        // Filter Parameters (Global)
        unsigned char PGlobalFilterEnabled;
        FilterParams *GlobalFilter;
        unsigned char PGlobalFilterVelocityScale;
        unsigned char PGlobalFilterVelocityScaleFunction;
        EnvelopeParams *GlobalFilterEnvelope;
        LFOParams *GlobalFilterLfo;

        // Overtone time-based modifiers
        unsigned char POvertonePar1LfoEnabled;
        unsigned char POvertonePar1EnvEnabled;
        unsigned char POvertonePar2LfoEnabled;
        unsigned char POvertonePar2EnvEnabled;
        EnvelopeParams *OvertonePar1Env;
        EnvelopeParams *OvertonePar2Env;
        LFOParams *OvertonePar1Lfo;
        LFOParams *OvertonePar2Lfo;

        // Other Parameters
        unsigned char Pfixedfreq;   // If the base frequency is fixed to 440 Hz

        unsigned char PfixedfreqET; // Equal temperate (this is used only if the
                                    // Pfixedfreq is enabled)
                                    // If this parameter is 0, the frequency is
                                    // fixed (to 440 Hz)
                                    // if this parameter is 64,
                                    // 1 MIDI halftone -> 1 frequency halftone
        bool profileupdated;
        bool overtoneupdated;

        // Overtone parameter struct (type=0,p1=1,p2=2,p3=3)
        unsigned char  POvertoneSpread[4];
        float POvertoneFreqMult[MAX_SUB_HARMONICS];

        unsigned char Pnumstages;   // how many times the filters are applied
        unsigned char Pbandwidth;

        unsigned char Phmagtype;    // how the magnitudes are computed
                                    // 0 = linear, 1 = -60dB, 2 = -60dB

        unsigned char Phmag[MAX_SUB_HARMONICS];   // Magnitudes

        unsigned char Phrelbw[MAX_SUB_HARMONICS]; // Relative BandWidth ("64"=1.0)

        unsigned char Pbwscale; // how much the bandwidth is increased according
                                // to lower/higher frequency; 64-default

        unsigned char Pstart;   // how the harmonics start, "0" = 0, "1" = random, "2" = 1

        void setPhmag(int harmonic, int value) {Phmag[harmonic] = value; profileupdated = true; }
        void setPhrelbw(int harmonic, int value) {Phrelbw[harmonic] = value; profileupdated = true; }
        void setOvtPar(int p, unsigned char value) { POvertoneSpread[p] = value; overtoneupdated = true; }
};

#endif



