#include <random>
#include <limits>
#include <fstream>
#include <sstream>

#include <wx/regex.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/filewritestream.h>

#include "CRandomizerGUI.h"
#include "CAboutGUI.h"

BEGIN_EVENT_TABLE(CRandomizerGUI, wxFrame)
	EVT_BUTTON(1003, OnGenerateSeedClick)
	EVT_BUTTON(1004, OnTimeSeedClick)
	EVT_BUTTON(3001, OnRandomizerClick)
	EVT_COLLAPSIBLEPANE_CHANGED(4000, OnCollapsePane)
	EVT_CHECKBOX(4001, OnFixMusicClick)
	EVT_MENU(10001, OnOpenPresetClick)
	EVT_MENU(10002, OnSavePresetClick)
	EVT_MENU(20002, OnAboutClick)
END_EVENT_TABLE()

using namespace std;

CRandomizerGUI::CRandomizerGUI():wxFrame(nullptr,wxID_ANY,"Metroid Prime Door Randomizer",wxDefaultPosition,wxDefaultSize,wxDEFAULT_FRAME_STYLE)
{

	seed = static_cast<long unsigned int>(time(NULL));

	//Initialize layout boxes
	main_box = new wxBoxSizer(wxVERTICAL);
	top_row = new wxBoxSizer(wxHORIZONTAL);
	mid_row = new wxStaticBoxSizer(new wxStaticBox(this,wxID_ANY,"Door type weights"),wxHORIZONTAL);
	low_row = new wxBoxSizer(wxVERTICAL);
	low_row_inner = new wxBoxSizer(wxHORIZONTAL);

	//Menu bar
	menu_bar = new wxMenuBar();
	SetMenuBar(menu_bar);

	menu_file = new wxMenu();
	menu_file->Append(10001, "Open preset...\tCtrl+O");
	menu_file->Append(10002, "Save preset...\tCtrl+S");

	menu_help = new wxMenu();
	menu_help->Append(20001, "User Guide\tF1");
	menu_help->Append(20002, "About MPDR");

	menu_bar->Append(menu_file, "File");
	menu_bar->Append(menu_help, "Help");

	//Top row

	file_picker = new wxFilePickerCtrl(this, 1001, "NTSC 0-00 images only!", "Select Metroid Prime (NTSC 0-00) image file.", "GameCube disk images (*.iso, *.gcm)|*.iso;*.gcm");
	file_picker_target = new CFilePickerTarget(file_picker);
	outfile_picker = new wxFilePickerCtrl(this, 1005,"", "Select where to save the patched image file.", "GameCube disk images (*.iso, *.gcm)|*.iso;*.gcm",
		wxDefaultPosition,wxDefaultSize, wxFLP_SAVE|wxFLP_USE_TEXTCTRL| wxFLP_OVERWRITE_PROMPT);
	SetDropTarget(file_picker_target);

	seed_field = new wxTextCtrl(this, 1002, std::to_string(seed),wxDefaultPosition,wxSize(80,-1));
	button_seed = new wxButton(this, 1003, "Generate");
	button_seed_time = new wxButton(this, 1004, "Use time");

	top_row->Add(new wxStaticText(this, wxID_ANY, "Disk file:"), 0, wxALL, 9);
	top_row->Add(file_picker, 1, wxALL, 5);
	top_row->Add(new wxStaticText(this, wxID_ANY, "Output file:"), 0, wxALL, 9);
	top_row->Add(outfile_picker, 1, wxALL, 5);
	top_row->Add(new wxStaticText(this, wxID_ANY, wxString("Seed (0-").Append(std::to_string(ULONG_MAX).append("):"))), 0, wxALL, 9);
	top_row->Add(seed_field, 0, wxALL, 5);
	top_row->Add(button_seed,0,wxALL,5);
	top_row->Add(button_seed_time,0,wxALL,5);

	//Mid row
	for (int i = 0; i < 5; i++) {
		weight_boxes[i] = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxString(area_names[i])), wxHORIZONTAL);
		wxBoxSizer *slider_box = new wxBoxSizer(wxVERTICAL);
		wxBoxSizer *label_box = new wxBoxSizer(wxVERTICAL);
		wxBoxSizer *count_box = new wxBoxSizer(wxVERTICAL);
		mid_row->Add(weight_boxes[i], 1, wxALL, 5);
		for (int j = 0; j < 4; j++) {
			weight_sliders[i][j] = new wxSlider(this, 2001 + i*5 + j, 25, 0, 100);
			weight_sliders[i][j]->Bind(wxEVT_SLIDER, &CRandomizerGUI::OnSliderChange, this);
			wxStaticText *slider_label = new wxStaticText(this,wxID_ANY, wxString(door_types[j]));
			std::string count =std::to_string(weight_sliders[i][j]->GetValue());
			weight_counters[i][j] = new wxSpinCtrl(this, 2101 + i * 5 + j,count,wxDefaultPosition,wxSize(15*3,-1));
			weight_counters[i][j]->Bind(wxEVT_SPINCTRL, &CRandomizerGUI::OnWeightFieldChange, this);
			weight_counters[i][j]->SetRange(0, 100);
			label_box->Add(slider_label,1,wxTOP|wxBOTTOM,5);
			slider_box->Add(weight_sliders[i][j],0,wxEXPAND|wxBOTTOM,5);
			count_box->Add(weight_counters[i][j], 0, wxBOTTOM, 5);
		}
		weight_boxes[i]->Add(label_box,0,wxLEFT,5);
		weight_boxes[i]->Add(slider_box, 1, wxLEFT, 5);
		weight_boxes[i]->Add(count_box, 0);
	}

	//Details row

	details_panel = new wxCollapsiblePane(this, 4000, "Advanced settings");
	wxWindow *details_panel_win = details_panel->GetPane();

	details_box = new wxBoxSizer(wxVERTICAL);
	fix_music_cb = new wxCheckBox(details_panel_win, 4001, "Fix Flaaghra music");
	trilogy_picker = new wxFilePickerCtrl(details_panel_win, 4002);
	trilogy_picker->Disable();

	skip_frigate_cb = new wxCheckBox(details_panel_win, 4002, "Skip Pirate Frigate");
	button_exclude_doors = new wxButton(details_panel_win, 4003, "Exclude doors (coming soon)");
	skip_frigate_cb->SetValue(true);
	button_exclude_doors->Disable();


	wxBoxSizer *details_box_row1 = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer *details_box_row2 = new wxBoxSizer(wxHORIZONTAL);

	details_box_row1->Add(fix_music_cb, 0, wxALL, 9);
	details_box_row1->Add(new wxStaticText(details_panel_win, wxID_ANY, "Metroid Prime Trilogy file:"), 0, wxALL, 9);
	details_box_row1->Add(trilogy_picker, 1, wxALL, 5);

	details_box_row2->Add(skip_frigate_cb, 0, wxALL, 9);
	details_box_row2->Add(button_exclude_doors, 0, wxALL, 5);

	details_box->Add(details_box_row1, 0, wxEXPAND | wxBOTTOM, 5);
	details_box->Add(details_box_row2, 0, wxTOP, 5);

	details_panel_win->SetSizer(details_box);
	details_box->SetSizeHints(details_panel_win);

	//Bottom row

	progress_bar = new wxGauge(this, 3000, 100,wxDefaultPosition,wxSize(100,24));
	button_randomize = new wxButton(this, 3001, "Randomize");

	progress_logbox = new wxListCtrl(this, 3002,wxDefaultPosition,wxDefaultSize,wxLC_REPORT);
	progress_logbox->InsertColumn(0,"Type");
	progress_logbox->InsertColumn(1,"Timestamp");
	progress_logbox->InsertColumn(2,"Message");
	progress_logbox->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
	progress_logbox->SetColumnWidth(2, 800);

	low_row_inner->Add(progress_bar, 1, wxRIGHT, 5);
	low_row_inner->Add(button_randomize);
	low_row->Add(low_row_inner, 0, wxEXPAND|wxALL, 5);

	wxBoxSizer *status_container = new wxBoxSizer(wxHORIZONTAL);
	status_label = new wxStaticText(this, wxID_ANY, "Inactive");
	
	status_container->Add(new wxStaticText(this, wxID_ANY, "Current status: "),0);
	status_container->Add(status_label,1);

	low_row->Add(status_container, 0, wxEXPAND | wxALL,5);
	low_row->Add(progress_logbox, 1, wxEXPAND | wxALL, 5);

	// Progress indicator
	progress_indicator = new wxAppProgressIndicator(this);

	//Layout all rows
	main_box->Add(top_row,0,wxEXPAND|wxALL,10);
	main_box->Add(mid_row,0,wxEXPAND|wxALL,10);
	main_box->Add(details_panel,0,wxGROW|wxEXPAND|wxALL,10);
	main_box->Add(low_row, 1,wxEXPAND|wxALL, 10);

	SetSizerAndFit(main_box);
	main_box->Layout();

	SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_MENU));
	Refresh();

	// Set logging target
	logger_target = new CLogger(progress_logbox, status_label);
	wxLog::SetActiveTarget(logger_target);

	// Message dialog
	msgdialog_manager = new CDialogsGUI(this);
}


