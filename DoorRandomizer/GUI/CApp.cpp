#include "CApp.h"

wxIMPLEMENT_APP(CApp);

CApp::CApp()
{
}


CApp::~CApp()
{
}

bool CApp::OnInit() {

	main_frame = new CRandomizerGUI();
	main_frame->Show();

	return true;
}