#pragma once
#include "UI.h"
#include "json.hpp"
#include "Math.h"
#include "InputBinding.h"

namespace MikuMikuWorld
{
	struct InputConfiguration
	{
		MultiInputBinding togglePlayback = { "toggle_playback",
			                                 { ImGuiKey_Space, ImGuiMod_None } };
		MultiInputBinding stop = { "stop", { ImGuiKey_Backspace } };
		MultiInputBinding decreaseNoteSize = { "decrease_note_size", {} };
		MultiInputBinding increaseNoteSize = { "increase_note_size", {} };
		MultiInputBinding shrinkDown = { "shrink_down", {} };
		MultiInputBinding shrinkUp = { "shrink_up", {} };
		MultiInputBinding compressSelection = { "compress_selection", {} };
		MultiInputBinding connectHolds = { "connect_holds", {} };
		MultiInputBinding splitHold = { "split_hold", {} };
		MultiInputBinding lerpHiSpeeds = { "lerp_hispeeds", {} };
		MultiInputBinding openHelp = { "help", { ImGuiKey_F1 } };
		MultiInputBinding openSettings = { "settings", { ImGuiKey_Comma, ImGuiMod_Ctrl } };
		MultiInputBinding deleteSelection = { "delete", { ImGuiKey_Delete } };
		MultiInputBinding copySelection = { "copy", { ImGuiKey_C, ImGuiMod_Ctrl } };
		MultiInputBinding cutSelection = { "cut", { ImGuiKey_X, ImGuiMod_Ctrl } };
		MultiInputBinding paste = { "paste", { ImGuiKey_V, ImGuiMod_Ctrl } };
		MultiInputBinding flipPaste = { "flip_paste",
			                            { ImGuiKey_V, ImGuiMod_Ctrl | ImGuiMod_Shift } };
		MultiInputBinding duplicate = { "duplicate", { ImGuiKey_D, ImGuiMod_Ctrl } };
		MultiInputBinding flipDuplicate = {
			"flip_duplicate", { ImGuiKey_D, ImGuiMod_Ctrl | ImGuiMod_Shift }
		};
		MultiInputBinding flip = { "flip", { ImGuiKey_F, ImGuiMod_Ctrl } };
		MultiInputBinding cancelPaste = { "cancel_paste", { ImGuiKey_Escape } };
		MultiInputBinding previousTick = { "previous_tick", { ImGuiKey_DownArrow } };
		MultiInputBinding nextTick = { "next_tick", { ImGuiKey_UpArrow } };
		MultiInputBinding create = { "new_chart", { ImGuiKey_N, ImGuiMod_Ctrl } };
		MultiInputBinding open = { "open", { ImGuiKey_O, ImGuiMod_Ctrl } };
		MultiInputBinding save = { "save", { ImGuiKey_S, ImGuiMod_Ctrl } };
		MultiInputBinding saveAs = { "save_as",
			                         { ImGuiKey_S, ImGuiMod_Ctrl | ImGuiMod_Shift } };
		MultiInputBinding exportSus = { "export_sus",
			                            { ImGuiKey_E, ImGuiMod_Shift | ImGuiMod_Ctrl } };
		MultiInputBinding exportUsc = { "export_usc", { ImGuiKey_E, ImGuiMod_Ctrl } };
		MultiInputBinding selectAll = { "select_all", { ImGuiKey_A, ImGuiMod_Ctrl } };
		MultiInputBinding undo = { "undo", { ImGuiKey_Z, ImGuiMod_Ctrl } };
		MultiInputBinding redo = { "redo", { ImGuiKey_Y, ImGuiMod_Ctrl } };
		MultiInputBinding zoomOut = { "zoom_out", {} };
		MultiInputBinding zoomIn = { "zoom_in", {} };

