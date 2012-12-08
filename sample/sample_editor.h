#pragma once

#include "plugin-bindings/plugguieditor.h"

const int NumControls = 1;    // the amount of controls

class sample_editor : public PluginGUIEditor, public CControlListener
{
public:
	sample_editor(TFruityPlug* effect);
	virtual ~sample_editor();

	// from PluginGUIEditor
	virtual bool open(void* ptr);
	virtual void close();
	virtual void setParameter(int32_t index, float value);

	// from CControlListener
	virtual void valueChanged(CControl* pControl);

protected:
	CControl* _controls[NumControls];
};
