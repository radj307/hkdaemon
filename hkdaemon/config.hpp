#pragma once
#include "hotkey.hpp"

namespace hkdaemon {
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
