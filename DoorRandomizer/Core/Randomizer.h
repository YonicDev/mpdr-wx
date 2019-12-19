#pragma once

#include <string>

#include <rapidjson/rapidjson.h>
#include <rapidjson/writer.h>

class CRandomizerGUI;
#include "../GUI/CRandomizerGUI.h"

constexpr int MPDR_VERSION = 1;

class RandomizerConfig {
public:
	RandomizerConfig(std::string iso_path, std::string iso_output, unsigned long seed, uint8_t weights[5][4], bool skip_frigate, bool fix_flaaghra_music, std::string trilogy_path);
	std::string iso_path;
	std::string iso_output;
	unsigned long seed;
	bool skip_frigate;
	bool fix_flaaghra_music;
	std::string trilogy_path;
	uint8_t weights[5][4];

	template<class Writer>
	void Serialize(Writer &writer) {
		using namespace rapidjson;
		writer.StartObject();
		writer.String("input_iso");
		writer.String(iso_path.c_str(), static_cast<SizeType>(iso_path.length()));
		writer.String("output_iso");
		writer.String(iso_output.c_str(), static_cast<SizeType>(iso_output.length()));
		writer.String("seed");
		writer.Uint64(seed);
		writer.String("weights");
		writer.StartObject();
		for (int i = 0; i < 5; i++) {
			writer.String(area_names[i].c_str(),static_cast<SizeType>(area_names[i].length()));
			writer.StartArray();
			for (int j = 0; j < 4; j++) {
				writer.Int(weights[i][j]);
			}
			writer.EndArray();
		}
		writer.EndObject();
		writer.String("skip_frigate");
		writer.Bool(skip_frigate);
		writer.String("fix_flaaghra_music");
		writer.Bool(fix_flaaghra_music);
		writer.String("trilogy_disc_path");
		writer.String(trilogy_path.c_str(), static_cast<SizeType>(trilogy_path.length()));
		writer.EndObject();
	}
private:
	const std::string area_names[5] = {
		"tallon_overworld",
		"chozo_ruins",
		"magmoor_caverns",
		"phendrana_drifts",
		"phazon_mines"
	};
};

class Randomizer {
public:
	static int Randomize(RandomizerConfig *config, CRandomizerGUI *gui);
};