#include "ScoreContext.h"
#include "Constants.h"
#include "IO.h"
#include "UI.h"
#include "Utilities.h"
#include <stdio.h>
#include <map>
#include <unordered_map>
#include <vector>

using json = nlohmann::json;
using namespace IO;

namespace MikuMikuWorld
{
	constexpr const char* clipboardSignature = "MikuMikuWorld clipboard\n";

	void ScoreContext::setStep(HoldStepType type)
	{
		if (selectedNotes.empty())
			return;

		bool edit = false;
		Score prev = score;
		for (id_t id : selectedNotes)
		{
			const Note& note = score.notes.at(id);
			if (note.getType() != NoteType::HoldMid)
				continue;

			HoldNote& hold = score.holdNotes.at(note.parentID);
			if (hold.isGuide())
				continue;

			int pos = findHoldStep(hold, id);
			if (pos != -1)
			{
				if (type == HoldStepType::HoldStepTypeCount)
				{
					cycleStepType(hold.steps[pos]);
					edit = true;
				}
				else
				{
					// don't record history if the type did not change
					edit |= hold.steps[pos].type != type;
					hold.steps[pos].type = type;
				}
			}
		}

		if (edit)
			pushHistory("Change step type", prev, score);
	}

	void ScoreContext::setFlick(FlickType flick)
	{
		if (selectedNotes.empty())
			return;

		bool edit = false;
		Score prev = score;
		for (id_t id : selectedNotes)
		{
			Note& note = score.notes.at(id);
			bool canFlick = note.canFlick();

			if (note.getType() == NoteType::HoldEnd)
			{
				canFlick = score.holdNotes.at(note.parentID).endType == HoldNoteType::Normal;
			}

			if (canFlick)
			{
				if (flick == FlickType::FlickTypeCount)
				{
					cycleFlick(note);
					edit = true;
				}
				else
				{
					edit |= note.flick != flick;
					note.flick = flick;
				}
			}
		}

		if (edit)
			pushHistory("Change flick", prev, score);
	}

	void ScoreContext::setEase(EaseType ease)
	{
		if (selectedNotes.empty())
			return;

		bool edit = false;
		Score prev = score;
		for (id_t id : selectedNotes)
		{
			Note& note = score.notes.at(id);
			if (note.getType() == NoteType::Hold)
			{
				if (ease == EaseType::EaseTypeCount)
				{
					cycleStepEase(score.holdNotes.at(note.ID).start);
					edit = true;
				}
				else
				{
					edit |= score.holdNotes.at(note.ID).start.ease != ease;
					score.holdNotes.at(note.ID).start.ease = ease;
				}
			}
			else if (note.getType() == NoteType::HoldMid)
			{
				HoldNote& hold = score.holdNotes.at(note.parentID);
				int pos = findHoldStep(hold, id);
				if (pos != -1)
				{
					if (ease == EaseType::EaseTypeCount)
					{
						cycleStepEase(hold.steps[pos]);
						edit = true;
					}
					else
					{
						// don't record history if the type did not change
						edit |= hold.steps[pos].ease != ease;
						hold.steps[pos].ease = ease;
					}
				}
			}
		}

		if (edit)
			pushHistory("Change ease", prev, score);
	}

	void ScoreContext::setHoldType(HoldNoteType hold)
	{
		if (selectedNotes.empty())
			return;

		Score prev = score;
		bool edit = false;
		for (id_t id : selectedNotes)
		{
			// Invisible hold points cannot be trace notes!
			Note& note = score.notes.at(id);
			if (!(note.getType() == NoteType::Hold || note.getType() == NoteType::HoldEnd))
				continue;
			HoldNote& holdNote =
			    score.holdNotes.at(note.getType() == NoteType::Hold ? note.ID : note.parentID);

			// For now do not allow changing guides to normal holds or vice versa
			if (holdNote.isGuide())
				continue;

			if (note.getType() == NoteType::Hold)
			{
				if ((hold != HoldNoteType::Normal))
					note.friction = false;

				holdNote.startType = hold;
				edit = true;
			}
			else if (note.getType() == NoteType::HoldEnd)
			{
				// reset flick to none if the end is not normal
				if (hold != HoldNoteType::Normal)
				{
					note.flick = FlickType::None;
					note.friction = false;
				}

				holdNote.endType = hold;
				edit = true;
			}
		}

		if (edit)
			pushHistory("Change hold", prev, score);
	}

	void ScoreContext::setFadeType(FadeType fade)
	{
		if (selectedNotes.empty())
			return;

		Score prev = score;
		bool edit = false;
		for (id_t id : selectedNotes)
		{
			// Invisible hold points cannot be trace notes!
			Note& note = score.notes.at(id);

			if (!(note.getType() == NoteType::Hold || note.getType() == NoteType::HoldEnd))
				continue;
			HoldNote& holdNote =
			    score.holdNotes.at(note.getType() == NoteType::Hold ? note.ID : note.parentID);

			if (!holdNote.isGuide())
				continue;

			holdNote.fadeType = fade;
			edit = true;
		}

		if (edit)
			pushHistory("Change fade", prev, score);
	}

	void ScoreContext::setGuideColor(GuideColor color)
	{
		if (selectedNotes.empty())
			return;

		Score prev = score;
		bool edit = false;
		for (id_t id : selectedNotes)
		{
			// Invisible hold points cannot be trace notes!
			Note& note = score.notes.at(id);

			if (!(note.getType() == NoteType::Hold || note.getType() == NoteType::HoldEnd))
				continue;
			HoldNote& holdNote =
			    score.holdNotes.at(note.getType() == NoteType::Hold ? note.ID : note.parentID);

			if (!holdNote.isGuide())
				continue;

			if (color == GuideColor::GuideColorCount)
			{
				holdNote.guideColor =
				    (GuideColor)(((int)holdNote.guideColor + 1) % (int)GuideColor::GuideColorCount);
				edit = true;
			}
			else
			{
				if (holdNote.guideColor != color)
				{
					holdNote.guideColor = color;
					edit = true;
				}
			}
		}

		if (edit)
			pushHistory("Change guide", prev, score);
	}

