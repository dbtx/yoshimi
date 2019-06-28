#ifndef TANK_H
#define TANK_H

#include <list>
#include <stdint.h>
#include "Misc/SynthEngine.h"

//
// Surprise, this is more cumbersome than using templates.
// This is the way I do it _because_I_choose_to_
//
// what? ok FINE
// later on I'm going to have to redo everything for OpenCL so there's
// no rationale for taking shortcuts now or using sugary features of
// the language that will not help me there because C++ features
// probably don't belong in compute kernels. They probably do nothing
// very interesting for *arithmetic* anyway, so CL-2.1 support is very
// probably a low priority for the vendors. Probably. I mean I only
// heard about 2.1 in *one* talk that was organized by Khronos but
// *happened* to be given by an nVidia employee [1] and that was
// yeeeaars ago, and nVidia hasn't really embraced (even) CL 2.0 after
// all this time. So yeah, I'm writing C-ish stuff, copmute devices run
// on C-ish stuff, deal with it
//
// and, well, I'm probably just going to redo the native code for the
// entire synth engine in plain C because it's not worth maintaining
// two very similar implementations side by side, and lots of plain C
// code can just be deduplicated and #include'd in different places.
// that's the theory.
//
// [1] https://www.youtube.com/watch?v=ez18sOaR748
//

class LFO;
class LFOParams;
class Envelope;
class EnvelopeParams;
class Filter;
class FilterParams;
class Effect;
class EffectMgr;
class ADnote;
class ADnoteParameters;
class SUBnote;
class SUBnoteParameters;
class PADnote;
class PADnoteParameters;
union CommandBlock;

typedef enum
{
    TB_NONE,
    TB_MONO_BUF,
    TB_STEREO_BUF,
    TB_LFO_OBJ,
    TB_ENV_OBJ,
    TB_FILT_OBJ,
    TB_FX_UNIT,
    TB_FX_MGR,
    TB_ADN_OBJ,
    TB_ADN_PAR,
    TB_SUBN_OBJ,
    TB_SUBN_PAR,
    TB_PADN_OBJ,
    TB_PADN_PAR,
    TB_CMD_BLK
} tb_type;

// a pointer to an array of any object is always the size of a pointer, right?

typedef union
{
    float *f32;
    LFO *lfo;
    Envelope *env;
    Filter *filt;
    Effect *eff;
    EffectMgr *efmg;
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
    uint32_t nodeID;
    tb_data payload;
} tankbit;

typedef struct
{
    uint32_t sz;
    uint32_t *nodeID;
    tb_data *payload;
} tankblock;

class Tank
{
public:
    Tank(tb_type tt, uint32_t sz, SynthEngine *_synth);
    ~Tank();
    void add_tankblock(uint32_t sz);
    void gc();
private:
    SynthEngine *synth;
    tb_type purpose;
    std::list<tankblock *> tb_group;
};

#endif // TANK_H
