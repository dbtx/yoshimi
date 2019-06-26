/*
 * Data2Text.cpp
 */

#include "Interface/Data2Text.h"
#include "Misc/SynthEngine.h"
#include "Misc/MiscFuncs.h"

std::string DataText::resolveAll(SynthEngine *_synth, CommandBlock *getData, bool addValue)
{
    SynthEngine *synth = _synth;
    float value = getData->data.value.F;
    unsigned char type = getData->data.type;
    unsigned char control = getData->data.control;
    unsigned char npart = getData->data.part;
    unsigned char kititem = getData->data.kit;
    unsigned char engine = getData->data.engine;
    unsigned char insert = getData->data.insert;
    unsigned char insertParam = getData->data.parameter;

    if (control == TOPLEVEL::control::textMessage && insertParam != TOPLEVEL::insert::resonanceGraphInsert) // special case for simple messages
    {
        synth->getRuntime().Log(miscMsgPop(lrint(value)));
        synth->getRuntime().finishedCLI = true;
        return "";
    }

    showValue = true;
    std::string commandName;

    Part *part;
    part = synth->part[npart];

    // this is unique and placed here to avoid Xruns
    if (npart == TOPLEVEL::section::scales && (control <= SCALES::control::tuning || control >= SCALES::control::retune))
        synth->setAllPartMaps();

    if (npart == TOPLEVEL::section::vector)
        commandName = resolveVector(getData);
    else if (npart == TOPLEVEL::section::scales)
        commandName = resolveMicrotonal(getData);
    else if (npart == TOPLEVEL::section::config)
        commandName = resolveConfig(getData);
    else if (npart == TOPLEVEL::section::bank)
        commandName = resolveBank(getData);
    else if (npart == TOPLEVEL::section::midiIn || npart == TOPLEVEL::section::main)
        commandName = resolveMain(getData);

    else if (npart == TOPLEVEL::section::systemEffects || npart == TOPLEVEL::section::insertEffects)
        commandName = resolveEffects(getData);

    else if ((kititem >= EFFECT::type::none && kititem <= EFFECT::type::dynFilter) || (control >= PART::control::effectNumber && control <= PART::control::effectBypass && kititem == UNUSED))
        commandName = resolveEffects(getData);

    else if (npart >= NUM_MIDI_PARTS)
    {
        showValue = false;
        commandName = "Invalid part " + std::to_string(int(npart) + 1);
    }

    else if (kititem >= NUM_KIT_ITEMS && kititem < UNUSED)
    {
        showValue = false;
        commandName = "Invalid kit " + std::to_string(int(kititem) + 1);
    }

    else if (kititem != 0 && engine != UNUSED && control != PART::control::enable && part->kit[kititem].Penabled == false)
        commandName = "Part " + std::to_string(int(npart) + 1) + " Kit item " + std::to_string(int(kititem) + 1) + " not enabled";

    else if (kititem == UNUSED || insert == TOPLEVEL::insert::kitGroup)
    {
        if (control != PART::control::kitMode && kititem != UNUSED && part->Pkitmode == 0)
        {
            showValue = false;
            commandName = "Part " + std::to_string(int(npart) + 1) + " Kitmode not enabled";
        }
        else
            commandName = resolvePart(getData);
    }
    else if (kititem > 0 && part->Pkitmode == 0)
    {
        showValue = false;
        commandName = "Part " + std::to_string(int(npart) + 1) + " Kitmode not enabled";
    }

    else if (engine == PART::engine::padSynth)
    {
        switch(insert)
        {
            case UNUSED:
                commandName = resolvePad(getData);
                break;
            case TOPLEVEL::insert::LFOgroup:
                commandName = resolveLFO(getData);
                break;
            case TOPLEVEL::insert::filterGroup:
                commandName = resolveFilter(getData);
                break;
            case TOPLEVEL::insert::envelopeGroup:
                commandName = resolveEnvelope(getData);
                break;
            case TOPLEVEL::insert::envelopePoints:
                commandName = resolveEnvelope(getData);
                break;
            case TOPLEVEL::insert::envelopePointChange:
                commandName = resolveEnvelope(getData);
                break;
            case TOPLEVEL::insert::oscillatorGroup:
                commandName = resolveOscillator(getData);
                break;
            case TOPLEVEL::insert::harmonicAmplitude:
                commandName = resolveOscillator(getData);
                break;
            case TOPLEVEL::insert::harmonicPhaseBandwidth:
                commandName = resolveOscillator(getData);
                break;
            case TOPLEVEL::insert::resonanceGroup:
                commandName = resolveResonance(getData);
                break;
            case TOPLEVEL::insert::resonanceGraphInsert:
                commandName = resolveResonance(getData);
                break;
        }
    }

    else if (engine == PART::engine::subSynth)
    {
        switch (insert)
        {
            case UNUSED:
                commandName = resolveSub(getData);
                break;
            case TOPLEVEL::insert::harmonicAmplitude:
                commandName = resolveSub(getData);
                break;
            case TOPLEVEL::insert::harmonicPhaseBandwidth:
                commandName = resolveSub(getData);
                break;
            case TOPLEVEL::insert::filterGroup:
                commandName = resolveFilter(getData);
                break;
            case TOPLEVEL::insert::envelopeGroup:
                commandName = resolveEnvelope(getData);
                break;
            case TOPLEVEL::insert::envelopePoints:
                commandName = resolveEnvelope(getData);
                break;
            case TOPLEVEL::insert::envelopePointChange:
                commandName = resolveEnvelope(getData);
                break;
        }
    }

    else if (engine >= PART::engine::addVoice1)
    {
        switch (insert)
        {
            case UNUSED:
                commandName = resolveAddVoice(getData);
                break;
            case TOPLEVEL::insert::LFOgroup:
                commandName = resolveLFO(getData);
                break;
            case TOPLEVEL::insert::filterGroup:
                commandName = resolveFilter(getData);
                break;
            case TOPLEVEL::insert::envelopeGroup:
                commandName = resolveEnvelope(getData);
                break;
            case TOPLEVEL::insert::envelopePoints:
                commandName = resolveEnvelope(getData);
                break;
            case TOPLEVEL::insert::envelopePointChange:
                commandName = resolveEnvelope(getData);
                break;
            case TOPLEVEL::insert::oscillatorGroup:
                commandName = resolveOscillator(getData);
                break;
            case TOPLEVEL::insert::harmonicAmplitude:
                commandName = resolveOscillator(getData);
                break;
            case TOPLEVEL::insert::harmonicPhaseBandwidth:
                commandName = resolveOscillator(getData);
                break;
        }
    }

    else if (engine == PART::engine::addSynth)
    {
        switch (insert)
        {
            case UNUSED:
                commandName = resolveAdd(getData);
                break;
            case TOPLEVEL::insert::LFOgroup:
                commandName = resolveLFO(getData);
                break;
            case TOPLEVEL::insert::filterGroup:
                commandName = resolveFilter(getData);
                break;
            case TOPLEVEL::insert::envelopeGroup:
                commandName = resolveEnvelope(getData);
                break;
            case TOPLEVEL::insert::envelopePoints:
                commandName = resolveEnvelope(getData);
                break;
            case TOPLEVEL::insert::envelopePointChange:
                commandName = resolveEnvelope(getData);
                break;
            case TOPLEVEL::insert::resonanceGroup:
                commandName = resolveResonance(getData);
                break;
            case TOPLEVEL::insert::resonanceGraphInsert:
                commandName = resolveResonance(getData);
                break;
        }
    }

    std::string actual = "";
    if (showValue)
    {
        actual = " Value ";
        if (type & TOPLEVEL::type::Integer)
            actual += std::to_string(lrint(value));
        else
            actual += std::to_string(value);
    }
    if (addValue)
        commandName += actual;
    return commandName;
}


std::string DataText::resolveVector(CommandBlock *getData)
{
    int value_int = lrint(getData->data.value.F);
    unsigned char control = getData->data.control;
    unsigned int chan = getData->data.insert;

    std::string contstr = "";
    switch (control)
    {
        //case 0:
            //contstr = "Base Channel"; // local to source
            //break;
        //case 1:
            //contstr = "Options";
            //break;
        case VECTOR::control::name:
            showValue = false;
            contstr = "Name " + miscMsgPop(value_int);
            break;

        case VECTOR::control::Xcontroller:
            contstr = "Controller";
            break;
        case VECTOR::control::XleftInstrument:
            contstr = "Left Instrument";
            break;
        case VECTOR::control::XrightInstrument:
            contstr = "Right Instrument";
            break;
        case VECTOR::control::Xfeature0:
        case VECTOR::control::Yfeature0:
            contstr = "Feature 0";
            break;
        case VECTOR::control::Xfeature1:
        case VECTOR::control::Yfeature1:
            contstr = "Feature 1";
            break;
        case VECTOR::control::Xfeature2:
        case VECTOR::control::Yfeature2:
            contstr = "Feature 2 ";
            break;
        case VECTOR::control::Xfeature3:
        case VECTOR::control::Yfeature3:
            contstr = "Feature 3";
            break;

        case VECTOR::control::Ycontroller:
            contstr = "Controller";
            break;
        case VECTOR::control::YupInstrument:
            contstr = "Up Instrument";
            break;
        case VECTOR::control::YdownInstrument:
            contstr = "Down Instrument";
            break;

        case VECTOR::control::erase:
            showValue = false;
            if (chan > NUM_MIDI_CHANNELS)
                contstr = "all channels";
            else
                contstr = "chan " + std::to_string(chan + 1);
            return("Vector cleared on " + contstr);
            break;

        case 127:
            break;

        default:
            showValue = false;
            contstr = "Unrecognised";
    }

    if (control == VECTOR::control::undefined)
    {
        showValue = false;
        return("Vector " + contstr + " set to " + std::to_string(chan + 1));
    }
    std::string name = "Vector Chan " + std::to_string(chan + 1) + " ";
    if (control == 127)
        name += " all ";
    else if (control >= VECTOR::control::Ycontroller)
        name += "Y ";
    else if(control >= VECTOR::control::Xcontroller)
        name += "X ";

    return (name + contstr);
}