	void ScoreContext::setLayer(int layer)
	{
		if (selectedNotes.empty())
			return;

		bool edit = false;
		Score prev = score;
		for (id_t id : selectedNotes)
		{
			Note& note = score.notes.at(id);

			if (note.layer == layer)
				continue;
			note.layer = layer;
			edit = true;
		}

		if (edit)
			pushHistory("Change layer", prev, score);
	}

	void ScoreContext::toggleCriticals()
	{
		if (selectedNotes.empty())
			return;

		Score prev = score;
		std::unordered_set<int> critHolds;
		for (id_t id : selectedNotes)
		{
			Note& note = score.notes.at(id);
			if (note.getType() == NoteType::Damage)
			// noop
			{
			}
			else if (note.getType() == NoteType::Tap)
			{
				note.critical ^= true;
			}
			else if (note.getType() == NoteType::HoldEnd && (note.isFlick() || note.friction))
			{
				// if the start is critical the entire hold must be critical
				note.critical = score.notes.at(note.parentID).critical ? true : !note.critical;
			}
			else
			{
				critHolds.insert(note.getType() == NoteType::Hold ? note.ID : note.parentID);
			}
		}

		for (auto& hold : critHolds)
		{
			// flip critical state
			HoldNote& note = score.holdNotes.at(hold);

			if (note.isGuide())
			{
				if (note.guideColor == GuideColor::Yellow)
				{
					note.guideColor = GuideColor::Green;
				}
				else
				{
					note.guideColor = GuideColor::Yellow;
				}
				continue;
			}
			bool critical = !score.notes.at(note.start.ID).critical;

			// again if the hold start is critical, every note in the hold must be critical
			score.notes.at(note.start.ID).critical = critical;
			score.notes.at(note.end).critical = critical;
			for (auto& step : note.steps)
				score.notes.at(step.ID).critical = critical;
		}

		pushHistory("Change critical note", prev, score);
	}

	void ScoreContext::toggleFriction()
	{
		if (selectedNotes.empty())
			return;

		Score prev = score;
		bool edit = false;
		for (id_t id : selectedNotes)
		{
			// Hold steps and invisible hold points cannot be trace notes
			Note& note = score.notes.at(id);
			if (note.getType() == NoteType::HoldMid)
				continue;

			if (note.getType() == NoteType::Hold || note.getType() == NoteType::HoldEnd)
			{
				HoldNote& holdNote =
				    score.holdNotes.at(note.getType() == NoteType::Hold ? note.ID : note.parentID);
				if (holdNote.isGuide())
					continue;

				if (note.getType() == NoteType::Hold)
				{
					holdNote.startType = HoldNoteType::Normal;
				}
				else
				{
					holdNote.endType = HoldNoteType::Normal;
					if (!note.isFlick() && note.friction && !score.notes.at(note.parentID).critical)
					{
						// Prevent critical hold end if the hold start is not critical
						note.critical = false;
					}
				}
			}

			note.friction = !note.friction;
			edit = true;
		}

		if (edit)
			pushHistory("Change trace notes", prev, score);
	}

	void ScoreContext::deleteSelection()
	{
		if (selectedNotes.empty() && selectedHiSpeedChanges.empty())
			return;

		Score prev = score;
		for (auto& id : selectedNotes)
		{
			auto notePos = score.notes.find(id);
			if (notePos == score.notes.end())
				continue;

			Note& note = notePos->second;
			if (note.getType() != NoteType::Hold && note.getType() != NoteType::HoldEnd)
			{
				if (note.getType() == NoteType::HoldMid)
				{
					// find hold step and remove it from the steps data container
					if (score.holdNotes.find(note.parentID) != score.holdNotes.end())
					{
						std::vector<HoldStep>& steps = score.holdNotes.at(note.parentID).steps;
						steps.erase(std::find_if(steps.cbegin(), steps.cend(),
						                         [id](const HoldStep& s) { return s.ID == id; }));
					}
				}
				score.notes.erase(id);
			}
			else
			{
				const HoldNote& hold =
				    score.holdNotes.at(note.getType() == NoteType::Hold ? note.ID : note.parentID);
				score.notes.erase(hold.start.ID);
				score.notes.erase(hold.end);

				// hold steps cannot exist without a hold
				for (const auto& step : hold.steps)
					score.notes.erase(step.ID);

				score.holdNotes.erase(hold.start.ID);
			}
		}
		for (auto& id : selectedHiSpeedChanges)
		{
			score.hiSpeedChanges.erase(id);
		}

		selectedNotes.clear();
		selectedHiSpeedChanges.clear();
		pushHistory("Delete notes", prev, score);
	}

	void ScoreContext::flipSelection()
	{
		if (selectedNotes.empty())
			return;

		Score prev = score;
		for (id_t id : selectedNotes)
		{
			Note& note = score.notes.at(id);
			note.lane = MAX_LANE - note.lane - note.width + 1;

			if (note.flick == FlickType::Left)
				note.flick = FlickType::Right;
			else if (note.flick == FlickType::Right)
				note.flick = FlickType::Left;
		}

		pushHistory("Flip notes", prev, score);
	}

	void ScoreContext::cutSelection()
	{
		copySelection();
		deleteSelection();
	}

	void ScoreContext::copySelection()
	{
		if (selectedNotes.empty() && selectedHiSpeedChanges.empty())
			return;

		int minTick = INT_MAX;
		if (!selectedNotes.empty())
		{
			minTick = score.notes
			              .at(*std::min_element(
			                  selectedNotes.begin(), selectedNotes.end(), [this](int id1, int id2)
			                  { return score.notes.at(id1).tick < score.notes.at(id2).tick; }))
			              .tick;
		}
		if (!selectedHiSpeedChanges.empty())
		{
			minTick = std::min(
			    minTick, score.hiSpeedChanges
			                 .at(*std::min_element(selectedHiSpeedChanges.begin(),
			                                       selectedHiSpeedChanges.end(),
			                                       [this](int id1, int id2) {
				                                       return score.hiSpeedChanges.at(id1).tick <
				                                              score.hiSpeedChanges.at(id2).tick;
			                                       }))
			                 .tick);
		}

		json data =
		    jsonIO::noteSelectionToJson(score, selectedNotes, selectedHiSpeedChanges, minTick);

		std::string clipboard{ clipboardSignature };
		clipboard.append(data.dump());

		ImGui::SetClipboardText(clipboard.c_str());
	}

