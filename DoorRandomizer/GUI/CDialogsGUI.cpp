#include "CDialogsGUI.h"

CDialogsGUI::CDialogsGUI(wxWindow *parent) {
	setWindowParent(parent);
}

CDialogsGUI::~CDialogsGUI() {

}

void CDialogsGUI::setWindowParent(wxWindow *parent) {
	this->parent = parent;
}

void CDialogsGUI::showError(wxString message,wxString caption) {
	messageBox = new wxMessageDialog(parent, message, caption, wxOK|wxOK_DEFAULT|wxICON_ERROR|wxCENTRE);
	messageBox->ShowModal();
	messageBox->Destroy();
}

void CDialogsGUI::showInfo(wxString message, wxString caption) {
	messageBox = new wxMessageDialog(parent, message, caption, wxOK | wxOK_DEFAULT | wxICON_INFORMATION | wxCENTRE);
	messageBox->ShowModal();
	messageBox->Destroy();
}