		MultiInputBinding timelineSelect = { "timeline_select",
			                                 { ImGuiKey_1 },
			                                 { ImGuiKey_Keypad1 } };
		MultiInputBinding timelineTap = { "timeline_tap", { ImGuiKey_2 }, { ImGuiKey_Keypad2 } };
		MultiInputBinding timelineHold = { "timeline_hold", { ImGuiKey_3 }, { ImGuiKey_Keypad3 } };
		MultiInputBinding timelineHoldMid = { "timeline_hold_step",
			                                  { ImGuiKey_4 },
			                                  { ImGuiKey_Keypad4 } };
		MultiInputBinding timelineFlick = { "timeline_flick",
			                                { ImGuiKey_5 },
			                                { ImGuiKey_Keypad5 } };
		MultiInputBinding timelineCritical = { "timeline_critical",
			                                   { ImGuiKey_6 },
			                                   { ImGuiKey_Keypad6 } };
		MultiInputBinding timelineFriction = { "timeline_trace",
			                                   { ImGuiKey_7 },
			                                   { ImGuiKey_Keypad7 } };
		MultiInputBinding timelineGuide = { "timeline_guide", {} };
		MultiInputBinding timelineDamage = { "timeline_damage", {} };
		MultiInputBinding timelineBpm = { "timeline_bpm", { ImGuiKey_8 }, { ImGuiKey_Keypad8 } };
		MultiInputBinding timelineTimeSignature = { "timeline_time_signature",
			                                        { ImGuiKey_9 },
			                                        { ImGuiKey_Keypad9 } };
		MultiInputBinding timelineHiSpeed = { "timeline_hi_speed",
			                                  { ImGuiKey_0 },
			                                  { ImGuiKey_Keypad0 } };
	};

	constexpr size_t maxRecentFilesEntries = 10;

	struct ApplicationConfiguration
	{
		std::string version;

		// session state
		Vector2 windowPos;
		Vector2 windowSize;
		bool maximized;
		bool vsync;
		bool showFPS;
		int accentColor;
		Color userColor;
		BaseTheme baseTheme;
		std::string language;
		int division;
		float zoom;

		// settings
		bool minifyUsc;
		bool showSusExport;
		int timelineWidth;
		int notesHeight;
		bool matchNotesSizeToTimeline;
		float laneOpacity;
		float backgroundBrightness;
		bool drawBackground;
		std::string backgroundImage;
		bool useSmoothScrolling;
		float smoothScrollingTime;
		float cursorPositionThreshold;
		float scrollSpeedNormal;
		float scrollSpeedShift;
		bool returnToLastSelectedTickOnPause;
		bool followCursorInPlayback;
		bool drawWaveform;
		bool showTickInProperties;
		bool autoSaveEnabled;
		int autoSaveInterval;
		int autoSaveMaxCount;
		float masterVolume;
		float bgmVolume;
		float seVolume;
		int seProfileIndex;
		bool debugEnabled;

		InputConfiguration input;

		std::vector<std::string> recentFiles;

		ApplicationConfiguration();

		void read(const std::string& filename);
		void write(const std::string& filename);
		void restoreDefault();
	};

	extern ApplicationConfiguration config;

	static MultiInputBinding* bindings[] = {
		&config.input.create,
		&config.input.open,
		&config.input.save,
		&config.input.saveAs,
		&config.input.exportSus,
		&config.input.exportUsc,
		&config.input.undo,
		&config.input.redo,

		&config.input.cutSelection,
		&config.input.copySelection,
		&config.input.paste,
		&config.input.flipPaste,
		&config.input.duplicate,
		&config.input.flipDuplicate,
		&config.input.flip,
		&config.input.cancelPaste,
		&config.input.deleteSelection,
		&config.input.shrinkDown,
		&config.input.shrinkUp,
		&config.input.compressSelection,
		&config.input.connectHolds,
		&config.input.splitHold,
		&config.input.lerpHiSpeeds,
		&config.input.selectAll,

		&config.input.togglePlayback,
		&config.input.stop,
		&config.input.previousTick,
		&config.input.nextTick,
		&config.input.decreaseNoteSize,
		&config.input.increaseNoteSize,
		&config.input.zoomOut,
		&config.input.zoomIn,

		&config.input.timelineSelect,
		&config.input.timelineTap,
		&config.input.timelineHold,
		&config.input.timelineHoldMid,
		&config.input.timelineFlick,
		&config.input.timelineCritical,
		&config.input.timelineFriction,
		&config.input.timelineGuide,
		&config.input.timelineDamage,
		&config.input.timelineBpm,
		&config.input.timelineTimeSignature,
		&config.input.timelineHiSpeed,

		&config.input.openHelp,
		&config.input.openSettings,
	};

	static_assert(sizeof(InputConfiguration) / sizeof(MultiInputBinding) ==
	                  (sizeof(bindings) / sizeof(MultiInputBinding*)),
	              "Too many or too few items in MikuMikuWorld::bindings");
}
