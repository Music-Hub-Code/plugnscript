// C++ scripting support-----------------------------
#include "dspapi.h"
#include "cpphelpers.h"

/** \file
*   MIDI transposer.
*   Shift MIDI Note events by the selected amount (semitones).
*/

#include "../library/Midi.h"

// metadata
DSP_EXPORT string name="MIDI Transpose";
DSP_EXPORT string author="Blue Cat Audio";
DSP_EXPORT string description="Transposes MIDI Note events by the selected amount";

// parameters definition
DSP_EXPORT array<string> inputParametersNames={"Pitch Shift"};
DSP_EXPORT array<double> inputParameters(inputParametersNames.length);

DSP_EXPORT array<double> inputParametersMin={-24};
DSP_EXPORT array<double> inputParametersMax={24};
DSP_EXPORT array<double> inputParametersDefault={0};
DSP_EXPORT array<int> inputParametersSteps={49};
DSP_EXPORT array<string> inputParametersUnits={"st"};
DSP_EXPORT array<string> inputParametersFormats={"+.0"};

// temp MIDI events for processing
MidiEvent tempEvent;

/* per-block processing function: called for every sample with updated parameters values.
*
*/
DSP_EXPORT void processBlock(BlockData& data)
{
    // store offset as integer
    int8 offset=int8(inputParameters[0]+.5);
    if(offset<0)
        offset-=1;

    // Read incoming MIDI events
    for(uint i=0;i<data.inputMidiEvents.length;i++)
    {
        // transpose Note On and Off events
        MidiEventType type=MidiEventUtils::getType(data.inputMidiEvents[i]);
        if(type==kMidiNoteOn || type==kMidiNoteOff)
        {
            tempEvent=data.inputMidiEvents[i];
            int8 note=MidiEventUtils::getNote(data.inputMidiEvents[i]);
            note+=offset;
            if(note>=0)
            {
                MidiEventUtils::setNote(tempEvent,note);
                data.outputMidiEvents.push(tempEvent);
            }
        }
        else
        {
            // forward other events (unchanged)
            data.outputMidiEvents.push(data.inputMidiEvents[i]);
        }
    }
}