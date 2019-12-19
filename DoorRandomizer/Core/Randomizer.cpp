#include "Randomizer.h"
#include <iomanip>
#include <sstream>
#include <stdlib.h>
#include <thread>

#include <rapidjson/document.h>

#include "../GUI/CDialogsGUI.h"

typedef void(*callback_t)(void *,const char *);
extern "C" void randomprime_patch_iso(const char *, void *, callback_t);

template< typename T>
std::string int_to_hex(T i) {
	using namespace std;
	stringstream stream;
	stream << "0x" << setfill('0') << setw(sizeof(T) * 2) << std::hex << i;
	return stream.str();
}

RandomizerConfig::RandomizerConfig(std::string iso_path, std::string iso_output, unsigned long seed, uint8_t weights[5][4], bool skip_frigate, bool fix_flaaghra_music, std::string trilogy_path) {
	this->iso_path = iso_path;
	this->iso_output = iso_output;
	this->seed = seed;
	this->skip_frigate = skip_frigate;
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 4; j++) {
			this->weights[i][j] = weights[i][j];
		}
	}
	this->trilogy_path = trilogy_path;
}

template<typename T>
void call_wrapper(void *ptr, const char*data) {
	(*static_cast<T*>(ptr))(data);
}

int Randomizer::Randomize(RandomizerConfig *config, CRandomizerGUI *gui) {

	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

	config->Serialize(writer);

	const char *parsed_config = sb.GetString();

	wxLogStatus("Setting up connection with randomizer...");

	auto prime_callback = [gui](const char *data) {
		rapidjson::Document message;
		message.Parse(data);

		const char *msg_type = message["type"].GetString();

		if (strcmp(msg_type, "error") == 0) {
			const char *msg_contents = message["msg"].GetString();
			wxLogError(msg_contents);
			gui->set_randomizer_status(true);
			gui->reset_progress(true);
			gui->msgdialog_manager->showError(std::string(msg_contents));
		}
		else if (strcmp(msg_type, "progress") == 0) {
			const char *msg_contents = message["msg"].GetString();
			const int percentage = std::floor(message["percent"].GetFloat());
			wxLogStatus(msg_contents);
			gui->set_progress_percentage(percentage);
		}
		else if (strcmp(msg_type, "success") == 0) {
			wxLogStatus("Done.");
			gui->set_progress_percentage(100);
			gui->reset_progress(false);
			gui->set_randomizer_status(true);
			gui->msgdialog_manager->showInfo("Finished patching!","Success");
		} else {
			wxLogDebug(data);
		}
	};

	std::thread prime_thread([parsed_config,prime_callback] {
		randomprime_patch_iso(parsed_config, (void *)&prime_callback, call_wrapper<decltype(prime_callback)>);
	});
	
	prime_thread.detach();

	gui->set_randomizer_status(false);
	gui->set_progress_waiting();
	
	delete config;
	return 0;

}