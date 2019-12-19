#pragma once

#include <wx/wx.h>

#include "CRandomizerGUI.h"

class CApp : public wxApp
{
public:
	CApp();
	~CApp();

public:
	virtual bool OnInit();

private:
	CRandomizerGUI *main_frame = nullptr;
};