	void ScoreContext::cancelPaste() { pasteData.pasting = false; }

	void ScoreContext::doPasteData(const json& data, bool flip)
	{
		int baseId = 0;
		pasteData.notes.clear();
		pasteData.damages.clear();
		pasteData.holds.clear();
		pasteData.hiSpeedChanges.clear();

		if (jsonIO::arrayHasData(data, "notes"))
		{
			for (const auto& entry : data["notes"])
			{
				Note note = jsonIO::jsonToNote(entry, NoteType::Tap);
				note.ID = baseId++;
				note.layer = selectedLayer;

				pasteData.notes[note.ID] = note;
			}
		}

		if (jsonIO::arrayHasData(data, "damages"))
		{
			for (const auto& entry : data["damages"])
			{
				Note note = jsonIO::jsonToNote(entry, NoteType::Damage);
				note.ID = baseId++;
				note.layer = selectedLayer;

				pasteData.damages[note.ID] = note;
			}
		}

		if (jsonIO::arrayHasData(data, "holds"))
		{
			for (const auto& entry : data["holds"])
			{
				if (!jsonIO::keyExists(entry, "start") || !jsonIO::keyExists(entry, "end"))
					continue;

				Note start = jsonIO::jsonToNote(entry["start"], NoteType::Hold);
				start.ID = baseId++;
				start.layer = selectedLayer;
				pasteData.notes[start.ID] = start;

				Note end = jsonIO::jsonToNote(entry["end"], NoteType::HoldEnd);
				end.ID = baseId++;
				end.parentID = start.ID;
				end.critical = start.critical || ((end.isFlick() || end.friction) && end.critical);
				end.layer = selectedLayer;
				pasteData.notes[end.ID] = end;

				std::string startEase =
				    jsonIO::tryGetValue<std::string>(entry["start"], "ease", "linear");

				HoldNote hold;
				hold.start = { start.ID, HoldStepType::Normal,
					           (EaseType)findArrayItem(startEase.c_str(), easeTypes,
					                                   arrayLength(easeTypes)) };
				hold.end = end.ID;
				for (int i = 0; i < arrayLength(fadeTypes); ++i)
				{
					if (entry["fade"] == fadeTypes[i])
					{
						hold.fadeType = (FadeType)i;
						break;
					}
				}
				for (int i = 0; i < arrayLength(guideColors); ++i)
				{
					if (entry["guide"] == guideColors[i])
					{
						hold.guideColor = (GuideColor)i;
						break;
					}
				}
				if (jsonIO::keyExists(entry, "steps"))
				{
					hold.steps.reserve(entry["steps"].size());
					for (const auto& step : entry["steps"])
					{
						Note mid = jsonIO::jsonToNote(step, NoteType::HoldMid);
						mid.critical = start.critical;
						mid.ID = baseId++;
						mid.parentID = start.ID;
						mid.layer = selectedLayer;
						pasteData.notes[mid.ID] = mid;

						std::string midType =
						    jsonIO::tryGetValue<std::string>(step, "type", "normal");
						std::string midEase =
						    jsonIO::tryGetValue<std::string>(step, "ease", "linear");
						int stepTypeIndex =
						    findArrayItem(midType.c_str(), stepTypes, arrayLength(stepTypes));
						int easeTypeIndex =
						    findArrayItem(midEase.c_str(), easeTypes, arrayLength(easeTypes));

						// Maintain compatibility with old step type names
						if (stepTypeIndex == -1)
						{
							stepTypeIndex = 0;
							if (midType == "invisible")
								stepTypeIndex = 1;
							if (midType == "ignored")
								stepTypeIndex = 2;
						}

						// Maintain compatibility with old ease type names
						if (easeTypeIndex == -1)
						{
							easeTypeIndex = 0;
							if (midEase == "in")
								easeTypeIndex = 1;
							if (midEase == "out")
								easeTypeIndex = 2;
						}

						hold.steps.push_back(
						    { mid.ID, (HoldStepType)stepTypeIndex, (EaseType)easeTypeIndex });
					}
				}

				std::string startType =
				    jsonIO::tryGetValue<std::string>(entry["start"], "type", "normal");
				std::string endType =
				    jsonIO::tryGetValue<std::string>(entry["end"], "type", "normal");

				if (startType == "guide" || endType == "guide")
				{
					hold.startType = hold.endType = HoldNoteType::Guide;
					start.friction = end.friction = false;
					end.flick = FlickType::None;
				}
				else
				{
					if (startType == "hidden")
					{
						hold.startType = HoldNoteType::Hidden;
						start.friction = false;
					}

					if (endType == "hidden")
					{
						hold.endType = HoldNoteType::Hidden;
						end.friction = false;
						end.flick = FlickType::None;
					}
				}

				pasteData.holds[hold.start.ID] = hold;
			}
		}

		int hiSpeedID = 0;

		if (jsonIO::arrayHasData(data, "hiSpeedChanges"))
		{
			for (const auto& entry : data["hiSpeedChanges"])
			{
				HiSpeedChange hs;
				hs.ID = hiSpeedID++;
				hs.tick = entry["tick"];
				hs.speed = entry["speed"];

				pasteData.hiSpeedChanges[hs.ID] = hs;
			}
		}

		if (flip)
		{
			for (auto& [_, note] : pasteData.notes)
			{
				note.lane = MAX_LANE - note.lane - note.width + 1;

				if (note.flick == FlickType::Left)
					note.flick = FlickType::Right;
				else if (note.flick == FlickType::Right)
					note.flick = FlickType::Left;
			}
			for (auto& [_, note] : pasteData.damages)
			{
				note.lane = MAX_LANE - note.lane - note.width + 1;
			}
		}

		pasteData.pasting = !(pasteData.notes.empty() && pasteData.damages.empty() &&
		                      pasteData.holds.empty() && pasteData.hiSpeedChanges.empty());
		if (pasteData.pasting)
		{
			// find the lane in which the cursor is in the middle of pasted notes
			float left = MAX_LANE + score.metadata.laneExtension;
			float right = MIN_LANE - score.metadata.laneExtension;
			float leftmostLane = MAX_LANE + score.metadata.laneExtension;
			float rightmostLane = MIN_LANE - score.metadata.laneExtension;
			for (const auto& [_, note] : pasteData.notes)
			{
				leftmostLane = std::min((float)leftmostLane, note.lane);
				rightmostLane = std::max((float)rightmostLane, note.lane + note.width - 1);
				left = std::min((float)left, note.lane + note.width);
				right = std::max((float)right, note.lane);
			}

			pasteData.minLaneOffset = MIN_LANE - score.metadata.laneExtension - leftmostLane;
			pasteData.maxLaneOffset = MAX_LANE + score.metadata.laneExtension - rightmostLane;
			pasteData.midLane = (left + right) / 2;
		}
	}

