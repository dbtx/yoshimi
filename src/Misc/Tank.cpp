#include "Misc/SynthEngine.h"
#include "DSP/Filter.h"
#include "Params/FilterParams.h"
#include "Effects/EffectMgr.h"
#include "Effects/Effect.h"
#include "Synth/LFO.h"
#include "Params/LFOParams.h"
#include "Synth/Envelope.h"
#include "Params/EnvelopeParams.h"
#include "Synth/ADnote.h"
#include "Params/ADnoteParameters.h"
#include "Synth/SUBnote.h"
#include "Params/SUBnoteParameters.h"
#include "Synth/PADnote.h"
#include "Params/PADnoteParameters.h"
#include "Tank.h"

Tank::Tank(tb_type tt, uint32_t sz, SynthEngine *_synth)
{
    synth = _synth;
    purpose = tt;
    add_tankblock(sz);
}

void Tank::add_tankblock(uint32_t sz)
{
    tankblock *tb = new tankblock;
    tb_group.push_front(tb);
    tb->sz = sz;
    tb->nodeID = new uint32_t[sz];
    switch (purpose)
    {
        case TB_NONE:
            break;
        case TB_MONO_BUF:
            tb->payload->f32a = (float *) malloc(synth->buffersize * sz);
            break;
        case TB_STEREO_BUF:
            tb->payload->f32a = (float *) malloc(2 * synth->buffersize * sz);
            break;
        case TB_LFO_OBJ:
            tb->payload->lfo = (LFO *)malloc(sizeof(LFO) * sz);
            break;
        case TB_ENV_OBJ:
            tb->payload->env = (Envelope *) malloc(sizeof(Envelope) * sz);
            break;
        case TB_FILT_OBJ:
            tb->payload->filt = (Filter *) malloc(sizeof(Filter) * sz);
            break;
        case TB_FX_UNIT:
            break;
        case TB_FX_MGR:
            break;
        case TB_ADN_OBJ:
            break;
        case TB_ADN_PAR:
            break;
        case TB_SUBN_OBJ:
            break;
        case TB_SUBN_PAR:
            break;
        case TB_PADN_OBJ:
            break;
        case TB_PADN_PAR:
            break;
        case TB_CMD_BLK:
            break;
//        default:
//            break;

    }
}
