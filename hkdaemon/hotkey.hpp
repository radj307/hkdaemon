#pragma once
#include "WindowsLite.h"
#include "action.hpp"

#include <sysarch.h>
#include <process.hpp>
#include <fileio.hpp>
#include <make_exception.hpp>

#include <nlohmann/json.hpp>

#include <vector>

namespace hkdaemon {
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

	inline std::string GetLastErrorMessage()
	{
		constexpr const unsigned BUFFER_SIZE{ 256u };
		char msg[BUFFER_SIZE];
		FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			0,
			errno,
			0,
			msg,
			BUFFER_SIZE,
			0
		);
		return{ msg };
	}

	static struct {
	private:
		mutable int32_t value{ 1 };

	public:
		CONSTEXPR int32_t next() const { return value++; }
	} hotkey_id_sequencer;

	struct hotkey {
		static HWND DefaultHWnd;

		int32_t id{ NULL };
		uint32_t modifiers{ NULL };
		uint32_t key{ NULL };
		action action;
		bool registered{ false };
	private:
		bool isRegistered{ false };
	public:

		CONSTEXPR hotkey() : id{ hotkey_id_sequencer.next() } {}
		CONSTEXPR hotkey(const uint32_t modifiers, const uint32_t key, const bool registered = false) : id{ hotkey_id_sequencer.next() }, modifiers{ modifiers }, key{ key }, registered{ registered }
		{
			if (this->registered) { Register(); }
		}
		CONSTEXPR hotkey(const Modifiers modifiers, const uint32_t key, const bool registered = false) : id{ hotkey_id_sequencer.next() }, modifiers{ static_cast<uint32_t>(modifiers) }, key{ key }, registered{ registered }
		{
			if (this->registered) { Register(); }
		}
		CONSTEXPR hotkey(std::string const& actionCommandline, const Modifiers modifiers, const uint32_t key, const bool registered = false) : id{ hotkey_id_sequencer.next() }, modifiers{ static_cast<uint32_t>(modifiers) }, key{ key }, registered{ registered }, action{ actionCommandline }
		{
			if (this->registered) { Register(); }
		}
		~hotkey() { UnRegister(); }

		Modifiers GetModifiers() const { return static_cast<Modifiers>(modifiers); }
		void SetModifiers(const Modifiers modifiers) { this->modifiers = static_cast<uint32_t>(modifiers); }
		void AddModifiers(const Modifiers modifiers) { this->modifiers = static_cast<uint32_t>(static_cast<uint16_t>(modifiers) | static_cast<uint16_t>(modifiers)); }
		void RemoveModifiers(const Modifiers modifiers) { this->modifiers = static_cast<uint32_t>(static_cast<uint16_t>(modifiers) & ~static_cast<uint16_t>(modifiers)); }

		bool GetIsRegistered() const { return isRegistered; }

		void Register(HWND hWnd = DefaultHWnd)
		{
			if (isRegistered) return;

			if (registered = RegisterHotKey(hWnd, id, modifiers, key)) {
				isRegistered = true;
			}
			else throw make_exception(GetLastErrorMessage());
		}
		void UnRegister(HWND hWnd = DefaultHWnd)
		{
			if (!isRegistered) return;

			if (registered = UnregisterHotKey(hWnd, id)) {
				isRegistered = false;
			}
			else throw make_exception(GetLastErrorMessage());
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
			if (action.fwdSTDIO) {
				std::stringstream ss;
				ss.set_rdbuf(os.rdbuf());
				return action.ExecuteCommandline(&ss);
			}
			return action.ExecuteCommandline(); //< don't forward commandline output
		}

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(hotkey, modifiers, key, registered, action);

	};
	inline HWND hotkey::DefaultHWnd{ NULL };
}