std::string DataText::resolveMicrotonal(CommandBlock *getData)
{
    int value = getData->data.value.F;
    unsigned char control = getData->data.control;

    std::string contstr = "";
    switch (control)
    {
        case SCALES::control::Afrequency:
            contstr = "'A' Frequency";
            break;
        case SCALES::control::Anote:
            contstr = "'A' Note";
            break;
        case SCALES::control::invertScale:
            contstr = "Invert Keys";
            break;
        case SCALES::control::invertedScaleCenter:
            contstr = "Key Center";
            break;
        case SCALES::control::scaleShift:
            contstr = "Scale Shift";
            break;
        case SCALES::control::enableMicrotonal:
            contstr = "Enable Microtonal";
            break;

        case SCALES::control::enableKeyboardMap:
            contstr = "Enable Keyboard Mapping";
            break;
        case SCALES::control::lowKey:
            contstr = "Keyboard First Note";
            break;
        case SCALES::control::middleKey:
            contstr = "Keyboard Middle Note";
            break;
        case SCALES::control::highKey:
            contstr = "Keyboard Last Note";
            break;

        case SCALES::control::tuning:
            contstr = "Tuning ";
            showValue = false;
            break;
        case SCALES::control::keyboardMap:
            contstr = "Keymap ";
            showValue = false;
            break;
        case SCALES::control::importScl:
            contstr = "Tuning Import ";
            showValue = false;
            break;
        case SCALES::control::importKbm:
            contstr = "Keymap Import ";
            showValue = false;
            break;

        case SCALES::control::name:
            contstr = "Name: " + std::string(synth->microtonal.Pname);
            showValue = false;
            break;
        case SCALES::control::comment:
            contstr = "Description: " + std::string(synth->microtonal.Pcomment);
            showValue = false;
            break;
        case SCALES::control::retune:
            contstr = "Retune";
            showValue = false;
            break;

        case SCALES::control::clearAll:
            contstr = "Clear all settings";
            showValue = false;
            break;

        default:
            showValue = false;
            contstr = "Unrecognised";

    }

    if (value < 1 && control >= SCALES::control::tuning && control <= SCALES::control::importKbm)
    { // errors :@(
        switch (value)
        {
            case 0:
                contstr += "Empty entry";
                break;
            case -1:
                contstr += "Value too small";
                break;
            case -2:
                contstr += "Invalid entry";
                break;
            case -3:
                contstr += "File not found";
                break;
            case -4:
                contstr += "Empty file";
                break;
            case -5:
                contstr += "Short or corrupted file";
                break;
            case -6:
                if (control == SCALES::control::tuning || control == SCALES::control::importScl)
                    contstr += "Invalid octave size";
                else
                    contstr += "Invalid keymap size";
                break;
            case -7:
                contstr += "Invalid note number";
                break;
            case -8:
                contstr += "Out of range";
                break;
        }
    }
    return ("Scales " + contstr);
}

std::string DataText::resolveConfig(CommandBlock *getData)
{
    float value = getData->data.value.F;
    unsigned char control = getData->data.control;
    unsigned char kititem = getData->data.kit;
    bool write = getData->data.type & TOPLEVEL::type::Write;
    int value_int = lrint(value);
    bool value_bool = YOSH::F2B(value);
    bool yesno = false;
    std::string contstr = "";
    switch (control)
    {
        case CONFIG::control::oscillatorSize:
            contstr = "AddSynth oscillator size";
            break;
        case CONFIG::control::bufferSize:
            contstr = "Internal buffer size";
            break;
        case CONFIG::control::padSynthInterpolation:
            contstr = "PadSynth interpolation ";
            if (value_bool)
                contstr += "cubic";
            else
                contstr += "linear";
            showValue = false;
            break;
        case CONFIG::control::virtualKeyboardLayout:
            contstr = "Virtual keyboard ";
            switch (value_int)
            {
                case 0:
                    contstr += "QWERTY";
                    break;
                case 1:
                    contstr += "Dvorak";
                    break;
                case 2:
                    contstr += "QWERTZ";
                    break;
                case 3:
                    contstr += "AZERTY";
                    break;
            }
            showValue = false;
            break;
        case CONFIG::control::XMLcompressionLevel:
            contstr = "XML compression";
            break;
        case CONFIG::control::reportsDestination:
            contstr = "Reports to ";
            if (value_bool)
                contstr += "console window";
            else
                contstr += "stdout";
            showValue = false;
            break;
        case CONFIG::control::savedInstrumentFormat:
            contstr = "Saved Instrument Format ";
            switch (value_int)
            {
                case 1:
                    contstr += "Legacy (.xiz)";
                    break;
                case 2:
                    contstr += "Yoshimi (.xiy)";
                    break;
                case 3:
                    contstr += "Both";
                    break;
            }
            showValue = false;
            break;
        case CONFIG::control::defaultStateStart:
            contstr += "Autoload default state";
            yesno = true;
            break;
        case CONFIG::control::enableSinglePath:
            contstr += "Single master instance";
            yesno = true;
            break;
        case CONFIG::control::hideNonFatalErrors:
            contstr += "Hide non-fatal errors";
            yesno = true;
            break;
        case CONFIG::control::showSplash:
            contstr += "Show splash screen";
            yesno = true;
            break;
        case CONFIG::control::logInstrumentLoadTimes:
            contstr += "Log instrument load times";
            yesno = true;
            break;
        case CONFIG::control::logXMLheaders:
            contstr += "Log XML headers";
            yesno = true;
            break;
        case CONFIG::control::saveAllXMLdata:
            contstr += "Save ALL XML data";
            yesno = true;
            break;
        case CONFIG::control::enableGUI:
            contstr += "Enable GUI";
            yesno = true;
            break;
        case CONFIG::control::enableCLI:
            contstr += "Enable CLI";
            yesno = true;
            break;
        case CONFIG::control::enableAutoInstance:
            contstr += "Enable Auto Instance";
            yesno = true;
            break;
        case CONFIG::control::historyLock:
        {
            std::string group[] = {"Instrument", "Patchset", "Scale", "State", "Vector", "Mlearn"};
            showValue = false;
            contstr = "History lock " + group[kititem];
            if (value_int)
                contstr += " On";
            else
                contstr += " Off";
            break;
        }
        case CONFIG::control::exposeStatus:
            showValue = false;
            contstr += "Show CLI context ";
            switch (value_int)
            {
                case 0:
                    contstr += "off";
                    break;
                case 1:
                    contstr += "on";
                    break;
                case 2:
                    contstr += "prompt";
                    break;
                default:
                    contstr += "unrecognised";
                    break;
            }
            break;

        case CONFIG::control::jackMidiSource:
            contstr += "JACK MIDI source: ";
            contstr += miscMsgPop(value_int);
            showValue = false;
            break;
        case CONFIG::control::jackPreferredMidi:
            contstr += "Start with JACK MIDI";
            yesno = true;
            break;
        case CONFIG::control::jackServer:
            contstr += "JACK server: ";
            contstr += miscMsgPop(value_int);
            showValue = false;
            break;
        case CONFIG::control::jackPreferredAudio:
            contstr += "Start with JACK audio";
            yesno = true;
            break;
        case CONFIG::control::jackAutoConnectAudio:
            contstr += "Auto-connect to JACK server";
            yesno = true;
            break;

        case CONFIG::control::alsaMidiSource:
            contstr += "ALSA MIDI source: ";
            contstr += miscMsgPop(value_int);
            showValue = false;
            break;
        case CONFIG::control::alsaPreferredMidi:
            contstr += "Start with ALSA MIDI";
            yesno = true;
            break;
        case CONFIG::control::alsaAudioDevice:
            contstr += "ALSA audio device: ";
            contstr += miscMsgPop(value_int);
            showValue = false;
            break;
        case CONFIG::control::alsaPreferredAudio:
            contstr += "Start with ALSA audio";
            yesno = true;
            break;
        case CONFIG::control::alsaSampleRate:
            contstr += "ALSA sample rate: ";
            switch (value_int)
            { // this is a hack :(
                case 0:
                case 192000:
                    contstr += "0 (192000)";
                    break;
                case 1:
                case 96000:
                    contstr += "1 (96000)";
                    break;
                case 2:
                case 48000:
                    contstr += "2 (48000)";
                    break;
                case 3:
                case 44100:
                    contstr += "3 (44100)";
                    break;
            }
            showValue = false;
            break;

        case CONFIG::control::bankRootCC:
            contstr += "Bank root CC ";
            switch (value_int)
            {
                case 0:
                    contstr += "MSB";
                    break;
                case 32:
                    contstr += "LSB";
                    break;
                default:
                    contstr += "OFF";
            }
            showValue = false;
            break;

        case CONFIG::control::bankCC:
            contstr += "Bank CC ";
            switch (value_int)
            {
                case 0:
                    contstr += "MSB";
                    break;
                case 32:
                    contstr += "LSB";
                    break;
                default:
                    contstr += "OFF";
            }
            showValue = false;
            break;
        case CONFIG::control::enableProgramChange:
            contstr += "Enable program change";
            yesno = true;
            break;
        case CONFIG::control::instChangeEnablesPart:
            contstr += "Program change enables part";
            yesno = true;
            break;
        case CONFIG::control::extendedProgramChangeCC:
            contstr += "CC for extended program change";
            break;
        case CONFIG::control::ignoreResetAllCCs:
            contstr += "Ignore 'reset all CCs'";
            yesno = true;
            break;
        case CONFIG::control::logIncomingCCs:
            contstr += "Log incoming CCs";
            yesno = true;
            break;
        case CONFIG::control::showLearnEditor:
            contstr += "Auto-open GUI MIDI-learn editor";
            yesno = true;
            break;

        case CONFIG::control::enableNRPNs:
            contstr += "Enable NRPN";
            yesno = true;
            break;

        case CONFIG::control::saveCurrentConfig:
        {
            std::string name = miscMsgPop(value_int);
            if (write)
                contstr += ("save" + name);
            else
            {
                contstr += "Condition - ";
                 if(synth->getRuntime().configChanged)
                     contstr += "DIRTY";
                 else
                     contstr += "CLEAN";
            }
            showValue = false;
            break;
        }
        default:
            contstr = "Unrecognised";
            break;
    }

    if (yesno)
    {
        if (value_bool)
            contstr += " - yes";
        else
            contstr += " - no";
        showValue = false;
    }
    return ("Config " + contstr);
}


