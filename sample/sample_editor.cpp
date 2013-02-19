#include "sample.h"
#include "sample_editor.h"

//----------------
// constructor
//----------------
sample_editor::sample_editor(TFruityPlug* effect):PluginGUIEditor(effect)
{
	rect.left = 0;
	rect.top = 0;
	rect.right = 170;
	rect.bottom = 50;
	setKnobMode(kLinearMode);
}

//----------------
// open
//----------------
bool sample_editor::open(void* ptr)
{
	PluginGUIEditor::open(ptr);

	// background
	CRect frameSize(rect.left, rect.top, rect.right, rect.bottom);
	frame = new CFrame(frameSize, ptr, this);
	CBitmap* background = new CBitmap("background.png");
	frame->setBackground(background);
	background->forget();

	// knob
	CRect r (0, 0, 30, 30);
	r.offset(130, 10);
	CKnob* knob = new CKnob(r, this, 0, nullptr, nullptr);
	knob->setColorHandle(CColor(55,55,55,200));
	knob->setColorShadowHandle(kTransparentCColor);
	knob->setRangeAngle(static_cast<float>(k2PI * 5 / 6));
	knob->setStartAngle(static_cast<float>(k2PI / 3));
	knob->setWheelInc(0.01f);
	knob->setHandleLineWidth(2.5f);
	knob->setInsetValue(6.0f);
	knob->setMax((1<<16));
	knob->setDefaultValue((1<<16));
	_controls[0] = knob;

	// add control
	frame->addView(knob);

	// synchronize host parameters
	TFruityPlug *plugin = static_cast<TFruityPlug*>(getEffect());
	setParameter(0, static_cast<float>(plugin->ProcessParam(0, 0, REC_GetValue)));

	return true;
}

//----------------
// close
//----------------
void sample_editor::close()
{
	if(frame)
	{
		CFrame* oldFrame = frame;
		frame = nullptr;
		oldFrame->forget();
	}
}

//-----------------------
// send mesasge to host
//-----------------------
void sample_editor::valueChanged(CControl* pControl)
{
	TFruityPlug *plugin = static_cast<TFruityPlug*>(getEffect());

	int index = pControl->getTag();
	if(index < NumControls)
	{
		plugin->ProcessParam(index, static_cast<int>(pControl->getValue() + 0.5), REC_UpdateValue);
	}
}

//---------------------------
// recieve message from host
//---------------------------
void sample_editor::setParameter(long index, float value)
{
	if (frame && index < NumControls)
	{
		_controls[index]->setValue(value);
	}
}
