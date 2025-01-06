#pragma once

namespace MikuMikuWorld
{
	struct Score;

	class ScoreStats
	{
	  private:
		int hispeeds, taps, flicks, holds, guides, steps, traces, total, combo;

		void resetCounts();
		void resetCombo();

	  public:
		ScoreStats();

		void calculateStats(const Score& score);
		void calculateCombo(const Score& score);
		void reset();

		int getHiSpeeds() const { return hispeeds; }
		int getTaps() const { return taps; }
		int getFlicks() const { return flicks; }
		int getHolds() const { return holds; }
		int getSteps() const { return steps; }
		int getGuides() const { return guides; }
		int getTraces() const { return traces; }
		int getTotal() const { return total; }
		int getCombo() const { return combo; }
	};
}