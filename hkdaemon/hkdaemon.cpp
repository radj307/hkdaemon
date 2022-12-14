#include "rc/version.h"
#include "rc/copyright.h"
#include "config.hpp"
#include "VK_Extras.h"

#include <opt3.hpp>
#include <TermAPI.hpp>
#include <envpath.hpp>

#include <shellapi.h>
#include <future>

/**
 * @brief			Separates the low and high order bytes from the given `LPARAM`.
 * @param lParam	The lParam member of the WM_HOTKEY message type.
 * @returns			A pair with the key and modifier values, respectively.
 */
inline std::pair<int, int> ParseWM_HotkeyLParam(LPARAM lParam)
{
	return{ static_cast<int>((lParam >> 16) & 0xFFF), static_cast<int>(lParam & 0xFFF) };
}

struct print_help {
	std::string programName;

	STRCONSTEXPR print_help(std::string const& programName) : programName{ programName } {}

	friend std::ostream& operator<<(std::ostream& os, const print_help& h)
	{
		return os
			<< "hkdaemon v" << hkdaemon_VERSION_EXTENDED << '\n'
			<< "  Windows Hotkey Daemon" << '\n'
			<< '\n'
			<< "USAGE:\n"
			<< h.programName << " [OPTIONS]" << '\n'
			<< '\n'
			<< "OPTIONS:\n"
			<< "  -h, --help            Prints this help display, then exits." << '\n'
			<< "  -v, --version         Prints the current version number, then exits." << '\n'
			<< "  -c, --config <PATH>   Specifies the location of the hotkey configuration file." << '\n'
			<< "  -n, --new             If the specified hotkey config doesn't exist at the specified location, a new one is created." << '\n'
			<< "  -H, --hk <JSON>       Imports hotkeys directly from provided JSON data." << '\n'
			<< "      --hk-example      Prints a blank hotkey template to STDOUT for use with the above option, then exits." << '\n'
			<< "      --dump            Dumps the finalized hotkey config to STDOUT, then exits." << '\n'
			;
	}
};

int main(const int argc, char** argv)
{
	try {
		opt3::ArgManager args{ argc, argv,
			opt3::make_template(opt3::ConflictStyle::Conflict, opt3::CaptureStyle::Required, 'c', "config"),
			opt3::make_template(opt3::ConflictStyle::None, opt3::CaptureStyle::Required, 'H', "hk")
		};
		env::PATH PATH{};
		const auto& [programDir, programName] { PATH.resolve_split(argv[0]) };

		if (args.check_any<opt3::Flag, opt3::Option>('h', "help")) {
			std::cout << print_help(programName.generic_string());
			return 1;
		}
		else if (args.check_any<opt3::Flag, opt3::Option>('v', "version")) {
			std::cout << hkdaemon_VERSION_EXTENDED << std::endl;
			return 2;
		}
		else if (args.checkopt("hk-example")) {
			nlohmann::json j = hkdaemon::hotkey{};
			std::cout << j << std::endl;
			return 3;
		}

		const auto cfgPath{ args.castgetv_any<std::filesystem::path, opt3::Flag, opt3::Option>('c', "config").value_or(programDir / std::filesystem::path{ programName }.replace_extension("json")) };

		if (args.check_any<opt3::Flag, opt3::Option>('n', "new")) {
			hkdaemon::config::WriteTo(cfgPath);
			std::cout << "Successfully created a new hotkey config at " << cfgPath << std::endl;
			return 3;
		}

		hkdaemon::config cfg;

		if (file::exists(cfgPath))
			cfg = hkdaemon::config::ReadFrom(cfgPath);

		if (const auto& importHotkeys{ args.getv_all<opt3::Flag, opt3::Option>('H', "hk") }; !importHotkeys.empty()) {
			bool atLeastOne{ false };
			cfg.hotkeys.reserve(cfg.hotkeys.size() + importHotkeys.size());
			for (auto& hotkeyJSON : importHotkeys) {
				try {
					cfg.hotkeys.emplace_back(nlohmann::json{ hotkeyJSON }.get<hkdaemon::hotkey>());
					atLeastOne = true;
				} catch (const std::exception& ex) {
					std::cerr << "The hotkey specified by \"" << hotkeyJSON << "\" was skipped because it caused an exception: " << ex.what() << std::endl;
				}
			}
			cfg.hotkeys.shrink_to_fit(); //< unallocate elements that had invalid JSON data
			if (atLeastOne) {
				if (hkdaemon::config::WriteTo(cfgPath, cfg)) {
					std::cout << "Successfully saved imported hotkeys to " << cfgPath << std::endl;
				}
				else throw make_exception("An error occurred while attempting to write to ", cfgPath);
			}
		}
		if (args.checkopt("dump")) {
			nlohmann::json j = cfg;
			std::cout << j << std::endl;
			return 4;
		}

		cfg.InitializeHotkeys();

	#ifdef _DEBUG
		// register a hotkey for debugging
		if (cfg.hotkeys.empty()) {
			hkdaemon::hotkey hk{ "echo \"" + color::setcolor::red.as_sequence() + "Hello World!" + color::setcolor::reset.as_sequence() + "\"", hkdaemon::Modifiers::Ctrl | hkdaemon::Modifiers::Shift, VK_A };
			hk.Register();
			cfg.hotkeys.emplace_back(hk);
			hkdaemon::config::WriteTo(cfgPath, cfg); //< write the debug config to disk
		}
	#endif

		// Windows Message Loop:
		for (MSG msg; GetMessage(&msg, NULL, 0, 0); ) {
			switch (msg.message) {
			case WM_HOTKEY: {
				const auto id{ msg.wParam };
				for (const auto& hk : cfg.hotkeys) {
					if (hk.id == id) {
						auto t{ std::async(std::launch::async, [&hk]() { return hk.ExecuteAction(std::clog); }) };


						break;
					}
				}
				break;
			}
			default: break;
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
