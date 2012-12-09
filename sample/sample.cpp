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
	1, // the amount of parameters
	0  // infinite
};

//----------------
// 
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
sample::sample(int tag, TFruityPlugHost *host)
{
	Info = &PlugInfo;
	HostTag = tag;
	EditorHandle = 0;

	_host = host;
	_editor = new sample_editor(this);

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

//----------------
// 
//----------------
void _stdcall sample::SaveRestoreState(IStream *Stream, BOOL Save)
{
	if( Save )
	{
		unsigned long length = 0;
		Stream->Write(_params, sizeof(_params), &length);
	}
	else
	{
		unsigned long length = 0;
		Stream->Read(_params, sizeof(_params), &length);
		for( int ii = 0; ii < NumParams; ii++ )
		{
			if( ii == 0 )
			{
				_gain = ((float)_params[ii]) / (1<<16);
			}

			// update GUI
			_editor->setParameter(ii, (float)_params[ii]);
		}
	}
}

//----------------
// 
//----------------
int _stdcall sample::Dispatcher(intptr_t ID, intptr_t Index, intptr_t Value)
{
	switch (ID) {
	case FPD_ShowEditor:
		if (Value == 0)
		{
			// close
			_editor->close();
			EditorHandle = 0;
		}
		else
		{
			if( EditorHandle == 0 )
			{
				// open
				_editor->open((HWND)Value);
				EditorHandle = (HWND)_editor->getFrame()->getPlatformFrame()->getPlatformRepresentation();
			}
			else
			{
				// 
				::SetParent( EditorHandle, (HWND)Value );
			}
		}
		break;
	}
	return 0;
}

//----------------
// 
//----------------
void _stdcall sample::GetName(int Section, int Index, int Value, char *label)
{
	switch (Section)
	{
	case FPN_Param:
		strcpy_s(label, 256, "Gain");
		break;
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
				_gain = ((float)Value) / (1<<16);
				if( _gain < 1.0e-8)
				{
					// convert to dB
					sprintf_s(hinttext, 256, "Gain: -oo dB", 20.0f * log10(_gain));
				}
				else
				{
					// convert to dB
					sprintf_s(hinttext, 256, "Gain: %.3f dB", 20.0f * log10(_gain));
				}
			}

			_host->OnHint(Index, hinttext);

			if( RECFlags & REC_UpdateControl )
			{
				// update GUI
				_editor->setParameter(Index, (float)Value);
			}
			else
			{
				// send to host
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
   for (int ii = 0; ii < Length; ii++)
   {
		(*DestBuffer)[ii][0] = (*SourceBuffer)[ii][0] * _gain;
		(*DestBuffer)[ii][1] = (*SourceBuffer)[ii][1] * _gain;
   }
}
