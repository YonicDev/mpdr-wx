#pragma once

#include <wx/wx.h>

class CAboutGUI : public wxDialog
{
public:
	CAboutGUI();
	~CAboutGUI();

private:
	wxStaticText *about_text = nullptr;
	wxBoxSizer *wrapper = nullptr;
};

