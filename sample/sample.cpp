#include "sample.h"
#include "sample_editor.h"

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
int _stdcall sample::Dispatcher(intptr_t ID, intptr_t Index, intptr_t Value)
{
	if( ID == FPD_ShowEditor )
	{
		if( _editor == nullptr)
		{
			// first
			_editor = new sample_editor(this);
		}

        if (Value == 0)
        {
			// close editor
			_editor->close();
			EditorHandle = 0;
        }
        else if( EditorHandle == 0 )
		{
			// open editor
			_editor->open(reinterpret_cast<HWND>(Value));
			EditorHandle = static_cast<HWND>(_editor->getFrame()->getPlatformFrame()->getPlatformRepresentation());
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
					sprintf_s(hinttext, "Gain: %.3f dB", 20.0f * log10(_gain));
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
void _stdcall sample::Idle()
{
	if(_editor) _editor->idle();
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