	void ScoreContext::confirmPaste()
	{
		Score prev = score;

		std::unordered_map<int, int> noteIDMap;

		auto getNewID = [this, &noteIDMap](int oldID) -> int
		{
			if (noteIDMap.find(oldID) != noteIDMap.end())
				return noteIDMap[oldID];
			auto id = Note::getNextID();
			noteIDMap[oldID] = id;
			return id;
		};

		// update IDs and copy notes
		for (auto& [_, note] : pasteData.notes)
		{
			note.ID = getNewID(note.ID);
			if (note.parentID != -1)
				note.parentID = getNewID(note.parentID);

			note.lane += pasteData.offsetLane;
			note.tick += pasteData.offsetTicks;
			note.layer = selectedLayer;
			score.notes[note.ID] = note;
		}

		for (auto& [_, note] : pasteData.damages)
		{
			note.ID = getNewID(note.ID);
			if (note.parentID != -1)
				note.parentID = getNewID(note.parentID);

			note.lane += pasteData.offsetLane;
			note.tick += pasteData.offsetTicks;
			note.layer = selectedLayer;
			score.notes[note.ID] = note;
		}
		for (auto& [_, hold] : pasteData.holds)
		{
			hold.start.ID = getNewID(hold.start.ID);
			hold.end = getNewID(hold.end);
			for (auto& step : hold.steps)
				step.ID = getNewID(step.ID);

			score.holdNotes[hold.start.ID] = hold;
		}

		for (auto& [_, hsc] : pasteData.hiSpeedChanges)
		{
			hsc.ID = getNextHiSpeedID();
			hsc.layer = selectedLayer;
			hsc.tick += pasteData.offsetTicks;
			score.hiSpeedChanges[hsc.ID] = hsc;
		}

		// select newly pasted notes
		selectedNotes.clear();
		selectedHiSpeedChanges.clear();
		std::transform(pasteData.notes.begin(), pasteData.notes.end(),
		               std::inserter(selectedNotes, selectedNotes.end()),
		               [this](const auto& it) { return it.second.ID; });
		std::transform(pasteData.damages.begin(), pasteData.damages.end(),
		               std::inserter(selectedNotes, selectedNotes.end()),
		               [this](const auto& it) { return it.second.ID; });
		std::transform(pasteData.hiSpeedChanges.begin(), pasteData.hiSpeedChanges.end(),
		               std::inserter(selectedHiSpeedChanges, selectedHiSpeedChanges.end()),
		               [this](const auto& it) { return it.second.ID; });

		pasteData.pasting = false;
		pushHistory("Paste notes", prev, score);
	}

	void ScoreContext::paste(bool flip)
	{
		const char* clipboardDataPtr = ImGui::GetClipboardText();
		if (clipboardDataPtr == nullptr)
			return;

		std::string clipboardData(clipboardDataPtr);
		if (!startsWith(clipboardData, clipboardSignature))
			return;

		doPasteData(json::parse(clipboardData.substr(strlen(clipboardSignature))), flip);
	}

	void ScoreContext::duplicateSelection(bool flip)
	{
		copySelection();
		paste(flip);
	}

	void ScoreContext::shrinkSelection(Direction direction)
	{
		if ((selectedNotes.size() + selectedHiSpeedChanges.size()) < 2)
			return;

		enum class Type
		{
			Note,
			HiSpeed
		};

		Score prev = score;

		std::vector<std::pair<Type, id_t>> sortedSelection;
		for (auto noteID : selectedNotes)
			sortedSelection.push_back({ Type::Note, noteID });
		for (auto hscID : selectedHiSpeedChanges)
			sortedSelection.push_back({ Type::HiSpeed, hscID });
		std::sort(sortedSelection.begin(), sortedSelection.end(),
		          [this](auto a, auto b)
		          {
			          auto [aType, aId] = a;
			          auto [bType, bId] = b;

			          const int n1 = aType == Type::Note ? score.notes.at(aId).tick
			                                             : score.hiSpeedChanges.at(aId).tick;
			          const int n2 = bType == Type::Note ? score.notes.at(bId).tick
			                                             : score.hiSpeedChanges.at(bId).tick;
			          return n1 < n2;
		          });

		int factor = 1; // tick increment/decrement amount
		if (direction == Direction::Up)
		{
			// start from the last note
			std::reverse(sortedSelection.begin(), sortedSelection.end());
			factor = -1;
		}

		auto first = *sortedSelection.begin();

		int firstTick = first.first == Type::Note ? score.notes.at(first.second).tick
		                                          : score.hiSpeedChanges.at(first.second).tick;
		for (int i = 0; i < sortedSelection.size(); ++i)
		{

			if (sortedSelection[i].first == Type::Note)
			{
				Note& note = score.notes.at(sortedSelection[i].second);
				note.tick = firstTick + (i * factor);
			}
			else
			{
				HiSpeedChange& hsc = score.hiSpeedChanges.at(sortedSelection[i].second);
				hsc.tick = firstTick + (i * factor);
			}
		}

		const std::unordered_set<int> holds = getHoldsFromSelection();
		for (const auto& hold : holds)
			sortHoldSteps(score, score.holdNotes.at(hold));

		pushHistory("Shrink notes", prev, score);
	}

