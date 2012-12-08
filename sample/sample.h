#pragma once

#include "fp_plugclass.h"

const int NumParams = 1;    // the amount of parameters

class PluginGUIEditor;

class sample : public TFruityPlug
{
public:
    sample(int Tag, TFruityPlugHost *Host);
    virtual ~sample();

	// from TFruityPlug
    virtual int _stdcall Dispatcher(intptr_t ID, intptr_t Index, intptr_t Value);
    virtual void _stdcall Idle();
    virtual void _stdcall SaveRestoreState(IStream *Stream, BOOL Save);
    virtual void _stdcall GetName(int Section, int Index, int Value, char *Name);
    virtual int _stdcall ProcessEvent(int EventID, int EventValue, int Flags){return 0;}
    virtual int _stdcall ProcessParam(int Index, int Value, int RECFlags);
    virtual void _stdcall Eff_Render(PWAV32FS SourceBuffer, PWAV32FS DestBuffer, int Length);
    virtual void _stdcall Gen_Render(PWAV32FS DestBuffer, int &Length){}
    virtual TVoiceHandle _stdcall TriggerVoice(PVoiceParams VoiceParams, intptr_t SetTag){return 0;}
    virtual void _stdcall Voice_Release(TVoiceHandle Handle){}
    virtual void _stdcall Voice_Kill(TVoiceHandle Handle){}
    virtual int _stdcall Voice_ProcessEvent(TVoiceHandle Handle, int EventID, int EventValue, int Flags){return 0;}
    virtual int _stdcall Voice_Render(TVoiceHandle Handle, PWAV32FS DestBuffer, int &Length){return 0;}
    virtual void _stdcall NewTick(){}
    virtual void _stdcall MIDITick(){};
    virtual void _stdcall MIDIIn(int &Msg){}
    virtual void _stdcall MsgIn(int Msg){}

protected:
    // GUI
    PluginGUIEditor* _editor;

    // host
    TFruityPlugHost* _host;

    // parameter
    int		_params[NumParams];

    // gain
    float _gain;
};