CRandomizerGUI::~CRandomizerGUI()
{
	delete msgdialog_manager;
}

void CRandomizerGUI::OnOpenPresetClick(wxCommandEvent &evt) {
	using namespace rapidjson;
	preset_dialog = new wxFileDialog(this, "Open preset file", "", ".json", "Preset files (*.json)|*.json", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (preset_dialog->ShowModal() != wxID_CANCEL) {
		ifstream json_file(preset_dialog->GetPath().ToStdString());
		if (json_file.is_open()) {
			string json_raw;
			json_raw.assign(istreambuf_iterator<char>(json_file),istreambuf_iterator<char>());
			json_file.close();

			Document doc;
			if (!doc.Parse(json_raw.c_str()).HasParseError()) {
				if (doc.IsObject() && doc.HasMember("mpdrp_version")) {
					seed_field->SetValue(to_string(doc["seed"].GetInt()));
					int i=0,j=0;
					for (Value::ConstMemberIterator area = doc["weights"].MemberBegin(); area != doc["weights"].MemberEnd(); ++area) {
						j = 0;
						GenericArray<true,Value::ValueType>area_weights = area->value.GetArray();
						for (Value::ConstValueIterator weight = area_weights.Begin(); weight != area_weights.End(); ++weight) {
							weight_counters[i][j]->SetValue(weight->GetInt());
							weight_sliders[i][j]->SetValue(weight->GetInt());
							++j;
						}
						++i;
					}
					wxLogDebug("Loaded JSON preset");
				} else {
					wxLogError("Not a valid JSON preset.");
				}
			}
			else {
				wxLogError("Error parsing the JSON file.");
			}
		}
		else {
			wxLogError("Could not load JSON at %s", preset_dialog->GetPath());
		}
		
	};
}

void CRandomizerGUI::OnSavePresetClick(wxCommandEvent &evt) {
	using namespace rapidjson;
	preset_dialog = new wxFileDialog(this, "Save preset file", "", "preset.json", "Preset files (*.json)|*.json", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (preset_dialog->ShowModal() != wxID_CANCEL) {
		Document doc;
		Document::AllocatorType &allocator = doc.GetAllocator();
		doc.SetObject();
		doc.AddMember("mpdrp_version", 1, allocator);
		doc.AddMember("seed",wxAtoi(seed_field->GetValue()),allocator);
		Value areas;
		areas.SetObject();
		for (int i = 0; i<5; ++i) {
			Value weights;
			weights.SetArray();
			for (int j = 0; j < 4; ++j) {
				Value weight;
				weight.SetInt(weight_counters[i][j]->GetValue());
				weights.PushBack(weight,allocator);
			}
			areas.AddMember(GenericStringRef<char>(area_names[i]), weights, allocator);
		}
		doc.AddMember("weights", areas,allocator);

		std::string path = trilogy_picker->IsEnabled() ? trilogy_picker->GetPath().ToStdString() : "";
		Value trilogy_path;
		trilogy_path.SetString(path.c_str(), allocator);
		doc.AddMember("trilogy_path", trilogy_path, allocator);

		Value skip_frigate;
		skip_frigate.SetBool(skip_frigate_cb->IsChecked());
		doc.AddMember("skip_frigate", skip_frigate, allocator);

		wxLogDebug("Successfully created JSON object.");

		struct Stream {
			ofstream json_file;
			typedef char Ch;
			void Put(Ch ch) { json_file.put(ch); }
			void Flush() {}
		} stream;
		stream.json_file = std::ofstream(preset_dialog->GetPath().ToStdString());

		if (stream.json_file.is_open()) {
			Writer<Stream> writer(stream);
			doc.Accept(writer);
			stream.json_file.close();
			wxLogMessage("Preset successfully saved to %s", preset_dialog->GetPath());
		} else {
			wxLogError("Could not save file to %s", preset_dialog->GetPath());
		}
	}
}

void CRandomizerGUI::OnRandomizerClick(wxCommandEvent &evt) {
	uint8_t weights[5][4];

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 4; j++) {
			weights[i][j] = weight_counters[i][j]->GetValue();
		}
	}

	unsigned long seed_long;
	stringstream(seed_field->GetValue().ToStdString()) >> seed_long;

	RandomizerConfig *config = new RandomizerConfig(
		file_picker->GetPath().ToStdString(),    // iso_path
		outfile_picker->GetPath().ToStdString(), // out_path
		seed_long,                               // seed
		weights,                                 // weights
		skip_frigate_cb->GetValue(),             // skip_frigate
		fix_music_cb->GetValue(),                // fix_flaaghra_music
		trilogy_picker->GetPath().ToStdString()  // trilogy_path
	);

	wxLogMessage("Setting up connection with randomizer...");

	int result = Randomizer::Randomize(config, this);

	evt.Skip();
}

