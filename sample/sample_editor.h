#pragma once

#include "plugin-bindings/plugguieditor.h"
#include "lib/platform/platformfactory.h"

const int NumControls = 1;    // the amount of controls

class TFruityPlug;

class sample_editor : public VSTGUI::VSTGUIEditorInterface, public VSTGUI::IControlListener
{
public:
	sample_editor(TFruityPlug* effect, void* ptr);
	virtual ~sample_editor();

	// from VSTGUIEditorInterface
	virtual void doIdleStuff();

	// from IControlListener
	virtual void valueChanged(VSTGUI::CControl* pControl);

	//
	void setParameter(int32_t index, float value);

	//
	void* getHWND();

protected:
	TFruityPlug* plugin;
	VSTGUI::CControl* _controls[NumControls];
};
