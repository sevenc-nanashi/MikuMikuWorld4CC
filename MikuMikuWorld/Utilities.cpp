#include "Utilities.h"
#include "Constants.h"
#include "ImGui/imgui.h"
#include "Localization.h"
#include "IO.h"
#include <ctime>
#include <vector>
#include <choc/platform/choc_Platform.h>
#include <boost/locale.hpp>

namespace MikuMikuWorld
{
	std::string Utilities::getCurrentDateTime()
	{
		std::time_t now = std::time(0);
		std::tm localTime = *std::localtime(&now);

		char buf[128];
		strftime(buf, 128, "%Y-%m-%d-%H-%M-%S", &localTime);

		return buf;
	}

	std::string Utilities::getSystemLocale()
	{
		std::locale locale = boost::locale::generator().generate("");
		return locale.name();
	}

	std::vector<std::string> Utilities::splitString(const std::string& base, const char delimiter)
	{
		std::vector<std::string> result;
		std::string buffer;

		for (const auto& character : base)
		{
			if (character == delimiter)
			{
				result.push_back(buffer);
				buffer.clear();
			}
			else
			{
				buffer += character;
			}
		}

		result.push_back(buffer);

		return result;
	}

	std::string Utilities::getDivisionString(int div)
	{
		return IO::formatString(getString("division"), div);
	}

	float Utilities::centerImGuiItem(const float width)
	{
		return (ImGui::GetContentRegionAvail().x * 0.5f) - (width * 0.5f);
	}

	void Utilities::ImGuiCenteredText(const std::string& str)
	{
		Utilities::ImGuiCenteredText(str.c_str());
	}

	void Utilities::ImGuiCenteredText(const char* str)
	{
		ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize(str).x) *
		                     0.5f);
		ImGui::Text(str);
	}

	void drawShadedText(ImDrawList* drawList, ImVec2 textPos, float fontSize, ImU32 fontColor,
	                    const char* text)
	{
		if (!drawList)
			return;

		drawList->AddText(ImGui::GetFont(), fontSize, textPos + ImVec2{ 0.75f, 1.0f }, 0xff111111,
		                  text);
		drawList->AddText(ImGui::GetFont(), fontSize, textPos, fontColor, text);
	}
}