void CRandomizerGUI::OnAboutClick(wxCommandEvent &evt) {
	CAboutGUI *about_frame = new CAboutGUI();
	about_frame->ShowModal();
	about_frame->Destroy();
	evt.Skip();
}

void CRandomizerGUI::OnGenerateSeedClick(wxCommandEvent &evt) {
	std::mt19937_64 eng(seed);
	std::uniform_int_distribution<unsigned long> distr;

	seed = distr(eng);
	seed_field->SetValue(std::to_string(seed));
	evt.Skip();
}

void CRandomizerGUI::OnTimeSeedClick(wxCommandEvent &evt) {
	seed = static_cast<long unsigned int>(time(NULL));
	seed_field->SetValue(std::to_string(seed));
	evt.Skip();
}

void CRandomizerGUI::OnFixMusicClick(wxCommandEvent &evt) {
	trilogy_picker->Enable(fix_music_cb->GetValue());
	evt.Skip();
}

void CRandomizerGUI::OnCollapsePane(wxCollapsiblePaneEvent &evt) {
	details_panel->GetPane()->Layout();
	details_panel->GetParent()->Layout();
	evt.Skip();
}

void CRandomizerGUI::OnSliderChange(wxCommandEvent &evt) {
	wxSlider *target = static_cast<wxSlider*>(evt.GetEventObject());
	int i, j;
	bool found = false;
	for (i = 0; i < 5 && !found; i++) {
		for (j = 0; j < 4 && !found; j++) {
			found = weight_sliders[i][j]->GetId() == target->GetId();
		}
	}
	wxSpinCtrl *linked = weight_counters[i-1][j-1];
	linked->SetValue(std::to_string(target->GetValue()));
	evt.Skip();
}

