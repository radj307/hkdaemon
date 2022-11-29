#pragma once
#include "hotkey.hpp"

#include <fileio.hpp>

namespace hkdaemon {
	struct config {
		std::vector<hotkey> hotkeys;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(config, hotkeys);

		void InitializeHotkeys()
		{
			for (auto& it : hotkeys) {
				if (it.registered && !it.GetIsRegistered()) {
					it.Register();
				}
			}
		}

		static config ReadFrom(std::filesystem::path const& path)
		{
			nlohmann::json j;
			file::read(path) >> j;
			return j.get<config>();
		}
		static bool WriteTo(std::filesystem::path const& path, config const& cfg = {})
		{
			nlohmann::json j = cfg; //< this MUST be "= cfg" NOT "{cfg}"; failure to heed this warning will result in the JSON data implicitly being part of an array causing failures when reading it back!"
			return file::write(path, std::setw(2), j);
		}

		void Read(std::filesystem::path const& path)
		{
			*this = std::move(ReadFrom(path));
		}
		bool Write(std::filesystem::path const& path) const
		{
			return WriteTo(path, *this);
		}
	};
}
