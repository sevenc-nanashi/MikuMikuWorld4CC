#include "Application.h"
#include "IO.h"
#if !CHOC_EMSCRIPTEN
#include "nfd.hpp"
#include <whereami++.hpp>
#endif

namespace mmw = MikuMikuWorld;
mmw::Application app;

#if CHOC_WINDOWS
int main()
{
	int argc;
	LPWSTR* args;
	args = CommandLineToArgvW(GetCommandLineW(), &argc);
	if (!args)
	{
		IO::messageBox(APP_NAME, "CommandLineToArgvW failed...", IO::MessageBoxButtons::Ok,
		               IO::MessageBoxIcon::Error);
		return 1;
	}
#else
int main(int argc, char* argv[])
{
	std::wstring args[argc];
	for (int i = 0; i < argc; ++i)
		args[i] = IO::mbToWideStr(argv[i]);
#endif

#if !CHOC_EMSCRIPTEN
	NFD::Init();
#endif

#ifndef DEBUG
	try
	{
#endif
#if CHOC_EMSCRIPTEN
		std::string dir = "/app";
#else
		std::string dir = whereami::executable_dir();
		dir += "/";
#endif
		mmw::Result result = app.initialize(dir);

		if (!result.isOk())
			throw(std::runtime_error(result.getMessage().c_str()));

		for (int i = 1; i < argc; ++i)
			app.appendOpenFile(IO::wideStringToMb(args[i]));

		app.handlePendingOpenFiles();
		app.run();
#ifndef DEBUG
	}
	catch (const std::exception& ex)
	{
		std::string msg =
		    std::string(
		        "An unhandled exception has occurred and the application will now close.\n\n")
		        .append(ex.what())
		        .append("\n\nApplication Version: ")
		        .append(mmw::Application::getAppVersion());

		IO::messageBox(APP_NAME, msg, IO::MessageBoxButtons::Ok, IO::MessageBoxIcon::Error);
	}
#endif

	app.dispose();
#if !CHOC_EMSCRIPTEN
	NFD::Quit();
#endif
	return 0;
}

#if CHOC_WINDOWS
LRESULT CALLBACK wndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_TIMER:
		if (mmw::Application::windowState.windowDragging &&
		    wParam == mmw::Application::windowState.windowTimerId)
		{
			// grabbing the glfw window blocks the message queue causing the application to stop
			// rendering so we handle the message ourselves and update the UI explicitly
			if (app.getGlfwWindow())
				app.update();

			return 0;
		}
		break;

	case WM_ENTERSIZEMOVE:
		mmw::Application::windowState.windowDragging = true;
		break;

	case WM_EXITSIZEMOVE:
		mmw::Application::windowState.windowDragging = false;
		break;

	case WM_DROPFILES:
		if (HDROP dropHandle = reinterpret_cast<HDROP>(wParam); dropHandle != NULL)
		{
			const UINT filesCount = ::DragQueryFileW(dropHandle, 0xFFFFFFFF, NULL, 0u);
			for (UINT i = 0; i < filesCount; ++i)
			{
				const UINT bufferSize = ::DragQueryFileW(dropHandle, i, NULL, 0u);
				if (bufferSize > 0)
				{
					std::wstring wFilename(bufferSize + 1, 0);
					if (::DragQueryFileW(dropHandle, i, wFilename.data(),
					                     static_cast<UINT>(wFilename.size())) != 0)
						app.appendOpenFile(IO::wideStringToMb(wFilename.data()));
				}
			}

			::DragFinish(dropHandle);
		}
		break;

	default:
		// we don't handle this message ourselves so delegate it to the original glfw window's proc
		return CallWindowProcW((WNDPROC)glfwGetWindowUserPointer(app.getGlfwWindow()), hwnd, uMsg,
		                       wParam, lParam);
	}

	return ::DefWindowProcW(hwnd, uMsg, wParam, lParam);
}
#endif
