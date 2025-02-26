#pragma once
#include "Constants.h"
#include "TimelineMode.h"
#include <map>
#include <unordered_map>
#include <vector>

namespace MikuMikuWorld
{
	struct HiSpeedChange;

	struct TimeSignature
	{
		int measure;
		int numerator;
		int denominator;
	};

	struct Tempo
	{
		int tick;
		float bpm;

		Tempo();
		Tempo(int tick, float bpm);
	};

	int snapTick(int tick, int div);
	float beatsPerMeasure(const TimeSignature& t);

	float ticksToSec(int ticks, int beatTicks, float bpm);
	int secsToTicks(float secs, int beatTicks, float bpm);

	float accumulateDuration(int tick, int beatTicks, const std::vector<Tempo>& tempos);
	int accumulateTicks(float sec, int beatTicks, const std::vector<Tempo>& tempos);

	int accumulateMeasures(int ticks, int beatTicks, const std::map<int, TimeSignature>& ts);
	int measureToTicks(int measure, int beatTicks, const std::map<int, TimeSignature>& ts);

	const Tempo& getTempoAt(int tick, const std::vector<Tempo>& tempos);
	int findTimeSignature(int measure, const std::map<int, TimeSignature>& ts);
	id_t findHighSpeedChange(int tick, const std::unordered_map<id_t, HiSpeedChange>& hiSpeeds,
	                         int selectedLayer);
}
