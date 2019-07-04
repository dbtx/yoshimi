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
        case TB_CTRL_OBJ:
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

    // the map will have nodeID for key and block/bit for value.
    // so grab will return pointer to array position in struct at list item
    // (block).payload[bit]

    // gc can be shallow memcpy from array to array. other tankbits are in other
    // tanks and their addresses stay correct

    // take 17
    // in each parent object that calls grab for a tankbit, we are later going to
    // be the primary one who knows we're done and "calls ungrab". so.
    //
    // we stop using a separate array in tankblock to store nodeIDs
    // and instead we put the nodeID as a member in AH CRAP it doesn't work on floats
    // so we make floats a struct after all.   ^^in the new tankbit, called parent
    // or smth. that ID is what Tank looks at when finding an unused tb. it knows
    // the type already, it can access all of them, if a parent is 0 then it is
    // none. so when parent is ready to ungrab LFO at the pointer self->AmplitudeLFO,
    // it just sets self->AmplitudeLFO = self->AmplitudeLFO->parent = 0; thus
    // disowning it, and the memory that gets written to is memory that exists
    // inside the Tank. parent's nodeID is one of
    // numberoftankbits(FOOnote) * Polyphony, or
    // numberofParts * numberofkititems * numberoftankbits(FOOnoteParameters)
    //
    // now, gc considerations-- we copy the tankbit wholesale as usual and its
    // nonzero nodeID for parent comes along for the ride. we check for a gc run
    // and re-cache
    // the pointer we get but how do we update pointer to us? we didn't keep a pointer
    // to the parent. if we did then we'd be able to parent->smthLFO = here_instead.
    // so maybe call LFO->grab(this) and the Tank automagically takes this.nodeID
    // whenever? and saves (this) so it can change this.SomeLFO member to new (LFO *)
    // location later on? no. (not stored in the tank)
    // the grab'bed one will be written to when returning the
    // pointer to use, to the parent. grabbed tankbit holds pointer to parent, thus
    // parent.nodeID, so parent's type is deducible from nodeID's bitmask,
    // so during a gc we can change movedtankbit.parent->SomeLFO to our new
    // tankblock[idx]. or tankblock + idx. or whatever.

    // what if parent gets moved during same gc? parent pointer becomes wrong.
    // what do we do for grabbed children? we need to update their parents' pointers
    // though nodeID won't change of course address will

    // hm. we can still visit those through the pointers that quite naturally still
    // belong to this tankbit... but we got to make certain that things get
    // updated in the right sequence. i mean that tanks get updated.

    // OH RIGHT i was not planning on doing gc on every tank at once anyway.
    // because gc takes time and we are going to have time in between periods
    // so we use an atomic or smth to unlatch gc caller last, before returning from
    // process(). IIRC there are no LFOs that belong to LFOs so there's no
    // race condition tied up in gc'ing the only LFOtank. but even in some kind of
    // sequence, i'd not do them all in a pass-- i'd stagger them at least, one Tank
    // per gc run.

    // tankbits parents are always in other tanks!

    // and everything is broken anyway. by design. heh

    // I would like to sort of preserve locality by causing e.g. LFOParameters
    // in *that* tank to stay near the LFOparameters of other LFOs that'll be
    // temporally local in the same *notes* but that's going to be tricky.
    // if we don't always grab the same notes'worth of LFOs (and we won't) then
    // ungrabbing will leave wrong-size holes, for Tank to give out to someone later
    // but they'll not need the results near the same time.

    // it'll be like I did a different flavor of memory fragmentation.

    // so maybe i can defragment only the results buffers? oh but if the LFOparams
    // tankbits are not fragmented, BUT the LFO tankbits are, then the problem will
    // probably still appear. likewise during mixing w.r.t. result buffers. I wish
    // sometimes that memory was just one big giant shift register, indeed it
    // could be but not so very big in the end, or very expensive.

    // anyway if we do all these bitty updates immediately after copying
    // the tankbit to its new slot, then AT LEAST there'll be no broken pointers
    // in the midst of updating members, because all the data is still in old place.
    // and it won't get overwritten with new data until some following noteOn()
    // because we aren't yet shuffling whole sets of live tankbits, we're just
    // trying to get them out of that tank which is closest to empty.
    // for now.
    // in the future try to really defrag each note and not just the free space
    // and that hopefully without tons of memcpy. tall order and way too far ahead

    // anyway the good-ish news there is that i'm not looping over parts and kititems
    // anymore, i'm looping over the notes however they sit in any order at all.
    // AFTER i loop over the LFOs however they sit. I guess I could--

    // whoa. I could look at the scattered FOOnote params and *decide* how the
    // result buffers will be laid out each period so they always get used in the
    // same order that those other tankbits will be marched through. and locality++

    // do that, very much that. don't write then defrag, just WRITE IT IN ORDER
    // each time. estimate, count nodeIDs, sort ascending, reserve float bufs,
    // march through and scattered-write them whenever their LFOparams gets fired.

    // mooltipass

    // then AFTER visiting all the type's members' also-grabbed members
    // and ungrabbing them as the need arises, we can just...
    free(tblk->payload->f32a);
    return;
    // our caller (probably gc()) is going to knock any now-empty tblk out from
    // the Tank.tb_group list it was in, it's not our problem
}
