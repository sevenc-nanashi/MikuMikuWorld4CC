#pragma once
#include <stdint.h>
#include <array>
#include <string>
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "Utilities.h"

struct InputBinding
{
	ImGuiKey keyCode{};
	ImGuiKeyChord keyModifiers{};

	InputBinding(ImGuiKey key = ImGuiKey_None, ImGuiKeyChord mods = ImGuiMod_None)
	    : keyCode{ key }, keyModifiers{ mods }
	{
	}

	constexpr bool operator==(const InputBinding& other) const
	{
		return keyModifiers == other.keyModifiers && keyCode == other.keyCode;
	}

	constexpr bool operator!=(const InputBinding& other) const { return !(*this == other); }
};

DECLARE_ENUM_FLAG_OPERATORS(ImGuiKey);

struct MultiInputBinding
{
	int count = 0;
	const char* name;
	std::array<InputBinding, 4> bindings;

	MultiInputBinding(const char* name) { this->name = name; }

	MultiInputBinding(const char* name, InputBinding binding)
	{
		this->name = name;
		if (binding.keyCode != 0)
			bindings[count++] = binding;
	}

	MultiInputBinding(const char* name, InputBinding b1, InputBinding b2)
	{
		this->name = name;
		if (b1.keyCode != 0)
			bindings[count++] = b1;

		if (b2.keyCode != 0)
			bindings[count++] = b2;
	}

	void moveUp(int index)
	{
		if (index < 1)
			return;

		std::swap(bindings[index - 1], bindings[index]);
	}

	void moveDown(int index)
	{
		if (index > 2 || count <= index + 1)
			return;

		std::swap(bindings[index + 1], bindings[index]);
	}

	void addBinding(InputBinding binding)
	{
		if (count == bindings.size())
			return;

		bindings[count++] = binding;
	}

	void removeAt(int index)
	{
		if (index < 0 || index >= bindings.size())
			return;

		// shift elements to the left
		for (int i = index; i < count - 1; ++i)
			bindings[i] = bindings[i + 1];

		--count;
	}
};

const char* ToShortcutString(const MultiInputBinding& binding);
const char* ToShortcutString(const InputBinding& binding);
const char* ToShortcutString(ImGuiKey key, ImGuiKey mods);
const char* ToShortcutString(ImGuiKey key);

std::string ToFullShortcutsString(const MultiInputBinding& binding);
std::string ToSerializedString(const InputBinding& binding);
InputBinding FromSerializedString(std::string string);

namespace ImGui
{
	bool TestModifiers(ImGuiKey mods);

	bool IsDown(const InputBinding& binding);
	bool IsPressed(const InputBinding& binding, bool repeat = false);

	bool IsAnyDown(const MultiInputBinding& binding);
	bool IsAnyPressed(const MultiInputBinding& binding, bool repeat = false);
}