void CRandomizerGUI::OnWeightFieldChange(wxCommandEvent &evt) {
	wxSpinCtrl *target = static_cast<wxSpinCtrl*>(evt.GetEventObject());
	int i, j;
	bool found = false;
	for (i = 0; i < 5 && !found; i++) {
		for (j = 0; j < 4 && !found; j++) {
			found = weight_counters[i][j]->GetId() == target->GetId();
		}
	}
	wxSlider *linked = weight_sliders[i - 1][j - 1];
	linked->SetValue(target->GetValue());
	evt.Skip();
}

CFilePickerTarget::CFilePickerTarget(wxFilePickerCtrl *owner) {
	this->owner = owner;
}

CFilePickerTarget::~CFilePickerTarget() {
}

bool CFilePickerTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString &filenames) {
	bool check = filenames.Count() == 1 && wxRegEx("(\.iso)|(\.gcm)$").Matches(filenames[0]);
	if (check) {
		owner->SetPath(filenames[0]);
	}
	return check;
}

void CRandomizerGUI::set_progress_waiting() {
	progress_bar->Pulse();
	progress_indicator->Pulse();
}

void CRandomizerGUI::set_randomizer_status(bool status) {
	button_randomize->Enable(status);
}

void CRandomizerGUI::reset_progress(bool also_gauge) {
	progress_indicator->Reset();
	if (also_gauge) { progress_bar->SetValue(0); }
}

void CRandomizerGUI::set_progress_percentage(int percentage) {
	if (percentage <= 0) { percentage = 0; }
	else if (percentage >= progress_bar->GetRange()) { percentage = progress_bar->GetRange(); }
	progress_bar->SetValue(percentage);
	progress_indicator->SetValue(percentage);
}