std::string DataText::resolveBank(CommandBlock *getData)
{
    int value_int = lrint(getData->data.value.F);
    int control = getData->data.control;
    int kititem = getData->data.kit;
    int engine = getData->data.engine;
    int insert = getData->data.insert;
    std::string name = miscMsgPop(value_int);
    std::string contstr = "";
    showValue = false;
    switch(control)
    {
        case BANK::control::deleteInstrument:
            contstr = "Instrument delete" + name;
            break;
        case BANK::control::selectFirstInstrumentToSwap:
            contstr = "Set Instrument ID " + std::to_string(insert + 1) + "  Bank ID " + std::to_string(kititem) + "  Root ID " + std::to_string(engine) + " for swap";
            break;
        case BANK::control::selectSecondInstrumentAndSwap:
            if (name == "")
                name = "ped with Instrument ID " + std::to_string(insert + 1) + "  Bank ID " + std::to_string(kititem) + "  Root ID " + std::to_string(engine);
            contstr = "Swap" + name;
            break;

        case BANK::control::selectFirstBankToSwap:
            contstr = "Set Bank ID " + std::to_string(kititem) + "  Root ID " + std::to_string(engine) + " for swap";
            break;
        case BANK::control::selectSecondBankAndSwap:
            if (name == "")
                name = "ped with Bank ID " + std::to_string(kititem) + "  Root ID " + std::to_string(engine);
            contstr = "Swap" + name;
            break;
        default:
            contstr = "Unrecognised";
            break;
    }
    return ("Bank " + contstr);
}

std::string DataText::resolveMain(CommandBlock *getData)
{
    float value = getData->data.value.F;
    int value_int = lrint(value);

    unsigned char control = getData->data.control;
    unsigned char kititem = getData->data.kit;
    unsigned char engine = getData->data.engine;

    std::string name;
    std::string contstr = "";
    if (getData->data.part == TOPLEVEL::section::midiIn)
    {
        switch (control)
        {
            case MIDI::control::noteOn:
                showValue = false;
                break;
            case MIDI::control::noteOff:
                showValue = false;
                break;
            case MIDI::control::controller:
                contstr = "CC " + std::to_string(int(engine)) + " ";
                break;
            case MIDI::control::bankChange:
                showValue = false;
                contstr = miscMsgPop(value_int);
                break;
        }
        return contstr;
    }

    switch (control)
    {
        case MAIN::control::volume:
            contstr = "Volume";
            break;

        case MAIN::control::partNumber:
            showValue = false;
            contstr = "Part Number " + std::to_string(value_int + 1);
            break;
        case MAIN::control::availableParts:
            contstr = "Available Parts";
            break;

        case MAIN::control::detune:
            contstr = "Detune";
            break;
        case MAIN::control::keyShift:
            contstr = "Key Shift";
            break;
        case MAIN::control::mono:
            contstr = "Master Mono/Stereo ";
            showValue = false;
            if (value_int)
                contstr += "Mono";
            else
                contstr += "Stereo";
            break;

        case MAIN::control::soloType:
            showValue = false;
            contstr = "Chan 'solo' Switch - ";
            switch (value_int)
            {
                case 0:
                    contstr += "Off";
                    break;
                case 1:
                    contstr += "Row";
                    break;
                case 2:
                    contstr += "Column";
                    break;
                case 3:
                    contstr += "Loop";
                    break;
                case 4:
                    contstr += "Twoway";
                    break;
            }
            break;
        case MAIN::control::soloCC:
            showValue = false;
            contstr = "Chan 'solo' Switch CC ";
            if (value_int > 127)
                contstr += "undefined - set mode first";
            else
                contstr += std::to_string(value_int);
            break;

        case MAIN::control::exportBank:
            showValue = false;
            contstr = "Bank Export" + miscMsgPop(value_int);
            break;

        case MAIN::control::importBank:
            showValue = false;
            contstr = "Bank Import" + miscMsgPop(value_int);
            break;

        case MAIN::control::deleteBank:
            showValue = false;
            contstr = "Bank delete" + miscMsgPop(value_int);
            break;

        case MAIN::control::loadInstrumentFromBank:
            showValue = false;
            contstr = "Part " + to_string (int(kititem + 1)) + " load" + miscMsgPop(value_int);
            break;
        case MAIN::control::loadInstrumentByName:
            showValue = false;
            contstr = "Part " + to_string (int(kititem + 1)) + " load" + miscMsgPop(value_int);
            break;

        case MAIN::control::saveInstrument:
            showValue = false;
            contstr = "Bank Slot Save" + miscMsgPop(value_int);
            break;

        case MAIN::control::saveNamedInstrument:
            showValue = false;
            contstr = "Instrument Save" + miscMsgPop(value_int);
            break;

        case MAIN::control::loadNamedPatchset:
            showValue = false;
            contstr = "Patchset Load" + miscMsgPop(value_int);
            break;

        case MAIN::control::saveNamedPatchset:
            showValue = false;
            contstr = "Patchset Save" + miscMsgPop(value_int);
            break;

        case MAIN::control::loadNamedVector:
            showValue = false;
            name = miscMsgPop(value_int);
            contstr = "Vector Load" + name;
            break;

        case MAIN::control::saveNamedVector:
            showValue = false;
            name = miscMsgPop(value_int);
            contstr = "Vector Save" + name;
            break;

        case MAIN::control::loadNamedScale:
            showValue = false;
            name = miscMsgPop(value_int);
            contstr = "Scale Load" + name;
            break;

        case MAIN::control::saveNamedScale:
            showValue = false;
            name = miscMsgPop(value_int);
            contstr = "Scale Save" + name;
            break;

        case MAIN::control::loadNamedState:
            showValue = false;
            name = miscMsgPop(value_int);
            contstr = "State Load" + name;
            break;

        case MAIN::control::saveNamedState:
            showValue = false;
            contstr = "State Save" + miscMsgPop(value_int);
            break;

        case MAIN::control::loadFileFromList:
            showValue = false;
            contstr = "Load Recent" + miscMsgPop(value_int);
            break;

        case MAIN::control::exportPadSynthSamples:
            showValue = false;
            contstr = "PadSynth Samples Save" + miscMsgPop(value_int);
            break;

        case MAIN::control::masterReset:
            showValue = false;
            contstr = "Reset All";
            break;
        case MAIN::control::masterResetAndMlearn:
            showValue = false;
            contstr = "Reset All including MIDI-learn";
            break;

        case MAIN::control::openManualPDF:
            showValue = false;
            contstr = "Open manual in PDF reader " + miscMsgPop(value_int);
            break;

        case MAIN::control::startInstance:
            showValue = false;
            contstr = "Start new instance " + std::to_string(value_int);
            break;
        case MAIN::control::stopInstance:
            showValue = false;
            contstr = "Close instance - " + miscMsgPop(value_int);
            break;

        case MAIN::control::stopSound:
            showValue = false;
            contstr = "Sound Stopped";
            break;

        case MAIN::control::readPartPeak:
            showValue = false;
            if (engine == 1)
                contstr = "Part R";
            else
                contstr = "Part L";
            contstr += std::to_string(int(kititem));
            if (value < 0.0f)
                contstr += " silent ";
            contstr += (" peak level " + std::to_string(value));
            break;

        case MAIN::control::readMainLRpeak:
            showValue = false;
            if(kititem == 1)
                contstr = "Right";
            else
                contstr = "Left";
            contstr += (" peak level " + std::to_string(value));
            break;

        case MAIN::control::readMainLRrms:
            showValue = false;
            if(kititem == 1)
                contstr = "Right";
            else
                contstr = "Left";
            contstr += (" RMS level " + std::to_string(value));
            break;

        default:
            showValue = false;
            contstr = "Unrecognised";
    }

    return ("Main " + contstr);
}


