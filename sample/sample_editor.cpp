#include "sample.h"
#include "sample_editor.h"

//----------------
// 
//----------------
sample_editor::sample_editor(TFruityPlug* effect):PluginGUIEditor(effect)
{
	rect.left = 0;
	rect.top = 0;
	rect.right = 200;
	rect.bottom = 175;

	setKnobMode( kLinearMode );
}

//----------------
// 
//----------------
sample_editor::~sample_editor()
{
}

//----------------
// 
//----------------
bool sample_editor::open(void* ptr)
{
	PluginGUIEditor::open(ptr);

	CRect frameSize (0, 0, 170, 50);
	frame = new CFrame (frameSize, ptr, this);
	CBitmap* background = new CBitmap ("panel.png");
	frame->setBackground(background);
	background->forget();

	CRect r (0, 0, 30, 30);
	r.offset(130, 10);

	CKnob* knob = new CKnob(r, this, 0, nullptr, nullptr);

	knob->setColorHandle( CColor(55,55,55,200) );
	knob->setColorShadowHandle( kTransparentCColor );
	knob->setRangeAngle( float(k2PI * 5 / 6) );
	knob->setStartAngle( float(k2PI / 3) );
	knob->setWheelInc( 0.01f );
	knob->setHandleLineWidth( 2.5f );
	knob->setInsetValue( 6.0f );
	knob->setMax((1<<16));

	_controls[0] = knob;
	frame->addView (knob);

	_controls[0]->setDefaultValue(0.5f * (1<<16));

	TFruityPlug *testplug = (TFruityPlug*)effect;

	//-- sync parameters
	setParameter(0, (float)testplug->ProcessParam(0, 0, REC_GetValue));
	return true;
}

//----------------
// 
//----------------
void sample_editor::close ()
{
	//-- on close we need to delete the frame object.
	//-- once again we make sure that the member frame variable is set to zero before we delete it
	//-- so that calls to setParameter won't crash.
	CFrame* oldFrame = frame;
	frame = 0;
	oldFrame->forget ();
}

//----------------
// 
//----------------
void sample_editor::valueChanged(CControl* pControl)
{
	TFruityPlug *testplug = (TFruityPlug*)effect;

	int index = pControl->getTag();
	if( index < NumControls )
	{
		testplug->ProcessParam(index, (int)pControl->getValue(), REC_UpdateValue );
	}
}

//----------------
// 
//----------------
void sample_editor::setParameter(long index, float value)
{
	if (frame && index < NumControls)
	{
		_controls[index]->setValue(value);
	}
}
