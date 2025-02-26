#pragma once
#include "Constants.h"
#include "Note.h"
#include "Tempo.h"
#include <cstdint>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace MikuMikuWorld
{
	id_t getNextSkillID();
	id_t getNextHiSpeedID();

	struct SkillTrigger
	{
		id_t ID;
		int tick;
	};

	struct Fever
	{
		int startTick;
		int endTick;
	};

	struct Layer
	{
		std::string name;
		bool hidden = false;
	};

	struct Waypoint
	{
		std::string name;
		int tick;
	};

	struct HiSpeedChange
	{
		id_t ID;
		int tick;
		float speed;
		int layer = 0;
	};

	struct ScoreMetadata
	{
		std::string title;
		std::string artist;
		std::string author;
		std::string musicFile;
		std::string jacketFile;
		float musicOffset;

		int laneExtension = 0;
	};

	struct Score
	{
		ScoreMetadata metadata;
		std::unordered_map<id_t, Note> notes;
		std::unordered_map<id_t, HoldNote> holdNotes;
		std::vector<Tempo> tempoChanges;
		std::map<int, TimeSignature> timeSignatures;
		std::unordered_map<id_t, HiSpeedChange> hiSpeedChanges;
		std::unordered_map<id_t, SkillTrigger> skills;
		Fever fever;

		std::vector<Layer> layers{ { Layer{ "default" } } };
		std::vector<Waypoint> waypoints;

		Score();
	};

	Score deserializeScore(const std::string& filename);
	void serializeScore(const Score& score, const std::string& filename);
}