std::string DataText::resolvePart(CommandBlock *getData)
{
    float value = getData->data.value.F;
    unsigned char control = getData->data.control;
    unsigned char npart = getData->data.part;
    unsigned char kititem = getData->data.kit;
    unsigned char engine = getData->data.engine;
    unsigned char insert = getData->data.insert;
    unsigned char parameter = getData->data.parameter;
    unsigned char effNum = engine;

    bool kitType = (insert == TOPLEVEL::insert::kitGroup);
    int value_int = lrint(value);
    bool value_bool = YOSH::F2B(value);
    bool yesno = false;

    if (control == UNUSED)
        return "Number of parts";

    std::string kitnum;
    if (kitType)
        kitnum = " Kit " + std::to_string(kititem + 1) + " ";
    else
        kitnum = " ";

    std::string name = "";
    if (control >= PART::control::volumeRange && control <= PART::control::receivePortamento)
    {
        name = "Controller ";
        if (control >= PART::control::portamentoTime)
            name += "Portamento ";
    }
    else if (control >= PART::control::midiModWheel && control <= PART::control::midiBandwidth)
        name = "MIDI ";
    else if (kititem != UNUSED)
    {
        switch (engine)
        {
            case PART::engine::addSynth:
                name = "AddSynth ";
                break;
            case PART::engine::subSynth:
                name = "SubSynth ";
                break;
            case PART::engine::padSynth:
                name = "PadSynth ";
                break;
        }
    }

    std::string contstr = "";
    switch (control)
    {
        case PART::control::volume:
            contstr = "Volume";
            break;
        case PART::control::velocitySense:
            contstr = "Vel Sens";
            break;
        case PART::control::panning:
            contstr = "Panning";
            break;
        case PART::control::velocityOffset:
            contstr = "Vel Offset";
            break;
        case PART::control::midiChannel:
            showValue = false;
            contstr = "Midi CH - " + std::to_string(value_int + 1);
            if (value_int >= NUM_MIDI_CHANNELS * 2)
                contstr += " Midi ignored";
            else if (value_int >= NUM_MIDI_CHANNELS)
                contstr = contstr + " Note off only on CH " + std::to_string(value_int + 1 - NUM_MIDI_CHANNELS);
            break;
        case PART::control::keyMode:
            showValue = false;
            contstr = "Mode - ";
            if (value_int == 0)
                contstr += "Poly";
            else if (value_int == 1)
                contstr += "Mono";
            else if (value_int >= 2)
                contstr += "Legato";
            break;
        case PART::control::portamento:
            contstr = "Portamento Enable";
            yesno = true;
            break;
        case PART::control::enable:
            contstr = "Enable";
            if (!kitType)
            {
                switch(engine)
                {
                    case PART::engine::addSynth:
                        contstr = "AddSynth " + contstr;
                        break;
                    case PART::engine::subSynth:
                        contstr = "SubSynth " + contstr;
                        break;
                    case PART::engine::padSynth:
                        contstr = "PadSynth " + contstr;
                        break;
                }
            }
            break;
        case PART::control::kitItemMute:
            if (kitType)
                contstr = "Mute";
            break;

        case PART::control::minNote:
            contstr = "Min Note";
            break;
        case PART::control::maxNote:
            contstr = "Max Note";
            break;
        case PART::control::minToLastKey: // always return actual value
            contstr = "Min To Last";
            break;
        case PART::control::maxToLastKey: // always return actual value
            contstr = "Max To Last";
            break;
        case PART::control::resetMinMaxKey:
            contstr = "Reset Key Range";
            break;

        case PART::control::kitEffectNum:
            if (kitType)
                contstr = "Effect Number";
            break;

        case PART::control::maxNotes:
            contstr = "Key Limit";
            break;
        case PART::control::keyShift:
            contstr = "Key Shift";
            break;

        case PART::control::partToSystemEffect1:
            contstr = "Effect Send 1";
            break;
        case PART::control::partToSystemEffect2:
            contstr = "Effect Send 2";
            break;
        case PART::control::partToSystemEffect3:
            contstr = "Effect Send 3";
            break;
        case PART::control::partToSystemEffect4:
            contstr = "Effect Send 4";
            break;

        case PART::control::humanise:
            contstr = "Humanise";
            break;

        case PART::control::drumMode:
            contstr = "Drum Mode";
            break;
        case PART::control::kitMode:
            contstr = "Kit Mode ";
            showValue = false;
            switch(value_int)
            {
                case 0:
                    contstr += "off";
                    break;
                case 1:
                    contstr += "multi";
                    break;
                case 2:
                    contstr += "single";
                    break;
                case 3:
                    contstr += "crossfade";
                    break;
            }
            break;

        case PART::control::effectNumber:
            contstr = "Effect Number";
            break;
        case PART::control::effectType:
            contstr = "Effect " + std::to_string(effNum + 1) + " Type";
            break;
        case PART::control::effectDestination:
            contstr = "Effect " + std::to_string(effNum + 1) + " Destination";
            break;
        case PART::control::effectBypass:
            contstr = "Bypass Effect "+ std::to_string(effNum + 1);
            break;

        case PART::control::defaultInstrument: // doClearPart
            contstr = "Set Default Instrument";
            break;

        case PART::control::audioDestination:
            contstr = "Audio destination ";
            showValue = false;
            switch(value_int)
            {
                case 3:
                    contstr += "both";
                    break;
                case 2:
                    contstr += "part";
                    break;
                case 1:
                    contstr += "main";
                    break;
                default:
                    contstr += "main";
                    break;
            }
            break;

        case PART::control::volumeRange:
            contstr = "Vol Range"; // not the *actual* volume
            break;
        case PART::control::volumeEnable:
            contstr = "Vol Enable";
            break;
        case PART::control::panningWidth:
            contstr = "Pan Width";
            break;
        case PART::control::modWheelDepth:
            contstr = "Mod Wheel Depth";
            break;
        case PART::control::exponentialModWheel:
            contstr = "Exp Mod Wheel";
            break;
        case PART::control::bandwidthDepth:
            contstr = "Bandwidth depth";
            break;
        case PART::control::exponentialBandwidth:
            contstr = "Exp Bandwidth";
            break;
        case PART::control::expressionEnable:
            contstr = "Expression Enable";
            break;
        case PART::control::FMamplitudeEnable:
            contstr = "FM Amp Enable";
            break;
        case PART::control::sustainPedalEnable:
            contstr = "Sustain Ped Enable";
            break;
        case PART::control::pitchWheelRange:
            contstr = "Pitch Wheel Range";
            break;
        case PART::control::filterQdepth:
            contstr = "Filter Q Depth";
            break;
        case PART::control::filterCutoffDepth:
            contstr = "Filter Cutoff Depth";
            break;
        case PART::control::breathControlEnable:
            yesno = true;
            contstr = "Breath Control";
            break;

        case PART::control::resonanceCenterFrequencyDepth:
            contstr = "Res Cent Freq Depth";
            break;
        case PART::control::resonanceBandwidthDepth:
            contstr = "Res Band Depth";
            break;

        case PART::control::portamentoTime:
            contstr = "Time";
            break;
        case PART::control::portamentoTimeStretch:
            contstr = "Tme Stretch";
            break;
        case PART::control::portamentoThreshold:
            contstr = "Threshold";
            break;
        case PART::control::portamentoThresholdType:
            contstr = "Threshold Type";
            break;
        case PART::control::enableProportionalPortamento:
            contstr = "Prop Enable";
            break;
        case PART::control::proportionalPortamentoRate:
            contstr = "Prop Rate";
            break;
        case PART::control::proportionalPortamentoDepth:
            contstr = "Prop depth";
            break;
        case PART::control::receivePortamento:
            contstr = "Receive";
            break;

        case PART::control::midiModWheel:
            contstr = "Modulation";
            break;
        case PART::control::midiBreath:
            ; // not yet
            break;
        case PART::control::midiExpression:
            contstr = "Expression";
            break;
        case PART::control::midiSustain:
            ; // not yet
            break;
        case PART::control::midiPortamento:
            ; // not yet
            break;
        case PART::control::midiFilterQ:
            contstr = "Filter Q";
            break;
        case PART::control::midiFilterCutoff:
            contstr = "Filter Cutoff";
            break;
        case PART::control::midiBandwidth:
            contstr = "Bandwidth";
            break;

        case PART::control::instrumentCopyright:
            ; // not yet
            break;
        case PART::control::instrumentComments:
            ; // not yet
            break;
        case PART::control::instrumentName:
            showValue = false;
            contstr = "Name is: " + miscMsgPop(value_int);
            break;
        case PART::control::defaultInstrumentCopyright:
            showValue = false;
            contstr = "Copyright ";
            if (parameter == 0)
                contstr += "load:\n";
            else
                contstr += "save:\n";
            contstr += miscMsgPop(value_int);
            break;
        case PART::control::resetAllControllers:
            contstr = "Clear controllers";
            break;

        case PART::control::partBusy:
            showValue = false;
            if (value_bool)
                contstr = "is busy";
            else
                contstr = "is free";
            break;

        default:
            showValue = false;
            contstr = "Unrecognised";

    }

    if (yesno)
    {
        if (value_bool)
            contstr += " - yes";
        else
            contstr += " - no";
        showValue = false;
    }
    return ("Part " + std::to_string(npart + 1) + kitnum + name + contstr);
}