	void ScoreContext::compressSelection()
	{
		enum class Type
		{
			Note,
			HiSpeed
		};

		Score prev = score;

		std::map<int, std::vector<std::pair<Type, int>>> selection;
		for (auto noteID : selectedNotes)
		{
			int tick = score.notes.at(noteID).tick;
			selection[tick].push_back({ Type::Note, noteID });
		}
		for (auto hscID : selectedHiSpeedChanges)
		{
			int tick = score.hiSpeedChanges.at(hscID).tick;
			selection[tick].push_back({ Type::HiSpeed, hscID });
		}

		if (selection.size() < 2)
			return;
		auto it = selection.begin();
		auto selectedLayer = selectedNotes.size() + selectedHiSpeedChanges.size() >= 1
		                         ? score.notes.at(*selectedNotes.begin()).layer
		                         : score.hiSpeedChanges.at(*selectedHiSpeedChanges.begin()).layer;
		int firstTick = it->first;
		float hiSpeedAtStart = 1.0;
		int latestHiSpeedTick = -1;
		for (auto [id, hsc] : score.hiSpeedChanges)
		{
			if (hsc.layer != selectedLayer || hsc.tick > firstTick)
			{
				continue;
			}
			if (hsc.tick > latestHiSpeedTick)
			{
				latestHiSpeedTick = hsc.tick;
				hiSpeedAtStart = hsc.speed;
			}
		}
		float currentHiSpeed = 1.0;
		for (size_t i = 0; i < selection.size(); i++)
		{
			auto elements = &(it->second);
			auto next = std::next(it);
			int newTick = firstTick + i;
			// Shrink notes
			for (size_t j = 0; j < elements->size(); j++)
			{
				if (elements->at(j).first == Type::Note)
				{
					Note& note = score.notes.at(elements->at(j).second);
					note.tick = newTick;
				}
				else
				{
					// Handle the Hi-Speed change
					HiSpeedChange& hsc = score.hiSpeedChanges.at(elements->at(j).second);
					currentHiSpeed = hsc.speed;

				}
			}
			// Add Hi-Speed
			id_t id = getNextHiSpeedID();
			this->score.hiSpeedChanges[id].ID = id;
			this->score.hiSpeedChanges[id].tick = newTick;
			if (elements->front().first == Type::Note)
			{
				Note& note = score.notes.at(elements->front().second);
				this->score.hiSpeedChanges[id].layer = note.layer;
			}
			else
			{
				HiSpeedChange& hsc = score.hiSpeedChanges.at(elements->front().second);
				this->score.hiSpeedChanges[id].layer = hsc.layer;
			}
			if (i == selection.size() - 1)
				this->score.hiSpeedChanges[id].speed = hiSpeedAtStart;

			else
				this->score.hiSpeedChanges[id].speed = (next->first - it->first) * currentHiSpeed;

			// Erase other Hi-Speed changes
			for (size_t j = 0; j < elements->size(); j++)
			{
				if (elements->at(j).first == Type::HiSpeed)
				{
					// Erase the Hi-Speed change and deselect it
					score.hiSpeedChanges.erase(elements->at(j).second);
					selectedHiSpeedChanges.erase(elements->at(j).second);
				}
			}

			selectedHiSpeedChanges.insert(id);

			it = next;
		}

		const std::unordered_set<int> holds = getHoldsFromSelection();
		for (const auto& hold : holds)
			sortHoldSteps(score, score.holdNotes.at(hold));

		pushHistory("Compress notes", prev, score);
	}

	void ScoreContext::connectHoldsInSelection()
	{
		if (!selectionCanConnect())
			return;

		Score prev = score;
		Note& note1 = score.notes[*selectedNotes.begin()];
		Note& note2 = score.notes[*std::next(selectedNotes.begin())];

		// Determine correct order of notes
		Note& earlierNote = note1.getType() == NoteType::HoldEnd ? note1 : note2;
		Note& laterNote = note1.getType() == NoteType::HoldEnd ? note2 : note1;

		HoldNote& earlierHold = score.holdNotes[earlierNote.parentID];
		HoldNote& laterHold = score.holdNotes[laterNote.ID];

		// Connect both ends
		earlierHold.end = laterHold.end;
		laterNote.parentID = earlierHold.start.ID;

		// We need to determine whether the new end will be critical
		Note& earlierHoldStart = score.notes.at(earlierHold.start.ID);
		Note& laterHoldEnd = score.notes.at(score.holdNotes.at(laterNote.ID).end);
		laterHoldEnd.critical =
		    earlierHoldStart.critical ? true : laterHoldEnd.isFlick() && laterHoldEnd.critical;

		// Update later note's end parent ID
		laterHoldEnd.parentID = earlierHold.start.ID;

		// Copy over later note's steps
		for (auto& step : laterHold.steps)
		{
			if (earlierHold.isGuide())
			{
				step.type = HoldStepType::Hidden;
			}
			earlierHold.steps.push_back(step);

			Note& note = score.notes.at(step.ID);
			note.critical = earlierHoldStart.critical;
			note.parentID = earlierHold.start.ID;
		}

		// Create new steps to connect both ends
		Note earlierNoteAsMid =
		    Note(NoteType::HoldMid, earlierNote.tick, earlierNote.lane, earlierNote.width);
		earlierNoteAsMid.ID = Note::getNextID();
		earlierNoteAsMid.critical = earlierHoldStart.critical;
		earlierNoteAsMid.parentID = earlierHold.start.ID;
		earlierNoteAsMid.layer = earlierHoldStart.layer;

		Note laterNoteAsMid =
		    Note(NoteType::HoldMid, laterNote.tick, laterNote.lane, laterNote.width);
		laterNoteAsMid.ID = Note::getNextID();
		laterNoteAsMid.critical = earlierHoldStart.critical;
		laterNoteAsMid.parentID = earlierHold.start.ID;
		laterNoteAsMid.layer = earlierHoldStart.layer;

		// Insert new steps to their appropriate containers
		score.notes[earlierNoteAsMid.ID] = earlierNoteAsMid;
		score.notes[laterNoteAsMid.ID] = laterNoteAsMid;
		earlierHold.steps.push_back(
		    { earlierNoteAsMid.ID,
		      earlierHold.isGuide() ? HoldStepType::Hidden : HoldStepType::Normal,
		      EaseType::Linear });
		earlierHold.steps.push_back(
		    { laterNoteAsMid.ID,
		      earlierHold.isGuide() ? HoldStepType::Hidden : laterHold.start.type,
		      laterHold.start.ease });

		// Remove old notes
		score.notes.erase(earlierNote.ID);
		score.notes.erase(laterNote.ID);
		score.holdNotes.erase(laterHold.start.ID);

		sortHoldSteps(score, earlierHold);

		selectedNotes.clear();
		selectedHiSpeedChanges.clear();
		selectedNotes.insert(earlierNoteAsMid.ID);
		selectedNotes.insert(laterNoteAsMid.ID);

		pushHistory("Connect holds", prev, score);
	}

