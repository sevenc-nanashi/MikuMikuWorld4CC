#pragma once

#include <cstdint>
#define IMGUI_DEFINE_MATH_OPERATORS
#define NOMINMAX

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3native.h>

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