std::string DataText::resolveAdd(CommandBlock *getData)
{
    unsigned char control = getData->data.control;
    unsigned char npart = getData->data.part;
    unsigned char kititem = getData->data.kit;

    std::string name = "";
    if (control <= ADDSYNTH::control::panning)
        name = " Amplitude ";
    else if (control >= ADDSYNTH::control::detuneFrequency && control <= ADDSYNTH::control::relativeBandwidth)
        name = "Frequency";

    std::string contstr = "";

    switch (control)
    {
        case ADDSYNTH::control::volume:
            contstr = "Volume";

            break;
        case ADDSYNTH::control::velocitySense:
            contstr = "Vel Sens";
            break;
        case ADDSYNTH::control::panning:
            contstr = "Panning";
            break;

        case ADDSYNTH::control::detuneFrequency:
            contstr = "Detune";
            break;

        case ADDSYNTH::control::octave:
            contstr = "Octave";
            break;
        case ADDSYNTH::control::detuneType:
            contstr = "Det type";
            break;
        case ADDSYNTH::control::coarseDetune:
            contstr = "Coarse Det";
            break;
        case ADDSYNTH::control::relativeBandwidth:
            contstr = "Rel B Wdth";
            break;

        case ADDSYNTH::control::stereo:
            contstr = "Stereo";
            break;
        case ADDSYNTH::control::randomGroup:
            contstr = "Rnd Grp";
            break;

        case ADDSYNTH::control::dePop:
            contstr = "De Pop";
            break;
        case ADDSYNTH::control::punchStrength:
            contstr = "Punch Strngth";
            break;
        case ADDSYNTH::control::punchDuration:
            contstr = "Punch Time";
            break;
        case ADDSYNTH::control::punchStretch:
            contstr = "Punch Strtch";
            break;
        case ADDSYNTH::control::punchVelocity:
            contstr = "Punch Vel";
            break;

        default:
            showValue = false;
            contstr = "Unrecognised";
    }

    return ("Part " + std::to_string(npart + 1) + " Kit " + std::to_string(kititem + 1) + " AddSynth " + name + contstr);
}


std::string DataText::resolveAddVoice(CommandBlock *getData)
{
    unsigned char control = getData->data.control;
    unsigned char npart = getData->data.part;
    unsigned char kititem = getData->data.kit;
    unsigned char engine = getData->data.engine;
    int nvoice;
    if (engine >= PART::engine::addMod1)
        nvoice = engine - PART::engine::addMod1;
    else
        nvoice = engine - PART::engine::addVoice1;

    std::string name = "";
    switch (control & 0xf0)
    {
        case ADDVOICE::control::volume:
            name = " Amplitude ";
            break;
        case ADDVOICE::control::modulatorType:
            name = " Modulator ";
            break;
        case ADDVOICE::control::detuneFrequency:
            name = " Frequency ";
            break;
        case ADDVOICE::control::unisonFrequencySpread:
            name = " Unison ";
            break;
        case ADDVOICE::control::bypassGlobalFilter:
            name = " Filter ";
            break;
        case ADDVOICE::control::modulatorAmplitude:
            name = " Modulator Amp ";
            break;
        case ADDVOICE::control::modulatorDetuneFrequency:
            name = " Modulator Freq ";
            break;
        case ADDVOICE::control::modulatorOscillatorPhase:
            name = " Modulator Osc ";
            break;
    }

    std::string contstr = "";

    switch (control)
    {
        case ADDVOICE::control::volume:
            contstr = "Volume";
            break;
        case ADDVOICE::control::velocitySense:
            contstr = "Vel Sens";
            break;
        case ADDVOICE::control::panning:
            contstr = "Panning";
            break;
        case ADDVOICE::control::invertPhase:
            contstr = "Minus";
            break;
        case ADDVOICE::control::enableAmplitudeEnvelope:
            contstr = "Enable Env";
            break;
        case ADDVOICE::control::enableAmplitudeLFO:
            contstr = "Enable LFO";
            break;

        case ADDVOICE::control::modulatorType:
            contstr = "Type";
            break;
        case ADDVOICE::control::externalModulator:
            contstr = "Extern Mod";
            break;

        case ADDVOICE::control::externalOscillator:
            contstr = "Extern Osc";
            break;

        case ADDVOICE::control::detuneFrequency:
            contstr = "Detune";
            break;
        case ADDVOICE::control::equalTemperVariation:
            contstr = "Eq T";
            break;
        case ADDVOICE::control::baseFrequencyAs440Hz:
            contstr = "440Hz";
            break;
        case ADDVOICE::control::octave:
            contstr = "Octave";
            break;
        case ADDVOICE::control::detuneType:
            contstr = "Det type";
            break;
        case ADDVOICE::control::coarseDetune:
            contstr = "Coarse Det";
            break;
        case ADDVOICE::control::pitchBendAdjustment:
            contstr = "Bend Adj";
            break;
        case ADDVOICE::control::pitchBendOffset:
            contstr = "Offset Hz";
            break;
        case ADDVOICE::control::enableFrequencyEnvelope:
            contstr = "Enable Env";
            break;
        case ADDVOICE::control::enableFrequencyLFO:
            contstr = "Enable LFO";
            break;

        case ADDVOICE::control::unisonFrequencySpread:
            contstr = "Freq Spread";
            break;
        case ADDVOICE::control::unisonPhaseRandomise:
            contstr = "Phase Rnd";
            break;
        case ADDVOICE::control::unisonStereoSpread:
            contstr = "Stereo";
            break;
        case ADDVOICE::control::unisonVibratoDepth:
            contstr = "Vibrato";
            break;
        case ADDVOICE::control::unisonVibratoSpeed:
            contstr = "Vib Speed";
            break;
        case ADDVOICE::control::unisonSize:
            contstr = "Size";
            break;
        case ADDVOICE::control::unisonPhaseInvert:
            contstr = "Invert";
            break;
        case ADDVOICE::control::enableUnison:
            contstr = "Enable";
            break;

        case ADDVOICE::control::bypassGlobalFilter:
            contstr = "Bypass Global";
            break;
        case ADDVOICE::control::enableFilter:
            contstr = "Enable";
            break;
        case ADDVOICE::control::enableFilterEnvelope:
            contstr = "Enable Env";
            break;
        case ADDVOICE::control::enableFilterLFO:
            contstr = "Enable LFO";
            break;

        case ADDVOICE::control::modulatorAmplitude:
            contstr = "Volume";
            break;
        case ADDVOICE::control::modulatorVelocitySense:
            contstr = "V Sense";
            break;
        case ADDVOICE::control::modulatorHFdamping:
            contstr = "F Damp";
            break;
        case ADDVOICE::control::enableModulatorAmplitudeEnvelope:
            contstr = "Enable Env";
            break;

        case ADDVOICE::control::modulatorDetuneFrequency:
            break;
        case ADDVOICE::control::modulatorFrequencyAs440Hz:
            contstr = "440Hz";
            break;
        case ADDVOICE::control::modulatorDetuneFromBaseOsc:
            contstr = "Follow voice";
            break;
        case ADDVOICE::control::modulatorOctave:
            contstr = "Octave";
            break;
        case ADDVOICE::control::modulatorDetuneType:
            contstr = "Det type";
            break;
        case ADDVOICE::control::modulatorCoarseDetune:
            contstr = "Coarse Det";
            break;
        case ADDVOICE::control::enableModulatorFrequencyEnvelope: // local, external
            contstr = "Enable Env";
            break;

        case ADDVOICE::control::modulatorOscillatorPhase:
            contstr = " Phase";
            break;
        case ADDVOICE::control::modulatorOscillatorSource:
            contstr = " Source";
            break;

        case ADDVOICE::control::delay:
            contstr = " Delay";
            break;
        case ADDVOICE::control::enableVoice:
            contstr = " Enable";
            break;
        case ADDVOICE::control::enableResonance:
            contstr = " Resonance Enable";
            break;
        case ADDVOICE::control::voiceOscillatorPhase:
            contstr = " Osc Phase";
            break;
        case ADDVOICE::control::voiceOscillatorSource:
            contstr = " Osc Source";
            break;
        case ADDVOICE::control::soundType:
            contstr = " Sound type";
            break;

        default:
            showValue = false;
            contstr = "Unrecognised";
    }

    return ("Part " + std::to_string(npart + 1) + " Kit " + std::to_string(kititem + 1) + " Add Voice " + std::to_string(nvoice + 1) + name + contstr);
}


std::string DataText::resolveSub(CommandBlock *getData)
{
    unsigned char control = getData->data.control;
    unsigned char npart = getData->data.part;
    unsigned char kititem = getData->data.kit;
    unsigned char insert = getData->data.insert;

    if (insert == TOPLEVEL::insert::harmonicAmplitude || insert == TOPLEVEL::insert::harmonicPhaseBandwidth)
    {
        std::string Htype;
        if (insert == TOPLEVEL::insert::harmonicAmplitude)
            Htype = " Amplitude";
        else
            Htype = " Bandwidth";

        return ("Part " + std::to_string(npart + 1) + " Kit " + std::to_string(kititem + 1) + " SubSynth Harmonic " + std::to_string(control + 1) + Htype);
    }

    std::string name = "";
    switch (control & 0x70)
    {
        case SUBSYNTH::control::volume:
            name = " Amplitude ";
            break;
        case SUBSYNTH::control::bandwidth:
            name = " Bandwidth ";
            break;
        case SUBSYNTH::control::detuneFrequency:
            name = " Frequency ";
            break;
        case SUBSYNTH::control::overtoneParameter1:
            name = " Overtones ";
            break;
        case SUBSYNTH::control::enableFilter:
            name = " Filter ";
            break;
    }

    std::string contstr = "";
    switch (control)
    {
        case SUBSYNTH::control::volume:
            contstr = "Volume";
            break;
        case SUBSYNTH::control::velocitySense:
            contstr = "Vel Sens";
            break;
        case SUBSYNTH::control::panning:
            contstr = "Panning";
            break;

        case SUBSYNTH::control::bandwidth:
            contstr = "";
            break;
        case SUBSYNTH::control::bandwidthScale:
            contstr = "Band Scale";
            break;
        case SUBSYNTH::control::enableBandwidthEnvelope:
            contstr = "Env Enab";
            break;

        case SUBSYNTH::control::detuneFrequency:
            contstr = "Detune";
            break;
        case SUBSYNTH::control::equalTemperVariation:
            contstr = "Eq T";
            break;
        case SUBSYNTH::control::baseFrequencyAs440Hz:
            contstr = "440Hz";
            break;
        case SUBSYNTH::control::octave:
            contstr = "Octave";
            break;
        case SUBSYNTH::control::detuneType:
            contstr = "Det type";
            break;
        case SUBSYNTH::control::coarseDetune:
            contstr = "Coarse Det";
            break;
        case SUBSYNTH::control::pitchBendAdjustment:
            contstr = "Bend Adj";
            break;
        case SUBSYNTH::control::pitchBendOffset:
            contstr = "Offset Hz";
            break;
        case SUBSYNTH::control::enableFrequencyEnvelope:
            contstr = "Env Enab";
            break;

        case SUBSYNTH::control::overtoneParameter1:
            contstr = "Par 1";
            break;
        case SUBSYNTH::control::overtoneParameter2:
            contstr = "Par 2";
            break;
        case SUBSYNTH::control::overtoneForceHarmonics:
            contstr = "Force H";
            break;
        case SUBSYNTH::control::overtonePosition:
            contstr = "Position";
            break;

        case SUBSYNTH::control::enableFilter:
            contstr = "Enable";
            break;

        case SUBSYNTH::control::filterStages:
            contstr = "Filt Stages";
            break;
        case SUBSYNTH::control::magType:
            contstr = "Mag Type";
            break;
        case SUBSYNTH::control::startPosition:
            contstr = "Start";
            break;

        case SUBSYNTH::control::clearHarmonics:
            contstr = "Clear Harmonics";
            break;

        case SUBSYNTH::control::stereo:
            contstr = "Stereo";
            break;

        default:
            showValue = false;
            contstr = "Unrecognised";
    }

    return ("Part " + std::to_string(npart + 1) + " Kit " + std::to_string(kititem + 1) + " SubSynth " + name + contstr);
}


