#include "sample_editor.h"
#include "sample.h"

//---------------------
// Plug-in information
//---------------------
TFruityPlugInfo PlugInfo =
{
	CurrentSDKVersion,
	"sample",
	"sample",
	FPF_Type_Effect,
	1 // the amount of parameters
};

//----------------
// DLL entry
//----------------
void* hInstance; // used by VSTGUI
extern "C" BOOL WINAPI DllMain(HINSTANCE hInst, DWORD dwReason, LPVOID lpvReserved)
{
	hInstance = hInst;
	return TRUE;
}

//----------------
// 
//----------------
extern "C" TFruityPlug* _stdcall CreatePlugInstance(TFruityPlugHost *Host, int Tag)
{
	return new sample(Tag, Host);
};

//----------------
// constructor
//----------------
sample::sample(int Tag, TFruityPlugHost *Host)
{
	Info = &PlugInfo;
	HostTag = Tag;
	EditorHandle = 0;
	_host = Host;
	_editor = nullptr;

	// parameter initialze
	_gain = 1.0;
	_params[0] = (1<<16);
}

//----------------
// destructor
//----------------
sample::~sample()
{
	delete _editor;
}

//-------------------------
// save or load parameter
//-------------------------
void _stdcall sample::SaveRestoreState(IStream *Stream, BOOL Save)
{
	if( Save )
	{
		// save paremeters
		unsigned long length = 0;
		Stream->Write(_params, sizeof(_params), &length);
	}
	else
	{
		// load paremeters
		unsigned long length = 0;
		Stream->Read(_params, sizeof(_params), &length);
		for( int ii = 0; ii < NumParams; ii++ )
		{
			if( ii == 0 )
			{
				_gain = static_cast<float>(_params[ii]) / (1<<16);
			}

			if( _editor != nullptr )
			{
				// send message to editor
				_editor->setParameter(ii, static_cast<float>(_params[ii]));
			}
		}
	}
}

//----------------
// 
//----------------
intptr_t _stdcall sample::Dispatcher(intptr_t ID, intptr_t Index, intptr_t Value)
{
	if( ID == FPD_ShowEditor )
	{
		if (Value == 0)
		{
			// close editor
			delete _editor;
			_editor = nullptr;
			EditorHandle = 0;
		}
		else if( EditorHandle == 0 )
		{
			if (_editor == nullptr)
			{
				// first
				_editor = new sample_editor(this, reinterpret_cast<HWND>(Value));
			}

			// open editor
			EditorHandle = reinterpret_cast<HWND>(_editor->getHWND());
		}
		else
		{
			// change parent window ?
			::SetParent(EditorHandle, reinterpret_cast<HWND>(Value));
		}
	}
	return 0;
}

//----------------
// 
//----------------
void _stdcall sample::GetName(int Section, int Index, int Value, char *Name)
{
	if(Section == FPN_Param)
	{
		strcpy_s(Name, 256, "Gain");
	}
}

int _stdcall sample::ProcessEvent(int EventID, int EventValue, int Flags)
{
	return 0;
}

//----------------
// 
//----------------
int _stdcall sample::ProcessParam(int Index, int Value, int RECFlags)
{
	int ret = 0;
	if( Index < NumParams )
	{
		if( RECFlags & REC_UpdateValue )
		{
			_params[Index] = Value;

			char hinttext[256] = { 0 };
			if( Index == 0 )
			{
				_gain = static_cast<float>(Value) / (1<<16);
				if( _gain < 1.0e-8)
				{
					// convert to dB
					sprintf_s(hinttext, "Gain: -oo dB");
				}
				else
				{
					// convert to dB
					sprintf_s(hinttext, "Gain: %.3f dB", 20.0 * log10(_gain));
				}
			}

			// display text to hint bar
			_host->OnHint(Index, hinttext);

			if( RECFlags & REC_UpdateControl )
			{
				// send message to editor
				_editor->setParameter(Index, static_cast<float>(Value));
			}
			else
			{
				// send message to host
				_host->OnParamChanged(this->HostTag, Index, Value);
			}
		}
		else if( RECFlags & REC_GetValue )
		{
			// get parameter
			ret = _params[Index];
		}
	}
	return ret;
}

//----------------
// idle
//----------------
void _stdcall sample::Idle_Public()
{
	if (_editor) _editor->doIdleStuff();
}

//----------------
// effect
//----------------
void _stdcall sample::Eff_Render(PWAV32FS SourceBuffer, PWAV32FS DestBuffer, int Length)
{
	float gain = _gain;
	for (int ii = 0; ii < Length; ii++)
	{
		(*DestBuffer)[ii][0] = (*SourceBuffer)[ii][0] * gain;
		(*DestBuffer)[ii][1] = (*SourceBuffer)[ii][1] * gain;
	}
}

void _stdcall sample::Gen_Render(PWAV32FS DestBuffer, int& Length)
{
}

TVoiceHandle _stdcall sample::TriggerVoice(PVoiceParams VoiceParams, intptr_t SetTag)
{
	return TVoiceHandle();
}

void _stdcall sample::Voice_Release(TVoiceHandle Handle)
{
}

void _stdcall sample::Voice_Kill(TVoiceHandle Handle)
{
}

int _stdcall sample::Voice_ProcessEvent(TVoiceHandle Handle, int EventID, int EventValue, int Flags)
{
	return 0;
}

int _stdcall sample::Voice_Render(TVoiceHandle Handle, PWAV32FS DestBuffer, int& Length)
{
	return 0;
}

void _stdcall sample::NewTick()
{
}

void _stdcall sample::MIDITick()
{
}

void _stdcall sample::MIDIIn(int& Msg)
{
}

void _stdcall sample::MsgIn(intptr_t Msg)
{
}

int _stdcall sample::OutputVoice_ProcessEvent(TOutVoiceHandle Handle, int EventID, int EventValue, int Flags)
{
	return 0;
}

void _stdcall sample::OutputVoice_Kill(TVoiceHandle Handle)
{
}
