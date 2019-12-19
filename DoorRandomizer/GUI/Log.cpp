#include "Log.h"

#include <ctime>



CLogger::CLogger(wxListCtrl* stack, wxStaticText* line)
{
	log_stack = stack;
	log_line = line;
	log_level_labels[wxLOG_FatalError] = "FATL";
	log_level_labels[wxLOG_Error] = "ERR";
	log_level_labels[wxLOG_Warning] = "WAR";
	log_level_labels[wxLOG_Message] = "MES";
	log_level_labels[wxLOG_Debug] = "DBG";
	log_level_labels[wxLOG_Progress] = "PGR";
}


CLogger::~CLogger()
{
}

void CLogger::DoLogRecord(wxLogLevel level, const wxString &msg, const wxLogRecordInfo &info) {

	long index = log_stack->InsertItem(0, log_level_labels[level]);

	const time_t raw_time = info.timestamp;
	struct tm* time_info = localtime(&raw_time);
	char timestamp[9];

	std::strftime(timestamp, 9, "%H:%M:%S",time_info);

	log_stack->SetItem(index,1, timestamp);
	log_stack->SetItem(index, 2, msg);
	if (level < wxLOG_Debug) {
		log_line->SetLabelText(msg);
	}
}