	void ScoreContext::splitHoldInSelection()
	{
		if (selectedNotes.size() != 1)
			return;

		Score prev = score;

		Note& note = score.notes[*selectedNotes.begin()];
		if (note.getType() != NoteType::HoldMid)
			return;

		HoldNote& hold = score.holdNotes[note.parentID];

		int pos = findHoldStep(hold, note.ID);
		if (pos == -1)
			return;

		Note holdStart = score.notes.at(hold.start.ID);

		Note newSlideEnd = Note(NoteType::HoldEnd, note.tick, note.lane, note.width);
		newSlideEnd.ID = Note::getNextID();
		newSlideEnd.parentID = hold.start.ID;
		newSlideEnd.critical = note.critical;
		newSlideEnd.layer = holdStart.layer;

		Note newSlideStart = Note(NoteType::Hold, note.tick, note.lane, note.width);
		newSlideStart.ID = Note::getNextID();
		newSlideStart.critical = holdStart.critical;
		newSlideStart.layer = holdStart.layer;

		HoldNote newHold;
		newHold.end = hold.end;

		Note& slideEnd = score.notes.at(hold.end);
		slideEnd.parentID = newSlideStart.ID;

		hold.end = newSlideEnd.ID;
		newHold.start = { newSlideStart.ID, HoldStepType::Normal, hold.steps[pos].ease };
		newHold.startType = hold.startType;
		newHold.endType = hold.endType;
		newHold.fadeType = hold.fadeType;
		newHold.guideColor = hold.guideColor;

		// Backwards loop to avoid incorrect indices after removal
		for (int i = hold.steps.size() - 1; i > pos; i--)
		{
			HoldStep& step = hold.steps[i];
			Note& stepNote = score.notes.at(step.ID);
			stepNote.parentID = newSlideStart.ID;
			newHold.steps.push_back(step);
			hold.steps.erase(hold.steps.begin() + i);
		}

		hold.steps.pop_back();
		score.notes.erase(note.ID);

		sortHoldSteps(score, hold);
		sortHoldSteps(score, newHold);
		selectedNotes.clear();
		selectedHiSpeedChanges.clear();
		selectedNotes.insert(newSlideStart.ID);
		selectedNotes.insert(newSlideEnd.ID);

		score.notes[newSlideEnd.ID] = newSlideEnd;
		score.notes[newSlideStart.ID] = newSlideStart;
		score.holdNotes[newSlideStart.ID] = newHold;
		pushHistory("Split hold", prev, score);
	}

	void ScoreContext::repeatMidsInSelection(ScoreContext& context)
	{

		int selectedTickNum = 0;
		for (const auto& noteId : context.selectedNotes)
		{
			auto& note = context.score.notes.at(noteId);
			if (note.hasEase())
			{
				selectedTickNum += 1;
			}
		}
		if (selectedTickNum < 3)
		{
			return;
		}

		Score prev = score;

		Note& note = score.notes[*selectedNotes.begin()];
		if (!(note.getType() == NoteType::HoldMid || note.getType() == NoteType::Hold))
			return;

		int holdIndex;

		if (note.getType() == NoteType::HoldMid)
		{
			holdIndex = note.parentID;
		}
		else
		{
			holdIndex = *selectedNotes.begin();
		}

		HoldNote& hold = score.holdNotes[holdIndex];

		std::vector<int> sortedSelection;

		for (const auto& noteId : context.selectedNotes)
		{
			auto& note = context.score.notes.at(noteId);
			if (note.hasEase())
			{
				sortedSelection.push_back(noteId);
			}
		}
		std::sort(sortedSelection.begin(), sortedSelection.end(),
		          [this](int a, int b) { return score.notes[a].tick < score.notes[b].tick; });

		Note& patternStart = score.notes.at(sortedSelection.front());
		Note& patternEnd = score.notes.at(sortedSelection.back());

		// TODO: Check this in ScoreEditorTimeline too (Otherwise it will become so unfriendly)
		/* if (patternStart.width != patternEnd.width) */
		/* 	return; */

		Note& holdStart = score.notes.at(hold.start.ID);
		Note& holdEnd = score.notes.at(hold.end);

		// score.notes.at(hold.start.ID).tick = 0;
		// score.notes.at(hold.end).flick = FlickType::Default;

		int patternHeight = patternEnd.tick - patternStart.tick;

		int iterations = std::floor((holdEnd.tick - holdStart.tick) / patternHeight);

		int startPos = findHoldStep(hold, patternStart.ID);
		int endPos = findHoldStep(hold, patternEnd.ID);

		if (startPos == -1)
		{
			hold.steps[endPos].ease = hold.start.ease;
		}
		else
		{
			hold.steps[endPos].ease = hold.steps[startPos].ease;
		}

		float minLane = MIN_LANE - context.score.metadata.laneExtension;
		float maxLane = MAX_LANE + context.score.metadata.laneExtension + 1;

		for (int j = 1; j < sortedSelection.size(); j++)
		{
			Note& currentRep = score.notes.at(sortedSelection[j]);
			int jPos = findHoldStep(hold, currentRep.ID);

			for (int i = 1; i < iterations; i++)
			{
				float lane = std::clamp(currentRep.lane + i * (patternEnd.lane - patternStart.lane),
				                        minLane, maxLane - currentRep.width);

				if (j == sortedSelection.size() - 1 && i == iterations - 1)
				{
					holdEnd.tick = currentRep.tick + patternHeight * i;
					holdEnd.lane = lane;
					holdEnd.width = currentRep.width;
					continue;
				}

				Note nextMid = Note(NoteType::HoldMid, currentRep.tick + patternHeight * i, lane,
				                    currentRep.width);

				nextMid.critical = patternStart.critical;

				nextMid.parentID = hold.start.ID;
				nextMid.layer = currentRep.layer;

				nextMid.ID = Note::getNextID();
				score.notes[nextMid.ID] = nextMid;

				HoldStepType type = jPos == -1 ? hold.steps[0].type : hold.steps[jPos].type;

				int temp = jPos;

				if (j == sortedSelection.size() - 1)
				{
					jPos = findHoldStep(hold, score.notes.at(sortedSelection[0]).ID);
				}

				EaseType ease = jPos == -1 ? hold.start.ease : hold.steps[jPos].ease;

				jPos = temp;

				hold.steps.push_back({ nextMid.ID, type, ease });
			}
		}

		sortHoldSteps(score, hold);

		pushHistory("Repeat hold mids", prev, score);
	}

