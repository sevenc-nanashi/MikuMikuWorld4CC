#pragma once
#include "Constants.h"
#include "ScoreContext.h"
#include <atomic>
#include <unordered_set>

namespace MikuMikuWorld
{
	class NotesPreset
	{
	  private:
		id_t ID;
		std::string filename;

	  public:
		NotesPreset(id_t id, std::string name);
		NotesPreset();

		std::string name;
		std::string description;

		nlohmann::json data;

		inline std::string getName() const { return name; };
		inline std::string getFilename() const { return filename; }
		inline id_t getID() const { return ID; };

		Result read(const std::string& filepath);
		void write(std::string filepath, bool overwrite);
	};

	enum class UpdateMode
	{
		Create,
		Delete
	};

	class PresetManager
	{
	  private:
		std::atomic<int> nextPresetID;
		std::vector<int> createPresets;
		std::vector<std::string> deletePresets;

	  public:
		std::unordered_map<int, NotesPreset> presets;

		void loadPresets(const std::string& path);
		void savePresets(const std::string& path);

		void createPreset(const Score& score, const std::unordered_set<id_t>& selectedNotes,
		                  const std::unordered_set<id_t>& selectedHiSpeedChanges,
		                  const std::string& name, const std::string& desc);

		void removePreset(int id);

		// Replaces illegal filesystem characters with '_'
		std::string fixFilename(const std::string& name);

		void applyPreset(int presetId, ScoreContext& context);
	};
}
