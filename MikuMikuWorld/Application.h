#pragma once

#include <cstdint>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <choc/platform/choc_Platform.h>

#if !CHOC_EMSCRIPTEN
#if CHOC_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif CHOC_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#elif CHOC_LINUX
#define GLFW_EXPOSE_NATIVE_X11
#else
#error "Unsupported platform"
#endif

#include <GLFW/glfw3native.h>

#if CHOC_LINUX
#undef Success // Avoid conflict with X11
#undef None
#endif
#endif

#include "ScoreEditor.h"
#include "ImGuiManager.h"
#ifdef _WIN32
#include <Windows.h>

LRESULT CALLBACK wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

namespace MikuMikuWorld
{
	class Result;

	struct WindowState
	{
		bool resetting = false;
		bool vsync = true;
		bool showPerformanceMetrics = false;
		bool maximized = false;
		bool closing = false;
		bool shouldPickScore = false;
		bool dragDropHandled = true;
		bool windowDragging = false;
		float lastDpiScale = 0.0f;
		void* windowHandle;
		Vector2 position{};
		Vector2 size{};
		uintptr_t windowTimerId{};
	};

	class Application
	{
	  private:
		GLFWwindow* window;
		std::unique_ptr<ScoreEditor> editor;
		std::unique_ptr<ImGuiManager> imgui;
		UnsavedChangesDialog unsavedChangesDialog;

		bool initialized;
		bool shouldPickScore;
		std::string language;

		std::vector<std::string> pendingOpenFiles;

		static std::string version;
		static std::string appDir;

		Result initOpenGL();
		std::string getVersion();

	  public:
		static WindowState windowState;
		static std::string pendingLoadScoreFile;

		Application();

		Result initialize(const std::string& root);
		void run();
		void update();
		void appendOpenFile(const std::string& filename);
		void handlePendingOpenFiles();
		void readSettings();
		void writeSettings();
		void loadResources();
		void dispose();

		GLFWwindow* getGlfwWindow() { return window; }

		static const std::string& getAppDir();
		static const std::string& getAppVersion();
	};
}
