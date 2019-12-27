#pragma once

#include <wx/wx.h>
#include <wx/filepicker.h>
#include <wx/dnd.h>
#include <wx/collpane.h>
#include <wx/spinctrl.h>
#ifdef WIN32
#include <wx/appprogress.h>
#endif
#include "Log.h"
#include "../Core/Randomizer.h"
#include "CDialogsGUI.h"

class CFilePickerTarget : public wxFileDropTarget
{
public:
	CFilePickerTarget(wxFilePickerCtrl *owner);
	~CFilePickerTarget();

public:

	virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString &filenames);

private:
	wxFilePickerCtrl *owner;
};

class CRandomizerGUI : public wxFrame
{
public:
	CRandomizerGUI();
	~CRandomizerGUI();

	void set_randomizer_status(bool status);
	void set_progress_waiting();
	void set_progress_percentage(int percentage);
	void reset_progress(bool also_gauge);

	CDialogsGUI *msgdialog_manager = nullptr;

	DECLARE_EVENT_TABLE()

private:

	long unsigned int seed;

	wxMenuBar *menu_bar = nullptr;
	wxMenu *menu_file = nullptr;
	wxMenu *menu_help = nullptr;

	wxButton *button_seed = nullptr;
	wxButton *button_seed_time = nullptr;
	wxButton *button_randomize = nullptr;

	wxFileDialog *preset_dialog = nullptr;

	wxFilePickerCtrl *file_picker = nullptr;
	wxFilePickerCtrl *outfile_picker = nullptr;
	CFilePickerTarget *file_picker_target = nullptr;
	wxTextCtrl *seed_field = nullptr;

	wxStaticBoxSizer *weight_boxes[5];
	wxSlider *weight_sliders[5][4];
	wxSpinCtrl *weight_counters[5][4];

	wxBoxSizer *top_row = nullptr;
	wxStaticBoxSizer *mid_row = nullptr;
	wxBoxSizer *low_row = nullptr;
	wxBoxSizer *low_row_inner = nullptr;
	wxBoxSizer *main_box = nullptr;

	wxCollapsiblePane *details_panel = nullptr;
	wxBoxSizer *details_box = nullptr;
	wxCheckBox *fix_music_cb = nullptr;
	wxFilePickerCtrl *trilogy_picker = nullptr;
	wxCheckBox *skip_frigate_cb = nullptr;
	wxButton *button_exclude_doors = nullptr;

	wxGauge *progress_bar = nullptr;
	wxStaticText *status_label = nullptr;
	wxListCtrl *progress_logbox = nullptr;
    #ifdef WIN32
	wxAppProgressIndicator *progress_indicator = nullptr;
    #endif
	CLogger *logger_target = nullptr;

	const char* area_names[5] = {
		"Tallon Overworld",
		"Chozo Ruins",
		"Magmoor Caverns",
		"Phendrana Drifts",
		"Phazon Mines"
	};

	const char* door_types[4] = {
		"Blue",
		"Purple",
		"White",
		"Red"
	};

	virtual void OnRandomizerClick(wxCommandEvent &evt);
	virtual void OnOpenPresetClick(wxCommandEvent &evt);
	virtual void OnSavePresetClick(wxCommandEvent &evt);
	virtual void OnAboutClick(wxCommandEvent &evt);
	virtual void OnGenerateSeedClick(wxCommandEvent &evt);
	virtual void OnTimeSeedClick(wxCommandEvent &evt);
	virtual void OnFixMusicClick(wxCommandEvent &evt);
	virtual void OnCollapsePane(wxCollapsiblePaneEvent &evt);
	virtual void OnSliderChange(wxCommandEvent &evt);
	virtual void OnWeightFieldChange(wxCommandEvent &evt);
};