std::string DataText::resolvePad(CommandBlock *getData)
{
    unsigned char type = getData->data.type;
    unsigned char control = getData->data.control;
    unsigned char npart = getData->data.part;
    unsigned char kititem = getData->data.kit;
    bool write = (type & TOPLEVEL::type::Write) > 0;

    std::string name = "";
    switch (control & 0x70)
    {
        case PADSYNTH::control::volume:
            name = " Amplitude ";
            break;
        case PADSYNTH::control::bandwidth:
            name = " Bandwidth ";
            break;
        case PADSYNTH::control::detuneFrequency:
            name = " Frequency ";
            break;
        case PADSYNTH::control::overtoneParameter1:
            name = " Overtones ";
            break;
        case PADSYNTH::control::baseWidth:
            name = " Harmonic Base ";
            break;
        case PADSYNTH::control::harmonicBase:
            name = " Harmonic Samples ";
            break;
    }

    std::string contstr = "";
    switch (control)
    {
        case PADSYNTH::control::volume:
            contstr = "Volume";
            break;
        case PADSYNTH::control::velocitySense:
            contstr = "Vel Sens";
            break;
        case PADSYNTH::control::panning:
            contstr = "Panning";
            break;

        case PADSYNTH::control::bandwidth:
            contstr = "Bandwidth";
            break;
        case PADSYNTH::control::bandwidthScale:
            contstr = "Band Scale";
            break;
        case PADSYNTH::control::spectrumMode:
            contstr = "Spect Mode";
            break;

        case PADSYNTH::control::detuneFrequency:
            contstr = "Detune";
            break;
        case PADSYNTH::control::equalTemperVariation:
            contstr = "Eq T";
            break;
        case PADSYNTH::control::baseFrequencyAs440Hz:
            contstr = "440Hz";
            break;
        case PADSYNTH::control::octave:
            contstr = "Octave";
            break;
        case PADSYNTH::control::detuneType:
            contstr = "Det type";
            break;
        case PADSYNTH::control::coarseDetune:
            contstr = "Coarse Det";
            break;

        case PADSYNTH::control::pitchBendAdjustment:
            contstr = "Bend Adj";
            break;
        case PADSYNTH::control::pitchBendOffset:
            contstr = "Offset Hz";
            break;

        case PADSYNTH::control::overtoneParameter1:
            contstr = "Overt Par 1";
            break;
        case PADSYNTH::control::overtoneParameter2:
            contstr = "Overt Par 2";
            break;
        case PADSYNTH::control::overtoneForceHarmonics:
            contstr = "Force H";
            break;
        case PADSYNTH::control::overtonePosition:
            contstr = "Position";
            break;

        case PADSYNTH::control::baseWidth:
            contstr = "Width";
            break;
        case PADSYNTH::control::frequencyMultiplier:
            contstr = "Freq Mult";
            break;
        case PADSYNTH::control::modulatorStretch:
            contstr = "Str";
            break;
        case PADSYNTH::control::modulatorFrequency:
            contstr = "Freq";
            break;
        case PADSYNTH::control::size:
            contstr = "Size";
            break;
        case PADSYNTH::control::baseType:
            contstr = "Type";
            break;
        case PADSYNTH::control::harmonicSidebands:
            contstr = "Halves";
            break;
        case PADSYNTH::control::spectralWidth:
            contstr = "Amp Par 1";
            break;
        case PADSYNTH::control::spectralAmplitude:
            contstr = "Amp Par 2";
            break;
        case PADSYNTH::control::amplitudeMultiplier:
            contstr = "Amp Mult";
            break;
        case PADSYNTH::control::amplitudeMode:
            contstr = "Amp Mode";
            break;
        case PADSYNTH::control::autoscale:
            contstr = "Autoscale";
            break;

        case PADSYNTH::control::harmonicBase:
            contstr = "Base";
            break;
        case PADSYNTH::control::samplesPerOctave:
            contstr = "samp/Oct";
            break;
        case PADSYNTH::control::numberOfOctaves:
            contstr = "Num Oct";
            break;
        case PADSYNTH::control::sampleSize:
            break;

        case PADSYNTH::control::applyChanges:
            showValue = false;
            contstr = "Changes Applied";
            break;

        case PADSYNTH::control::stereo:
            contstr = "Stereo";
            break;

        case PADSYNTH::control::dePop:
            contstr = "De Pop";
            break;
        case PADSYNTH::control::punchStrength:
            contstr = "Punch Strngth";
            break;
        case PADSYNTH::control::punchDuration:
            contstr = "Punch Time";
            break;
        case PADSYNTH::control::punchStretch:
            contstr = "Punch Strtch";
            break;
        case PADSYNTH::control::punchVelocity:
            contstr = "Punch Vel";
            break;

        default:
            showValue = false;
            contstr = "Unrecognised";
    }

    std::string isPad = "";

    if (write && ((control >= PADSYNTH::control::bandwidth && control <= PADSYNTH::control::spectrumMode) || (control >= PADSYNTH::control::overtoneParameter1 && control <= PADSYNTH::control::sampleSize)))
        isPad += " - Need to Apply";
    return ("Part " + std::to_string(npart + 1) + " Kit " + std::to_string(kititem + 1) + " PadSynth " + name + contstr + isPad);
}


