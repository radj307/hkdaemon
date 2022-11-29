#pragma once
#include <process.hpp>

#include <nlohmann/json.hpp>

#include <string>

namespace hkdaemon {
	struct action {
		std::string commandline;
		bool fwdSTDIO{ true };

		int ExecuteCommandline() const
		{
			return process::exec(commandline, process::Mode::READ | process::Mode::TEXT);
		}
		int ExecuteCommandline(std::stringstream* buffer) const
		{
			return process::exec(buffer, commandline, process::Mode::READ | process::Mode::TEXT);
		}
	};
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(action, commandline, fwdSTDIO);
}
