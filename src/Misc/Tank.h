#ifndef TANK_H
#define TANK_H

#include <list>
#include <stdint.h>

//
// Surprise, this is more cumbersome than using templates.
// This is the way I do it _because_I_choose_to_
//
// what? ok FINE
// later on I'm going to have to redo everything for OpenCL so there's
// no rationale for taking shortcuts now or using sugary features of
// the language that will not help me there because C++ features
// probably don't belong in compute kernels, they probably do nothing
// very interesting for *arithmetic* anyway, so CL-2.1 support is very
// probably a low priority for the vendors. Probably. I mean I only
// heard about 2.1 in *one* talk by an nVidia employee [1] and that was
// yeeeaars ago, and nVidia hasn't really (even) embraced CL 2.0 after
// all this time. So yeah, I'm writing C-ish stuff, deal with it
//
// [1] https://www.youtube.com/watch?v=ez18sOaR748
//

class LFO;
class Envelope;
class Filter;
class Effect;
class EffectMgr;
class ADnote;
class ADnoteParameters;
class SUBnote;
class SUBnoteParameters;
class PADnote;
class PADnoteParameters;
class CommandBlock;

typedef enum
{
    NONE,
    MONO_BUF,
    STEREO_BUF,
    LFO_OBJ,
    ENV_OBJ,
    FILT_OBJ,
    FX_UNIT,
    FX_MGR,
    ADN_OBJ,
    ADN_PAR,
    SUBN_OBJ,
    SUBN_PAR,
    PADN_OBJ,
    PADN_PAR,
    CMD_BLK
} tb_type;

typedef union
{
    float *f32;
    LFO *lfo;
    Envelope *env;
    Filter *filt;
    Effect *eff;
    EffectMgr *effmgr;
    ADnote *adn;
    ADnoteParameters *adp;
    SUBnote *subn;
    SUBnoteParameters *subp;
    PADnote *padn;
    PADnoteParameters *padp;
    CommandBlock *cb;
} tb_data;

typedef struct
{
    uint32_t sz;
    tb_type flavour;
    tb_data tankbits;
} section;

class Tank
{
public:
    Tank(tb_type tt, uint32_t sz);
private:
    std::list<section> subs;
};

#endif // TANK_H
