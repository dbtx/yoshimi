#include "Misc/SynthEngine.h"
#include "DSP/AnalogFilter.h"
#include "DSP/Filter.h"
#include "DSP/FormantFilter.h"
#include "DSP/SVFilter.h"
#include "Effects/EffectMgr.h"
#include "Effects/Effect.h"
#include "Misc/Part.h"
#include "Misc/EventQueue.h"
#include "Params/ADnoteParameters.h"
#include "Params/Controller.h"
#include "Params/EnvelopeParams.h"
#include "Params/FilterParams.h"
#include "Params/LFOParams.h"
#include "Params/PADnoteParameters.h"
#include "Params/SUBnoteParameters.h"
#include "Synth/ADnote.h"
#include "Synth/Envelope.h"
#include "Synth/LFO.h"
#include "Synth/OscilGen.h"
#include "Synth/PADnote.h"
#include "Synth/Resonance.h"
#include "Synth/SUBnote.h"
#include "globals.h"
#include "Tank.h"

Tank::Tank(tb_type tt, uint32_t sz, SynthEngine *_synth)
{
    synth = _synth;
    purpose = tt;
    add_tankblock(sz);
}

void Tank::add_tankblock(uint32_t sz)
{
    tankblock *tblk = new tankblock;
    tb_group.push_front(tblk);
    tblk->sz = sz;
    tblk->nodeID = new uint32_t[sz];
    switch (purpose)
    {
        case TB_NONE: // i don't know either
            break;
        case TB_MONO_BUF:
            tblk->payload->f32a = (float *) fftw_malloc(synth->buffersize * sz);
            break;
        case TB_STEREO_BUF:
            tblk->payload->f32a = (float *) fftw_malloc(2 * synth->buffersize * sz);
            break;
        case TB_CMD_BLK:
            tblk->payload->cmdb = (CommandBlock *) malloc(sizeof(CommandBlock) * sz);
            break;
        case TB_GUI_MSG:
            tblk->payload->gmsg = (GuiThreadMsg *) malloc(sizeof(GuiThreadMsg) * sz);
            break;
        case TB_ADN_OBJ:
            tblk->payload->adn = (ADnote *) malloc(sizeof(ADnote) * sz);
            break;
        case TB_ADN_PAR:
            tblk->payload->adnp = (ADnoteParameters *) malloc(sizeof(ADnoteParameters) * sz);
            break;
        case TB_ANA_FLT:
            tblk->payload->aflt = (AnalogFilter *) malloc(sizeof(AnalogFilter) * sz);
            break;
        case TB_CTRL_PAR:
            tblk->payload->ctrl = (Controller *) malloc(sizeof(Controller) * sz);
            break;
        case TB_ENV_OBJ:
            tblk->payload->env = (Envelope *) malloc(sizeof(Envelope) * sz);
            break;
        case TB_ENV_PAR:
            tblk->payload->envp = (EnvelopeParams *) malloc(sizeof(EnvelopeParams) * sz);
            break;
        case TB_EVT_QUE:
            tblk->payload->evtq = (EventQueue *) malloc(sizeof(EventQueue) * sz);
            break;
        case TB_FILT_OBJ:
            tblk->payload->filt = (Filter *) malloc(sizeof(Filter) * sz);
            break;
        case TB_FILT_PAR:
            tblk->payload->fltp = (FilterParams *) malloc(sizeof(FilterParams) * sz);
            break;
        case TB_FORM_FLT:
            tblk->payload->frmf = (FormantFilter *) malloc(sizeof(FormantFilter) * sz);
            break;
        case TB_FX_MGR:
            tblk->payload->efmg = (EffectMgr *) malloc(sizeof(EffectMgr) * sz);
            break;
        case TB_FX_UNIT:
            tblk->payload->eff = (Effect *) malloc(sizeof (Effect) * sz);
            break;
        case TB_LFO_OBJ:
            tblk->payload->lfo = (LFO *) malloc(sizeof(LFO) * sz);
            break;
        case TB_LFO_PAR:
            tblk->payload->lfop = (LFOParams *) malloc(sizeof(LFOParams) * sz);
            break;
        case TB_OSC_GEN:
            tblk->payload->oscg = (OscilGen *) malloc(sizeof(OscilGen) * sz);
            break;
        case TB_PADN_OBJ:
            tblk->payload->padn = (PADnote *) malloc(sizeof(PADnote) * sz);
            break;
        case TB_PADN_PAR:
            tblk->payload->padp = (PADnoteParameters *) malloc(sizeof(PADnoteParameters) * sz);
            break;
        case TB_PART_OBJ:
            tblk->payload->part = (Part *) malloc(sizeof(Part) * sz);
            break;
        case TB_RESO_OBJ:
            tblk->payload->reso = (Resonance *) malloc(sizeof(Resonance) * sz);
            break;
        case TB_SUBN_OBJ:
            tblk->payload->subn = (SUBnote *) malloc(sizeof(SUBnote) * sz);
            break;
        case TB_SUBN_PAR:
            tblk->payload->subp = (SUBnoteParameters *) malloc(sizeof(SUBnoteParameters) * sz);
            break;
        case TB_SV_FLT:
            tblk->payload->stvf = (SVFilter *) malloc(sizeof(SVFilter) * sz);
    }
}