	void ScoreContext::convertHoldToTraces(int division, bool deleteOrigin) {
		// Prepare history
		Score prev = score;

		// beats-per-measure hardcoded to 4
		int interval = TICKS_PER_BEAT * 4 / division;

		// Here, `slide` refers to a normal hold note or a guide note
		for (int targetSlideId : selectedNotes) {
			if (!score.notes.count(targetSlideId)) continue;
			if (score.notes.at(targetSlideId).getType() != NoteType::Hold) continue;

			const HoldNote& target = score.holdNotes.at(targetSlideId);
			const Note& holdStart = score.notes.at(targetSlideId);
			int endTick = score.notes.at(target.end).tick;

			int connectorTailIndex = -1;
			EaseType connectorType(EaseType::Linear);
			const Note* connectorHead = &score.notes.at(targetSlideId);
			const Note* connectorTail = connectorHead;
			bool critical = connectorHead->critical ||
							(target.isGuide() && target.guideColor == GuideColor::Yellow);

			// Find the connector head and tail for each trace note
			for (int tick = connectorHead->tick; tick <= endTick; tick += interval) {
				// Do not create trace notes if they will overlap with the hold start or end
				if (!deleteOrigin) {
					if (tick == holdStart.tick && target.startType == HoldNoteType::Normal) continue;
					if (tick == endTick && target.endType == HoldNoteType::Normal) continue;
				}

				// Update connector endpoints if current time goes beyond them
				if (tick > connectorTail->tick || connectorTailIndex == -1) {
					// By default, the new connector head is the old connector tail
					connectorHead = connectorTail;
					connectorType = target[connectorTailIndex].ease;
					for (connectorTailIndex++; connectorTailIndex < target.steps.size(); connectorTailIndex++) {
						if (target[connectorTailIndex].type != HoldStepType::Skip) {
							const Note& potentialTail = score.notes.at(target.id_at(connectorTailIndex));
							// If the current tick is late enough, it is the new connector tail
							if (potentialTail.tick >= tick) break;
							// Otherwise, this is a connector head later than the previous one
							connectorHead = &potentialTail;
							connectorType = target[connectorTailIndex].ease;
						}
					}
					// Note that connectorTail might be the slide end
					connectorTail = &score.notes.at(target.id_at(connectorTailIndex));
				}

				// Calculate the trace's position and width
				float t = (float)(tick - connectorHead->tick) / (connectorTail->tick - connectorHead->tick);
				auto easeFunc = getEaseFunction(connectorType);
				float left = easeFunc(connectorHead->lane, connectorTail->lane, t);
				float right = easeFunc(connectorHead->lane+connectorHead->width, connectorTail->lane+connectorTail->width, t);
				// Spawn a trace note
				Note newNote(NoteType::Tap, tick, left, right - left);
				newNote.ID = Note::getNextID();
				newNote.critical = critical;
				newNote.friction = true;
				newNote.layer = score.notes.at(targetSlideId).layer;

				score.notes.emplace(newNote.ID, newNote);
				Note::getNextID();
			}

			// Delete origin slide
			if (deleteOrigin) {
				score.notes.erase(targetSlideId);
				score.notes.erase(target.end);
				for (const HoldStep& step : target.steps) score.notes.erase(step.ID);
				score.holdNotes.erase(targetSlideId);
			}
		}

		selectedNotes.clear();
		pushHistory("Convert slides into traces", prev, score);
	}

