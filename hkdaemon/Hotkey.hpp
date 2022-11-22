#pragma once
#include "WindowsLite.h"

#include <sysarch.h>
#include <process.hpp>
#include <fileio.hpp>

#include <nlohmann/json.hpp>

#include <vector>

namespace hkdaemon {
	struct action {
		std::string commandline;
		bool forwardOutput{ true };

		int ExecuteCommandline() const
		{
			return process::exec(commandline, process::Mode::READ | process::Mode::TEXT);
		}
		int ExecuteCommandline(std::stringstream* buffer) const
		{
			return process::exec(buffer, commandline, process::Mode::READ | process::Mode::TEXT);
		}
	};
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(action, commandline, forwardOutput);

	enum class Modifiers : uint16_t {
		None = 0,
		Alt = MOD_ALT,
		Ctrl = MOD_CONTROL,
		Shift = MOD_SHIFT,
		Win = MOD_WIN,
		NoRepeat = MOD_NOREPEAT,
		Left = MOD_LEFT,
		Right = MOD_RIGHT,
	};
	$make_bitfield_operators(Modifiers, uint16_t);

	struct hotkey {
		static HWND DefaultHWnd;

		int32_t id{ NULL };
		uint32_t fsModifiers{ NULL };
		uint32_t vk{ NULL };
		action action;
	private:
		bool registered{ false };
		bool isRegistered{ false };
	public:

		CONSTEXPR hotkey()
		{
			if (this->registered) ReRegister();
		}
		CONSTEXPR hotkey(const int32_t id, const uint32_t fsModifiers, const uint32_t vk, const bool registered = false) : id{ id }, fsModifiers{ fsModifiers }, vk{ vk }, registered{ registered }
		{
			if (this->registered) ReRegister();
		}
		CONSTEXPR hotkey(const int32_t id, const Modifiers modifiers, const uint32_t vk, const bool registered = false) : id{ id }, fsModifiers{ static_cast<uint32_t>(modifiers) }, vk{ vk }, registered{ registered }
		{
			if (this->registered) ReRegister();
		}
		CONSTEXPR hotkey(std::string const& actionCommandline, const int32_t id, const Modifiers modifiers, const uint32_t vk, const bool registered = false) : id{ id }, fsModifiers{ static_cast<uint32_t>(modifiers) }, vk{ vk }, registered{ registered }, action{ actionCommandline }
		{
			if (this->registered) ReRegister();
		}
		~hotkey() { UnRegister(); }

		Modifiers GetModifiers() const { return static_cast<Modifiers>(fsModifiers); }
		void SetModifiers(const Modifiers modifiers) { fsModifiers = static_cast<uint32_t>(modifiers); }
		void AddModifiers(const Modifiers modifiers) { fsModifiers = static_cast<uint32_t>(static_cast<uint16_t>(fsModifiers) | static_cast<uint16_t>(modifiers)); }
		void RemoveModifiers(const Modifiers modifiers) { fsModifiers = static_cast<uint32_t>(static_cast<uint16_t>(fsModifiers) & ~static_cast<uint16_t>(modifiers)); }

		void Register(HWND hWnd = DefaultHWnd)
		{
			if (isRegistered) return;

			if (registered = RegisterHotKey(hWnd, id, fsModifiers, vk)) {
				isRegistered = true;
			} // TODO: error handling
		}
		void UnRegister(HWND hWnd = DefaultHWnd)
		{
			if (!isRegistered) return;

			if (registered = UnregisterHotKey(hWnd, id)) {
				isRegistered = false;
			} // TODO: error handling
		}
		void ReRegister(HWND hWnd = DefaultHWnd)
		{
			if (isRegistered)
				UnRegister(hWnd);
			Register(hWnd);
		}

		int ExecuteAction() const
		{
			return action.ExecuteCommandline();
		}
		int ExecuteAction(std::ostream& os) const
		{
			if (action.forwardOutput) {
				std::stringstream ss;
				ss.set_rdbuf(os.rdbuf());
				return action.ExecuteCommandline(&ss);
			}
			return action.ExecuteCommandline(); //< don't forward commandline output
		}

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(hotkey, id, fsModifiers, vk, registered, action);
	};
	inline HWND hotkey::DefaultHWnd{ NULL };

	struct config {
		std::vector<hotkey> hotkeys;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(config, hotkeys);

		static config ReadFrom(std::filesystem::path const& path)
		{
			nlohmann::json j;
			file::read(path) >> j;
			return j.get<config>();
		}
		static bool WriteTo(std::filesystem::path const& path, config const& cfg = {})
		{
			return file::write(path, nlohmann::json{ cfg }.dump());
		}
	};
}
