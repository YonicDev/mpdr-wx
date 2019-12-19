#pragma once

#include <wx/msgdlg.h>

class CDialogsGUI {

public:
	CDialogsGUI(wxWindow *parent);
	~CDialogsGUI();

private:
	wxWindow *parent = nullptr;
	wxMessageDialog *messageBox = nullptr;

public:
	void setWindowParent(wxWindow *parent);
	void showError(wxString message,wxString caption="Error");
	void showInfo(wxString message, wxString caption = "Info");

};