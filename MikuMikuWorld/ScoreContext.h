#pragma once
#include "Audio/AudioManager.h"
#include "Audio/Waveform.h"
#include "Constants.h"
#include "HistoryManager.h"
#include "Jacket.h"
#include "JsonIO.h"
#include "Score.h"
#include "ScoreStats.h"
#include "TimelineMode.h"
#include <unordered_set>

namespace MikuMikuWorld
{
	struct EditArgs
	{
		int noteWidth{ 3 };
		FlickType flickType{ FlickType::Default };
		HoldStepType stepType{ HoldStepType::Normal };
		EaseType easeType{ EaseType::Linear };
		GuideColor colorType{ GuideColor::Green };
		FadeType fadeType{ FadeType::Out };

		float bpm{ 160.0f };
		int timeSignatureNumerator{ 4 };
		int timeSignatureDenominator{ 4 };
		float hiSpeed{ 1.0f };
	};

	class EditorScoreData
	{
	  public:
		std::string title{};
		std::string designer{};
		std::string artist{};
		std::string filename{};
		std::string musicFilename{};
		float musicOffset{};
		Jacket jacket{};

		EditorScoreData() {}
		EditorScoreData(const ScoreMetadata& metadata, const std::string& filename)
		    : title{ metadata.title }, designer{ metadata.author }, artist{ metadata.artist },
		      musicFilename{ metadata.musicFile }, musicOffset{ metadata.musicOffset }
		{
			this->filename = filename;
			jacket.load(metadata.jacketFile);
		}

		ScoreMetadata toScoreMetadata() const
		{
			return { title, artist, designer, musicFilename, jacket.getFilename(), musicOffset };
		}
	};

	struct PasteData
	{
		std::unordered_map<id_t, Note> notes;
		std::unordered_map<id_t, HoldNote> holds;
		std::unordered_map<id_t, Note> damages;
		std::unordered_map<id_t, HiSpeedChange> hiSpeedChanges;
		bool pasting{ false };
		int offsetTicks{};
		int offsetLane{};
		int midLane{};
		int minLaneOffset{};
		int maxLaneOffset{};
	};

	class ScoreContext
	{
	  public:
		Score score;
		EditorScoreData workingData;
		ScoreStats scoreStats;
		HistoryManager history;
		Audio::AudioManager audio;
		PasteData pasteData{};
		std::unordered_set<id_t> selectedNotes;
		std::unordered_set<id_t> selectedHiSpeedChanges;

		Audio::WaveformMipChain waveformL, waveformR;

		int currentTick{};
		bool upToDate{ true };

		int selectedLayer = 0;
		bool showAllLayers = false;

		bool hasSelection() const
		{
			return selectedNotes.size() > 0 || selectedHiSpeedChanges.size() > 0;
		}

		bool hasHoldInSelection() const
		{
			for (id_t id : selectedNotes)
			{
				const Note& n = score.notes.at(id);
				if (n.getType() == NoteType::Hold || n.getType() == NoteType::HoldMid ||
				    n.getType() == NoteType::HoldEnd)
					return true;
			}
			return false;
		}

		std::unordered_set<int> getHoldsFromSelection()
		{
			std::unordered_set<int> holds;
			for (id_t id : selectedNotes)
			{
				const Note& note = score.notes.at(id);
				if (note.getType() == NoteType::Hold)
					holds.insert(note.ID);
				else if (note.getType() == NoteType::HoldMid || note.getType() == NoteType::HoldEnd)
					holds.insert(note.parentID);
			}

			return holds;
		}

		double getTimeAtCurrentTick() const
		{
			return accumulateDuration(currentTick, TICKS_PER_BEAT, score.tempoChanges);
		}

		bool selectionHasEase() const;
		bool selectionHasHold() const;
		bool selectionHasStep() const;
		bool selectionHasFlickable() const;
		bool selectionCanConnect() const;
		bool selectionCanChangeHoldType() const;
		bool selectionCanChangeFadeType() const;
		inline bool isNoteSelected(const Note& note)
		{
			return selectedNotes.find(note.ID) != selectedNotes.end();
		}
		inline void selectAll()
		{
			selectedNotes.clear();
			for (auto& it : score.notes)
				selectedNotes.insert(it.first);
		}
		inline void clearSelection() { selectedNotes.clear(); }

		void setStep(HoldStepType step);
		void setFlick(FlickType flick);
		void setEase(EaseType ease);
		void setHoldType(HoldNoteType hold);
		void setFadeType(FadeType fade);
		void setGuideColor(GuideColor color);
		void setLayer(int layer);
		void toggleCriticals();
		void toggleFriction();

		void deleteSelection();
		void flipSelection();
		void cutSelection();
		void copySelection();
		void paste(bool flip);
		void duplicateSelection(bool flip);
		void doPasteData(const nlohmann::json& data, bool flip);
		void cancelPaste();
		void confirmPaste();
		void shrinkSelection(Direction direction);
		void compressSelection();

		void connectHoldsInSelection();
		void splitHoldInSelection();
		void repeatMidsInSelection(ScoreContext& context);
		/**
		 * @brief Convert normal holds or guide notes within selection into traces
		 * @param division Current division. Used to determine the ticks between two trace notes
		 * @param deleteOrigin Delete the original hold notes or not
		 */
		void convertHoldToTraces(int division, bool deleteOrigin);

		void lerpHiSpeeds(int division);

		void convertHoldToGuide(GuideColor color);
		void convertGuideToHold();


		void undo();
		void redo();
		void pushHistory(std::string description, const Score& prev, const Score& current);
	};
}
