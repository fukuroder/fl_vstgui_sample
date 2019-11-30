#pragma once

#include "plugin-bindings/plugguieditor.h"

const int NumControls = 1;    // the amount of controls

class TFruityPlug;

class sample_editor : public VSTGUI::VSTGUIEditorInterface, public VSTGUI::IControlListener
{
public:
	sample_editor(TFruityPlug* effect, void* ptr);
	virtual ~sample_editor();

	// from VSTGUIEditorInterface
	virtual void doIdleStuff();

	void setParameter(int32_t index, float value);
	void* getHWND();

	// from IControlListener
	virtual void valueChanged(VSTGUI::CControl* pControl);

protected:
	TFruityPlug* plugin;
	VSTGUI::CControl* _controls[NumControls];
};
