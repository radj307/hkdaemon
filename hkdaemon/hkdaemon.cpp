﻿#include "rc/version.h"
#include "rc/copyright.h"
#include "hotkey.hpp"
#include "VK_Extras.h"

#include <opt3.hpp>
#include <TermAPI.hpp>
#include <envpath.hpp>

inline std::pair<int, int> ParseWM_HotkeyLParam(LPARAM lParam)
{
	return{ (lParam >> 16) & 0xFFF, lParam & 0xFFF };
}

int main(const int argc, char** argv)
{
	try {
		opt3::ArgManager args{ argc, argv };
		env::PATH PATH{};
		const auto& [programDir, programName] { PATH.resolve_split(argv[0]) };

		if (args.check_any<opt3::Flag, opt3::Option>('h', "help")) {
			// TODO: help
			return 1;
		}
		else if (args.check_any<opt3::Flag, opt3::Option>('v', "version")) {
			std::cout << hkdaemon_VERSION_EXTENDED << std::endl;
			return 2;
		}

		const auto cfgPath{ programDir / std::filesystem::path{ programName }.replace_extension("json") };
		if (!file::exists(cfgPath))
			hkdaemon::config::WriteTo(cfgPath);
		auto cfg{ hkdaemon::config::ReadFrom(cfgPath) };

	#ifdef _DEBUG
		if (cfg.hotkeys.empty()) {
			// register a hotkey for debugging
			hkdaemon::hotkey hk{ "echo \"Hello World!\"", 1, hkdaemon::Modifiers::Ctrl | hkdaemon::Modifiers::Shift, VK_A };
			hk.Register();
			cfg.hotkeys.emplace_back(hk);
		}
	#endif

		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0)) {
			switch (msg.message) {
			case WM_HOTKEY: {
				const auto id{ msg.wParam };
				const auto& [key, mod] { ParseWM_HotkeyLParam(msg.lParam) };
				for (const auto& hk : cfg.hotkeys) {
					if (hk.id == id && hk.vk == key && hk.fsModifiers == mod) {
						int rc{ hk.ExecuteAction(std::clog) };
						break;
					}
				}
				break;
			}
			default:
				break;
			}
		}

		return 0;
	} catch (const std::exception& ex) {
		std::cerr << ex.what() << std::endl;
		return -1;
	} catch (...) {
		std::cerr << "An undefined exception occurred!" << std::endl;
		return -1;
	}
}