std::string DataText::resolveOscillator(CommandBlock *getData)
{
    unsigned char type = getData->data.type;
    unsigned char control = getData->data.control;
    unsigned char npart = getData->data.part;
    unsigned char kititem = getData->data.kit;
    unsigned char engine = getData->data.engine;
    unsigned char insert = getData->data.insert;
    bool write = (type & TOPLEVEL::type::Write) > 0;

    std::string isPad = "";
    std::string eng_name;
    if (engine == PART::engine::padSynth)
    {
        eng_name = " Padsysnth";
        if (write)
            isPad = " - Need to Apply";
    }
    else
    {
        int eng;
        if (engine >= PART::engine::addMod1)
            eng = engine - PART::engine::addMod1;
        else
            eng = engine - PART::engine::addVoice1;
        eng_name = " Add Voice " + std::to_string(eng + 1);
        if (engine >= PART::engine::addMod1)
            eng_name += " Modulator";
    }

    if (insert == TOPLEVEL::insert::harmonicAmplitude)
    {
        return ("Part " + std::to_string(npart + 1) + " Kit " + std::to_string(kititem + 1) + eng_name + " Harmonic " + std::to_string((int)control + 1) + " Amplitude" + isPad);
    }
    else if(insert == TOPLEVEL::insert::harmonicPhaseBandwidth)
    {
        return ("Part " + std::to_string(npart + 1) + " Kit " + std::to_string(kititem + 1) + eng_name + " Harmonic " + std::to_string((int)control + 1) + " Phase" + isPad);
    }

    std::string name;
    if (control >= OSCILLATOR::control::clearHarmonics || control <= OSCILLATOR::control::harmonicRandomnessType)
        name = " Oscillator";
    else if (control >= OSCILLATOR::control::harmonicShift)
        name = " Harm Mods";
    else if (control >= OSCILLATOR::control::autoClear)
        name = " Base Mods";
    else
        name = " Base Funct";

    std::string contstr;
    switch (control)
    {
        case OSCILLATOR::control::phaseRandomness:
            contstr = " Random";
            break;
        case OSCILLATOR::control::magType:
            contstr = " Mag Type";
            break;
        case OSCILLATOR::control::harmonicAmplitudeRandomness:
            contstr = " Harm Rnd";
            break;
        case OSCILLATOR::control::harmonicRandomnessType:
            contstr = " Harm Rnd Type";
            break;

        case OSCILLATOR::control::baseFunctionParameter:
            contstr = " Par";
            break;
        case OSCILLATOR::control::baseFunctionType:
            contstr = " Type";
            break;
        case OSCILLATOR::control::baseModulationParameter1:
            contstr = " Mod Par 1";
            break;
        case OSCILLATOR::control::baseModulationParameter2:
            contstr = " Mod Par 2";
            break;
        case OSCILLATOR::control::baseModulationParameter3:
            contstr = " Mod Par 3";
            break;
        case OSCILLATOR::control::baseModulationType:
            contstr = " Mod Type";
            break;

        case OSCILLATOR::control::autoClear: // this is local to the GUI
            break;
        case OSCILLATOR::control::useAsBaseFunction:
            contstr = " Osc As Base";
            break;
        case OSCILLATOR::control::waveshapeParameter:
            contstr = " Waveshape Par";
            break;
        case OSCILLATOR::control::waveshapeType:
            contstr = " Waveshape Type";
            break;
        case OSCILLATOR::control::filterParameter1:
            contstr = " Osc Filt Par 1";
            break;
        case OSCILLATOR::control::filterParameter2:
            contstr = " Osc Filt Par 2";
            break;
        case OSCILLATOR::control::filterBeforeWaveshape:
            contstr = " Osc Filt B4 Waveshape";
            break;
        case OSCILLATOR::control::filterType:
            contstr = " Osc Filt Type";
            break;
        case OSCILLATOR::control::modulationParameter1:
            contstr = " Osc Mod Par 1";
            break;
        case OSCILLATOR::control::modulationParameter2:
            contstr = " Osc Mod Par 2";
            break;
        case OSCILLATOR::control::modulationParameter3:
            contstr = " Osc Mod Par 3";
            break;
        case OSCILLATOR::control::modulationType:
            contstr = " Osc Mod Type";
            break;
        case OSCILLATOR::control::spectrumAdjustParameter:
            contstr = " Osc Spect Par";
            break;
        case OSCILLATOR::control::spectrumAdjustType:
            contstr = " Osc Spect Type";
            break;

        case OSCILLATOR::control::harmonicShift:
            contstr = " Shift";
            break;
        case OSCILLATOR::control::clearHarmonicShift:
            contstr = " Reset";
            break;
        case OSCILLATOR::control::shiftBeforeWaveshapeAndFilter:
            contstr = " B4 Waveshape & Filt";
            break;
        case OSCILLATOR::control::adaptiveHarmonicsParameter:
            contstr = " Adapt Param";
            break;
        case OSCILLATOR::control::adaptiveHarmonicsBase:
            contstr = " Adapt Base Freq";
            break;
        case OSCILLATOR::control::adaptiveHarmonicsPower:
            contstr = " Adapt Power";
            break;
        case OSCILLATOR::control::adaptiveHarmonicsType:
            contstr = " Adapt Type";
            break;

        case OSCILLATOR::control::clearHarmonics:
            contstr = " Clear Harmonics";
            break;
        case OSCILLATOR::control::convertToSine:
            contstr = " Conv To Sine";
            break;

        default:
            showValue = false;
            contstr = "Unrecognised";
    }

    return ("Part " + std::to_string(npart + 1) + " Kit " + std::to_string(kititem + 1) + eng_name + name + contstr + isPad);
}


std::string DataText::resolveResonance(CommandBlock *getData)
{
    unsigned char type = getData->data.type;
    unsigned char control = getData->data.control;
    unsigned char npart = getData->data.part;
    unsigned char kititem = getData->data.kit;
    unsigned char engine = getData->data.engine;
    unsigned char insert = getData->data.insert;
    bool write = (type & TOPLEVEL::type::Write) > 0;

    std::string name;
    std::string isPad = "";
    if (engine == PART::engine::padSynth)
    {
        name = " PadSynth";
        if (write)
            isPad = " - Need to Apply";
    }
    else
        name = " AddSynth";

    if (insert == TOPLEVEL::insert::resonanceGraphInsert)
    {
        if (write == true && engine == PART::engine::padSynth)
            isPad = " - Need to Apply";
        return ("Part " + std::to_string(npart + 1) + " Kit " + std::to_string(kititem + 1) + name + " Resonance Point " + std::to_string(control + 1) + isPad);
    }

    if (write == true && engine == PART::engine::padSynth && control != 104)
        isPad = " - Need to Apply";
    std::string contstr;
    switch (control)
    {
        case RESONANCE::control::maxDb:
            contstr = "Max dB";
            break;
        case RESONANCE::control::centerFrequency:
            contstr = "Center Freq";
            break;
        case RESONANCE::control::octaves:
            contstr = "Octaves";
            break;

        case RESONANCE::control::enableResonance:
            contstr = "Enable";
            break;

        case RESONANCE::control::randomType:
            contstr = "Random";
            break;

        case RESONANCE::control::interpolatePeaks:
            contstr = "Interpolate Peaks";
            break;
        case RESONANCE::control::protectFundamental:
            contstr = "Protect Fundamental";
            break;

        case RESONANCE::control::clearGraph:
            contstr = "Clear";
            break;
        case RESONANCE::control::smoothGraph:
            contstr = "Smooth";
            break;

        default:
            showValue = false;
            contstr = "Unrecognised";
    }

    return ("Part " + std::to_string(npart + 1) + " Kit " + std::to_string(kititem + 1) + name + " Resonance " + contstr + isPad);
}


std::string DataText::resolveLFO(CommandBlock *getData)
{
    unsigned char control = getData->data.control;
    unsigned char npart = getData->data.part;
    unsigned char kititem = getData->data.kit;
    unsigned char engine = getData->data.engine;
    unsigned char insertParam = getData->data.parameter;

    std::string name;
    std::string lfo;

    if (engine == PART::engine::addSynth)
        name = " AddSynth";
    else if (engine == PART::engine::padSynth)
        name = " PadSynth";
    else if (engine >= PART::engine::addVoice1)
    {
        int nvoice = engine - PART::engine::addVoice1;
        name = " Add Voice " + std::to_string(nvoice + 1);
    }

    switch (insertParam)
    {
        case TOPLEVEL::insertType::amplitude:
            lfo = " Amp";
            break;
        case TOPLEVEL::insertType::frequency:
            lfo = " Freq";
            break;
        case TOPLEVEL::insertType::filter:
            lfo = " Filt";
            break;
    }

    std::string contstr;
    switch (control)
    {
        case LFOINSERT::control::speed:
            contstr = "Freq";
            break;
        case LFOINSERT::control::depth:
            contstr = "Depth";
            break;
        case LFOINSERT::control::delay:
            contstr = "Delay";
            break;
        case LFOINSERT::control::start:
            contstr = "Start";
            break;
        case LFOINSERT::control::amplitudeRandomness:
            contstr = "AmpRand";
            break;
        case LFOINSERT::control::type:
            contstr = "Type";
            break;
        case LFOINSERT::control::continuous:
            contstr = "Cont";
            break;
        case LFOINSERT::control::frequencyRandomness:
            contstr = "FreqRand";
            break;
        case LFOINSERT::control::stretch:
            contstr = "Stretch";
            break;

        default:
            showValue = false;
            contstr = "Unrecognised";
    }

    return ("Part " + std::to_string(npart + 1) + " Kit " + std::to_string(kititem + 1) + name + lfo + " LFO " + contstr);
}


std::string DataText::resolveFilter(CommandBlock *getData)
{
    int value_int = int(getData->data.value.F);
    unsigned char control = getData->data.control;
    unsigned char npart = getData->data.part;
    unsigned char kititem = getData->data.kit;
    unsigned char engine = getData->data.engine;

    int nseqpos = getData->data.parameter;
    int nformant = getData->data.parameter;
    int nvowel = getData->data.par2;

    std::string name;

    if (engine == PART::engine::addSynth)
        name = " AddSynth";
    else if (engine == PART::engine::subSynth)
        name = " SubSynth";
    else if (engine == PART::engine::padSynth)
        name = " PadSynth";
    else if (engine >= PART::engine::addVoice1)
        name = " Adsynth Voice " + std::to_string((engine - PART::engine::addVoice1) + 1);
    std::string contstr;
    switch (control)
    {
        case FILTERINSERT::control::centerFrequency:
            contstr = "C_Freq";
            break;
        case FILTERINSERT::control::Q:
            contstr = "Q";
            break;
        case FILTERINSERT::control::frequencyTracking:
            contstr = "FreqTrk";
            break;
        case FILTERINSERT::control::velocitySensitivity:
            contstr = "VsensA";
            break;
        case FILTERINSERT::control::velocityCurve:
            contstr = "Vsens";
            break;
        case FILTERINSERT::control::gain:
            contstr = "gain";
            break;
        case FILTERINSERT::control::stages:
            showValue = false;
            contstr = "Stages " + std::to_string(value_int + 1);
            break;
        case FILTERINSERT::control::baseType:
            contstr = "Filt Type";
            break;
        case FILTERINSERT::control::analogType:
            contstr = "An Type";
            break;
        case FILTERINSERT::control::stateVariableType:
            contstr = "SV Type";
            break;
        case FILTERINSERT::control::frequencyTrackingRange:
            contstr = "Fre Trk Offs";
            break;
        case FILTERINSERT::control::formantSlowness:
            contstr = "Form Fr Sl";
            break;
        case FILTERINSERT::control::formantClearness:
            contstr = "Form Vw Cl";
            break;
        case FILTERINSERT::control::formantFrequency:
            contstr = "Form Freq";
            break;
        case FILTERINSERT::control::formantQ:
            contstr = "Form Q";
            break;
        case FILTERINSERT::control::formantAmplitude:
            contstr = "Form Amp";
            break;
        case FILTERINSERT::control::formantStretch:
            contstr = "Form Stretch";
            break;
        case FILTERINSERT::control::formantCenter:
            contstr = "Form Cent Freq";
            break;
        case FILTERINSERT::control::formantOctave:
            contstr = "Form Octave";
            break;

        case FILTERINSERT::control::numberOfFormants:
            contstr = "Formants";
            break;
        case FILTERINSERT::control::vowelNumber:
            contstr = "Vowel Num";
            break;
        case FILTERINSERT::control::formantNumber:
            contstr = "Formant Num";
            break;
        case FILTERINSERT::control::sequenceSize:
            contstr = "Seq Size";
            break;
        case FILTERINSERT::control::sequencePosition:
            contstr = "Seq Pos";
            break;
        case FILTERINSERT::control::vowelPositionInSequence:
            contstr = "Vowel";
            break;
        case FILTERINSERT::control::negateInput:
            contstr = "Neg Input";
            break;

        default:
            showValue = false;
            contstr = "Unrecognised";
    }
    std::string extra = "";
    if (control >= FILTERINSERT::control::formantFrequency && control <= FILTERINSERT::control::formantAmplitude)
        extra ="Vowel " + std::to_string(nvowel) + " Formant " + std::to_string(nformant) + " ";
    else if (control == FILTERINSERT::control::vowelPositionInSequence)
        extra = "Seq Pos " + std::to_string(nseqpos) + " ";

    return ("Part " + std::to_string(npart + 1) + " Kit " + std::to_string(kititem + 1) + name + " Filter " + extra + contstr);
}