	void ScoreContext::lerpHiSpeeds(int division)
	{
		if (selectedHiSpeedChanges.size() < 2)
			return;

		Score prev = score;

		std::vector<int> sortedSelection;
		sortedSelection.insert(sortedSelection.end(), selectedHiSpeedChanges.begin(),
		                       selectedHiSpeedChanges.end());
		std::sort(sortedSelection.begin(), sortedSelection.end(), [this](int a, int b)
		          { return score.hiSpeedChanges[a].tick < score.hiSpeedChanges[b].tick; });

		for (int i = 0; i < sortedSelection.size() - 1; i++)
		{
			auto& first = score.hiSpeedChanges[sortedSelection[i]];
			auto& second = score.hiSpeedChanges[sortedSelection[i + 1]];

			int currentDivision = TICKS_PER_BEAT / (division / 4);

			int firstDivisionTick = first.tick - first.tick % currentDivision + currentDivision;
			for (int tick = firstDivisionTick; tick < second.tick; tick += currentDivision)
			{
				float t = ((float)tick - (float)first.tick) /
				          ((float)second.tick - (float)first.tick); // inverse lerp
				float speed =
				    (float)first.speed + t * ((float)second.speed - (float)first.speed); // lerp
				// remapping the current tick to the speed

				id_t id = getNextHiSpeedID();
				score.hiSpeedChanges[id] = { id, tick, speed, selectedLayer };
			}
		}

		pushHistory("Lerp hispeeds", prev, score);
	}

void ScoreContext::convertHoldToGuide(GuideColor color)
	{
		if (selectedNotes.empty())
			return;

		Score prev = score;
		bool edit = false;

		for (int id : selectedNotes)
		{
			if (!score.notes.count(id))
				continue;

			Note& note = score.notes.at(id);
			if (note.getType() == NoteType::Hold || note.getType() == NoteType::HoldEnd)
			{
				HoldNote& hold =
				    score.holdNotes.at(note.getType() == NoteType::Hold ? note.ID : note.parentID);
				if (!hold.isGuide())
				{
					hold.startType = HoldNoteType::Guide;
					hold.endType = HoldNoteType::Guide;
					hold.guideColor = color; 
					edit = true;
				}
			}
		}

		if (edit)
			pushHistory("Convert hold to guide", prev, score);
	}

	void ScoreContext::convertGuideToHold()
	{
		if (selectedNotes.empty())
			return;

		Score prev = score;
		bool edit = false;

		for (int id : selectedNotes)
		{
			if (!score.notes.count(id))
				continue;

			Note& note = score.notes.at(id);
			if (note.getType() == NoteType::Hold || note.getType() == NoteType::HoldEnd)
			{
				HoldNote& hold =
				    score.holdNotes.at(note.getType() == NoteType::Hold ? note.ID : note.parentID);
				if (hold.isGuide())
				{
					hold.startType = HoldNoteType::Normal;
					hold.endType = HoldNoteType::Normal;
					edit = true;
				}
			}
		}

		if (edit)
			pushHistory("Convert guide to hold", prev, score);
	}


	void ScoreContext::undo()
	{
		if (history.hasUndo())
		{
			score = history.undo();
			clearSelection();

			UI::setWindowTitle((workingData.filename.size()
			                        ? File::getFilename(workingData.filename)
			                        : windowUntitled) +
			                   "*");
			upToDate = false;

			scoreStats.calculateStats(score);
		}
	}

	void ScoreContext::redo()
	{
		if (history.hasRedo())
		{
			score = history.redo();
			clearSelection();

			UI::setWindowTitle((workingData.filename.size()
			                        ? File::getFilename(workingData.filename)
			                        : windowUntitled) +
			                   "*");
			upToDate = false;

			scoreStats.calculateStats(score);
		}
	}

	void ScoreContext::pushHistory(std::string description, const Score& prev, const Score& curr)
	{
		history.pushHistory(description, prev, curr);

		UI::setWindowTitle((workingData.filename.size() ? File::getFilename(workingData.filename)
		                                                : windowUntitled) +
		                   "*");
		scoreStats.calculateStats(score);

		upToDate = false;
	}

	bool ScoreContext::selectionHasEase() const
	{
		return std::any_of(selectedNotes.begin(), selectedNotes.end(),
		                   [this](const int id) { return score.notes.at(id).hasEase(); });
	}

	bool ScoreContext::selectionHasHold() const
	{
		return std::any_of(selectedNotes.begin(), selectedNotes.end(),
						   [this](int id) { return score.notes.at(id).getType() == NoteType::Hold; });
	}

	bool ScoreContext::selectionHasStep() const
	{
		return std::any_of(selectedNotes.begin(), selectedNotes.end(), [this](const int id)
		                   { return score.notes.at(id).getType() == NoteType::HoldMid; });
	}

	bool ScoreContext::selectionHasFlickable() const
	{
		return std::any_of(selectedNotes.begin(), selectedNotes.end(),
		                   [this](const int id) { return score.notes.at(id).canFlick(); });
	}

	bool ScoreContext::selectionCanConnect() const
	{
		if (selectedNotes.size() != 2)
			return false;

		const auto& note1 = score.notes.at(*selectedNotes.begin());
		const auto& note2 = score.notes.at(*std::next(selectedNotes.begin()));
		if (note1.tick == note2.tick)
			return (note1.getType() == NoteType::Hold && note2.getType() == NoteType::HoldEnd) ||
			       (note1.getType() == NoteType::HoldEnd && note2.getType() == NoteType::Hold);

		auto noteTickCompareFunc = [](const Note& n1, const Note& n2) { return n1.tick < n2.tick; };
		Note earlierNote = std::min(note1, note2, noteTickCompareFunc);
		Note laterNote = std::max(note1, note2, noteTickCompareFunc);

		return (earlierNote.getType() == NoteType::HoldEnd &&
		        laterNote.getType() == NoteType::Hold);
	}

	bool ScoreContext::selectionCanChangeHoldType() const
	{
		return std::any_of(
		    selectedNotes.begin(), selectedNotes.end(),
		    [this](const int id)
		    {
			    const Note& note = score.notes.at(id);
			    if (note.getType() == NoteType::Hold || note.getType() == NoteType::HoldEnd)
				    return !score.holdNotes
				                .at(note.getType() == NoteType::Hold ? note.ID : note.parentID)
				                .isGuide();

			    return false;
		    });
	}

	bool ScoreContext::selectionCanChangeFadeType() const
	{
		return std::any_of(
		    selectedNotes.begin(), selectedNotes.end(),
		    [this](const int id)
		    {
			    const Note& note = score.notes.at(id);
			    if (note.getType() == NoteType::Hold || note.getType() == NoteType::HoldEnd)
				    return score.holdNotes
				        .at(note.getType() == NoteType::Hold ? note.ID : note.parentID)
				        .isGuide();

			    return false;
		    });
	}
}
