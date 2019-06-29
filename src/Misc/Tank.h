#ifndef TANK_H
#define TANK_H

#include <list>
#include <stdint.h>
#include "Misc/SynthEngine.h"

// Surprise, this is somewhat more cumbersome than using templates.
// This is the way I do it _because_I_choose_to_
//
// what? ok FINE
// later on I'm going to have to redo everything for OpenCL so there's
// no rationale for taking shortcuts now or using sugary features of
// this language. Those will not help me there because C++ features
// probably don't belong in compute kernels. They probably do nothing
// very interesting for *arithmetic* anyway, so CL-2.1 support is very
// probably a low priority for the vendors. Probably. I mean I only
// heard about 2.1 in *one* talk that was organized by Khronos but
// *happened* to be given by an nVidia employee [1] and that was
// yeeeaars ago, and nVidia hasn't really embraced (even) CL 2.0 after
// all this time. So yeah, I'm writing C-ish stuff, compute devices run
// on C-ish stuff, deal with it
//
// and, well, I'm probably just going to redo the native code for the
// entire synth engine in plain C because it's not worth maintaining
// two very similar implementations side by side. Lots of plain C
// code can just be deduplicated and #include'd in different places:
// compute kernel sources and e.g. noteout() definitions.
// that's the theory.
//
// OK I admit the interface between C++ memory users and C memory users
// and OpenCL memory users (transfers) is the same thing no matter what.
// If I'm being honest I have to point out that precisely how I
// do whatever I do here is almost inconsequential. I'm simply
// going to put a memcpy-ish thing on the CL command queue and it
// won't matter how I gathered up what the void-pointer points at
// but...
// I still want to (C&CL)ificate every thing (except gui libraries).
// because.
// all the other dependencies are already plain C.
// so this mess with actual classes instead of structs? yeah it ain't
// gonna last anyway and I shoulda started off with more gusto.
//
// [1] https://www.youtube.com/watch?v=ez18sOaR748
//
// THAT WAS the part where I convince myself again (in public now)
// that what I already decided to do because of "reasons" is the
// good decision because of "reasons". In the future I'll be all
// over everyone else's case for doing exactly that (because it's
// actually pretty dangerous and usually just wrong) but in this
// very specific case, I'll just have to be honest about whether it
// turned out to be a good strategy, AFTER I poured in the time that
// is never coming back
//
// THIS IS the part where I visibly pretend to make up my mind about
// whether I'm actually writing C++ or C in the short term. enjoy

union CommandBlock;
class GuiThreadMsg;
class ADnote;
class ADnoteParameters;
class AnalogFilter;
class Controller;
class Effect;
class EffectMgr;
class Envelope;
class EnvelopeParams;
class EventQueue;
class Filter;
class FilterParams;
class FormantFilter;
class GuiThreadMsg;
class LFO;
class LFOParams;
class OscilGen;
class PADnote;
class PADnoteParameters;
class Part;
class Resonance;
class SUBnote;
class SUBnoteParameters;
class SUBnoteharmonic;
class SVFilter;

// tb = tankbit, smallest unit, the thing that we would always
// "self.otherthing = new Thing(thing_pars)", or a reusable array of float

typedef enum
{
    TB_NONE,
    TB_MONO_BUF,
    TB_STEREO_BUF,
    TB_CMD_BLK,
    TB_GUI_MSG,
    TB_ADN_OBJ,
    TB_ADN_PAR,
    TB_ANA_FLT,
    TB_CTRL_PAR,
    TB_ENV_OBJ,
    TB_ENV_PAR,
    TB_EVT_QUE,
    TB_FILT_OBJ,
    TB_FILT_PAR,
    TB_FORM_FLT,
    TB_FX_MGR,
    TB_FX_UNIT,
    TB_LFO_OBJ,
    TB_LFO_PAR,
    TB_OSC_GEN,
    TB_PADN_OBJ,
    TB_PADN_PAR,
    TB_PART_OBJ,
    TB_RESO_OBJ,
    TB_SUBN_HRM,
    TB_SUBN_OBJ,
    TB_SUBN_PAR,
    TB_SV_FLT,
} tb_type;

// a pointer to an array of any object is always the size of a pointer, right?
// yes of course. sweet

typedef union
{
    float               *f32a;
    CommandBlock        *cmdb;
    GuiThreadMsg        *gmsg;
    ADnote              *adn;
    ADnoteParameters    *adnp;
    AnalogFilter        *aflt;
    Controller          *ctrl;
    Effect              *eff;
    EffectMgr           *efmg;
    Envelope            *env;
    EnvelopeParams      *envp;
    EventQueue          *evtq;
    Filter              *filt;
    FilterParams        *fltp;
    FormantFilter       *frmf;
    LFO                 *lfo;
    LFOParams           *lfop;
    OscilGen            *oscg;
    PADnote             *padn;
    PADnoteParameters   *padp;
    Part                *part;
    Resonance           *reso;
    SUBnote             *subn;
    SUBnoteParameters   *subp;
    SUBnoteharmonic     *subh;
    SVFilter            *stvf;

} tb_data;

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
    void gc();
private:
    SynthEngine *synth;
    tb_type purpose;
    void add_tankblock(uint32_t sz);

    // the root of this monotypical object tree
    std::list<tankblock *> tb_group;

    // how big we want the 1st, 2nd, etc
    // based on how many we can ever need
    std::vector<uint32_t> block_sz_schedule;

};

#endif // TANK_H