std::string DataText::resolveEnvelope(CommandBlock *getData)
{
    int value = lrint(getData->data.value.F);
    bool write = (getData->data.type & TOPLEVEL::type::Write) > 0;
    unsigned char control = getData->data.control;
    unsigned char npart = getData->data.part;
    unsigned char kititem = getData->data.kit;
    unsigned char engine = getData->data.engine;
    unsigned char insert = getData->data.insert;
    unsigned char insertParam = getData->data.parameter;
    int par2 = getData->data.par2;

    std::string env;
    std::string name;
    if (engine == PART::engine::addSynth)
        name = " AddSynth";
    else if (engine == PART::engine::subSynth)
        name = " SubSynth";

    else if (engine == PART::engine::padSynth)
        name = " PadSynth";
    else if (engine >= PART::engine::addVoice1)
    {
        name = " Add Voice ";
        int nvoice;
        if (engine >= PART::engine::addMod1)
            nvoice = engine - PART::engine::addMod1;
        else
            nvoice = engine - PART::engine::addVoice1;
        name += std::to_string(nvoice + 1);
        if (engine >= PART::engine::addMod1)
            name += " Modulator";
    }

    switch(insertParam)
    {
        case TOPLEVEL::insertType::amplitude:
            env = " Amp";
            break;
        case TOPLEVEL::insertType::frequency:
            env = " Freq";
            break;
        case TOPLEVEL::insertType::filter:
            env = " Filt";
            break;
        case TOPLEVEL::insertType::bandwidth:
            env = " B.Width";
            break;
    }

    if (insert == TOPLEVEL::insert::envelopePoints)
    {
        if (!write)
        {
            return ("Freemode add/remove is write only. Current points " + std::to_string(int(par2)));
        }
        if (par2 != UNUSED)
            return ("Part " + std::to_string(int(npart + 1)) + " Kit " + std::to_string(int(kititem + 1)) + name  + env + " Env Added Freemode Point " + std::to_string(int(control & 0x3f)) + " X increment " + std::to_string(int(par2)) + " Y");
        else
        {
            showValue = false;
            return ("Part " + std::to_string(int(npart + 1)) + " Kit " + std::to_string(int(kititem + 1)) + name  + env + " Env Removed Freemode Point " +  std::to_string(int(control)) + "  Remaining " +  std::to_string(value));
        }
    }

    if (insert == TOPLEVEL::insert::envelopePointChange)
    {
        return ("Part " + std::to_string(int(npart + 1)) + " Kit " + std::to_string(int(kititem + 1)) + name  + env + " Env Freemode Point " +  std::to_string(int(control)) + " X increment " + std::to_string(int(par2)) + " Y");
    }

    std::string contstr;
    switch (control)
    {
        case ENVELOPEINSERT::control::attackLevel:
            contstr = "A val";
            break;
        case ENVELOPEINSERT::control::attackTime:
            contstr = "A dt";
            break;
        case ENVELOPEINSERT::control::decayLevel:
            contstr = "D val";
            break;
        case ENVELOPEINSERT::control::decayTime:
            contstr = "D dt";
            break;
        case ENVELOPEINSERT::control::sustainLevel:
            contstr = "S val";
            break;
        case ENVELOPEINSERT::control::releaseTime:
            contstr = "R dt";
            break;
        case ENVELOPEINSERT::control::releaseLevel:
            contstr = "R val";
            break;
        case ENVELOPEINSERT::control::stretch:
            contstr = "Stretch";
            break;

        case ENVELOPEINSERT::control::forcedRelease:
            contstr = "frcR";
            break;
        case ENVELOPEINSERT::control::linearEnvelope:
            contstr = "L";
            break;

        case ENVELOPEINSERT::control::edit:
            contstr = "Edit";
            break;

        case ENVELOPEINSERT::control::enableFreeMode:
            contstr = "Freemode";
            break;
        case ENVELOPEINSERT::control::points:
            contstr = "Points";
            break;
        case ENVELOPEINSERT::control::sustainPoint:
            contstr = "Sust";
            break;

        default:
            showValue = false;
            contstr = "Unrecognised";
    }

    return ("Part " + std::to_string(npart + 1) + " Kit " + std::to_string(int(kititem + 1)) + name  + env + " Env " + contstr);
}


std::string DataText::resolveEffects(CommandBlock *getData)
{
    int value = lrint(getData->data.value.F);
    unsigned char control = getData->data.control;
    unsigned char npart = getData->data.part;
    unsigned char kititem = getData->data.kit;
    unsigned char effnum = getData->data.engine;
    unsigned char insert = getData->data.insert;
    unsigned char parameter = getData->data.parameter;

    std::string name;
    std::string actual;
    if (npart == TOPLEVEL::section::systemEffects)
        name = "System";
    else if (npart == TOPLEVEL::section::insertEffects)
        name = "Insert";
    else
        name = "Part " + std::to_string(npart + 1);

    if (kititem == EFFECT::type::dynFilter && getData->data.insert != UNUSED)
    {
        if (npart == TOPLEVEL::section::systemEffects)
            name = "System";
        else if (npart == TOPLEVEL::section::insertEffects)
            name = "Insert";
        else name = "Part " + std::to_string(npart + 1);
        name += " Effect " + std::to_string(effnum + 1);

        return (name + " DynFilter ~ Filter Internal Control " + std::to_string(control));
    }

    name += " Effect " + std::to_string(effnum + 1);

    std::string effname = "";
    if (npart < NUM_MIDI_PARTS && (control == PART::control::effectNumber || control == PART::control::effectDestination || control == PART::control::effectBypass))
    {
        if (control == PART::control::effectNumber)
            name = "Set " + name;
        else if (control == PART::control::effectDestination)
        {
            effname = " sent to ";
            if (value == 0)
                effname += "next effect";
            else if (value == 1)
                effname += "part out";
            else if (value == 2)
                effname += "dry out";
        }
        if (control == PART::control::effectBypass)
            effname = " bypassed";
        else
            showValue = false;
        return (name + effname);
    }
    else if (npart >= TOPLEVEL::section::systemEffects && kititem == UNUSED)
    {
        std::string contstr;
        std::string second = "";
        if (npart == TOPLEVEL::section::systemEffects && insert == TOPLEVEL::insert::systemEffectSend)
        {
            name = "System ";
            contstr = "from Effect " + std::to_string(effnum + 1);
            second = " to Effect " + std::to_string(control + 1);
            return (name + contstr + second);
        }
        if (npart == TOPLEVEL::section::insertEffects && control == EFFECT::sysIns::effectDestination)
        {
            contstr = " To ";
            if (value == -2)
                contstr += "Master out";
            else if (value == -1)
                contstr = " Off";
            else
            {
                contstr += "Part ";
                second = std::to_string(value + 1);
            }
            showValue = false;
            return ("Send " + name + contstr + second);
        }
        if (control == EFFECT::sysIns::effectNumber)
        {
            name = "Set " + name;
            showValue = false;
            return (name + effname);
        }
    }
    std::string contstr = "";
    if ((npart < NUM_MIDI_PARTS && control == PART::control::effectType) || (npart > TOPLEVEL::section::main && kititem == UNUSED && control == EFFECT::sysIns::effectType))
    {
        name += " set to";
        kititem = value | EFFECT::type::none; // TODO fix this!
        showValue = false;
    }
    else
        contstr = " Control " + std::to_string(control + 1);

    switch (kititem)
    {
        case EFFECT::type::none:
            effname = " None";
            contstr = " ";
            break;
        case EFFECT::type::reverb:
            effname = " Reverb";
            break;
        case EFFECT::type::echo:
            effname = " Echo";
            break;
        case EFFECT::type::chorus:
            effname = " Chorus";
            break;
        case EFFECT::type::phaser:
            effname = " Phaser";
            break;
        case EFFECT::type::alienWah:
            effname = " AlienWah";
            break;
        case EFFECT::type::distortion:
            effname = " Distortion";
            break;
        case EFFECT::type::eq:
            effname = " EQ";
            if (control > 1)
                contstr = " (Band " + std::to_string(int(parameter)) + ") Control " + std::to_string(control);
            break;
        case EFFECT::type::dynFilter:
            effname = " DynFilter";
            break;

        default:
            showValue = false;
            contstr = " Unrecognised";
    }

    if (kititem != EFFECT::type::eq && control == EFFECT::control::preset)
    {
        contstr = " Preset " + std::to_string (lrint(value) + 1);
        showValue = false;
    }

    return (name + effname + contstr);
}