void Tank::free_tankblock(tankblock *tblk)
{
    // this is going to be messy.
    //
    // we convince ourselves that nothing is in the tblk before hand, i.e.
    // this one just had its few bits shuffled out during a gc() run
    // so for any stale bits here, we need to have already freed any resources in
    // their own "destructor"s especially visiting the float buffers they used,
    // always writing 0 to the previously held tb's nodeID (signifying free).
    // BUT LFO for example isn't going to be a friend class. if the tb's nodeID
    // *isn't* inside the tb then we can't just write 0 to it w/o the tank's
    // involvement... sigh

    // the tank's involvement will be Tank::ungrab(nodeID) but then we have to keep
    // track of the block+bit<->nodeID mapping all the time esp. during gc() and
    // that seems unpleasant. we could just return a pointer on grab() and later
    // update the pointer's spot because we know the nodeID we can still find it
    // the old fashioned way by walking the tree just like incoming events. ah, but
    // "hey update this pointer" is not a commandblock thing and never will be

    // eh, maybe grab() is rly
    // LFOtank::grab(uint32_t nodeID, **LFO);
    // so each consumer grabs sending its nodeID and getting its SomeLfo var both
    // set and copied for resetting after a gc... seems to be summat nasty coupling
    // well, maybe

    // okay then. we have everybody checking params being updated in case smth
    // special needs to happen. and the params objects do their own reset after
    // the update. (thoughtfully named postrender by myself)
    // so we can roughly as easily have everybody check their Tank
    // helpers that gave them old pointer and ask for new pointer, IF gc() ran.
    // we just sort of add it to the cost of running a gc

    // and tank still needs to keep track of who had what before gc. that's no
    // biggie, we have array of nodeID. and tank still needs to be involved in
    // ungrab, and at the abs. worst, we iterate over the same array and zero the
    // one that matches.
    // maybe use a search tree? binary tree? i'm out of my league unfortunately
    // but i got to learn those things anyway. the good news is nodeID are as
    // good as UUID inside this program. just remember to pass the nodeID of the
    // thing being grab'ed and not the nodeID of the thing calling grab, right?

    // gc can be shallow memcpy from array to array. other tankbits are in other
    // tanks and their addresses stay correct


    // then AFTER visiting all the former type's members' also-grabbed members
    // and ungrabbing them as the need arises, we can just...
    free(tblk->payload->f32a);
    return;
    //our caller is going to knock tblk out from the list it was in
}
