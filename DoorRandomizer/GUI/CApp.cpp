#include "CApp.h"

#if __linux__
    #include <X11/Xlib.h>
#endif

wxIMPLEMENT_APP(CApp);

CApp::CApp()
{
    #if __linux__
    XInitThreads();
    #endif
}


CApp::~CApp()
{
}

bool CApp::OnInit() {
	main_frame = new CRandomizerGUI();
	main_frame->Show();

	return true;
}
