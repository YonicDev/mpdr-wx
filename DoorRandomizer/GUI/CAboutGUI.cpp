#include "CAboutGUI.h"


CAboutGUI::CAboutGUI():wxDialog(nullptr, wxID_ANY, "About Metroid Prime Door Randomizer", wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
{
	about_text = new wxStaticText(this, wxID_ANY, "Metroid Prime Door Randomizer v0.1\nMIT License YonicStudios 2019",wxDefaultPosition,wxDefaultSize,wxALIGN_CENTER_HORIZONTAL);
	wrapper = new wxBoxSizer(wxHORIZONTAL);
	wrapper->Add(about_text, 1,wxALL,15);
	SetSizerAndFit(wrapper);
}


CAboutGUI::~CAboutGUI()
{
}
