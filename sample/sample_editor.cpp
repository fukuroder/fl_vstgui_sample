#include "lib/platform/platform_win32.h"
#include "sample_editor.h"
#include "fp_plugclass.h"

//----------------
// constructor
//----------------
sample_editor::sample_editor(TFruityPlug* effect, void* ptr)
	:plugin(effect), _controls{ nullptr }
{
	// background
	VSTGUI::CRect frameSize(0, 0, 170, 50);
	this->frame = new VSTGUI::CFrame(frameSize, this);
	this->frame->open(ptr, VSTGUI::PlatformType::kHWND);

	VSTGUI::CBitmap* background = new VSTGUI::CBitmap("background.png");
	this->frame->setBackground(background);
	background->forget();

	// knob
	VSTGUI::CRect r(0, 0, 30, 30);
	r.offset(130, 10);
	VSTGUI::CKnob* knob = new VSTGUI::CKnob(r, this, 0, nullptr, nullptr);
	knob->setColorHandle(VSTGUI::CColor(55, 55, 55, 200));
	knob->setColorShadowHandle(VSTGUI::kTransparentCColor);
	knob->setRangeAngle(static_cast<float>((M_PI*2) * 5 / 6));
	knob->setStartAngle(static_cast<float>((M_PI*2) / 3));
	knob->setWheelInc(0.01f);
	knob->setHandleLineWidth(2.5f);
	knob->setInsetValue(6.0f);
	knob->setMax((1 << 16));
	knob->setDefaultValue((1 << 16));
	this->_controls[0] = knob;

	// add control
	this->frame->addView(knob);

	// synchronize host parameters
	setParameter(0, static_cast<float>(this->plugin->ProcessParam(0, 0, REC_GetValue)));
}

//----------------
// destructor
//----------------
sample_editor::~sample_editor()
{
	if (frame != nullptr)
	{
		frame->forget();
	}
}

//-----------------------
// send mesasge to host
//-----------------------
void sample_editor::valueChanged(VSTGUI::CControl* pControl)
{
	int index = pControl->getTag();
	if(index < NumControls)
	{
		plugin->ProcessParam(index, static_cast<int>(pControl->getValue() + 0.5), REC_UpdateValue);
	}
}

void sample_editor::doIdleStuff()
{
	if (frame != nullptr)
	{
		frame->idle();
	}
}

//---------------------------
// recieve message from host
//---------------------------
void sample_editor::setParameter(int32_t index, float value)
{
	if (frame != nullptr && index < NumControls)
	{
		_controls[index]->setValue(value);
	}
}

void* sample_editor::getHWND()
{
	return frame->getPlatformFrame()->getPlatformRepresentation();
}
