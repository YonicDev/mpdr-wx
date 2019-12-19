#pragma once

#include <wx/wx.h>
#include <wx/log.h>
#include <wx/listctrl.h>
#include <map>

class CLogger : public wxLog
{
public:
	CLogger(wxListCtrl* stack, wxStaticText* line);
	~CLogger();

private:
	virtual void DoLogRecord(wxLogLevel level, const wxString &msg, const wxLogRecordInfo &info);
	std::map<unsigned long, const char*> log_level_labels;
	wxListCtrl* log_stack;
	wxStaticText* log_line;
};

