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

#ifndef XMRIG_PASSTHROUGH_H
#define XMRIG_PASSTHROUGH_H

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

class Passthrough {
public:
	Passthrough();

	bool isAnotherInstanceRunning();
	void createLockFile();
	void removeLockFile();
	int createProcess(int argc, char** argv, xmrig::Process* process);
	int handlePassthrough(int argc, char** argv);

private:
	std::string lockFilePath;
	
	std::string generateUniqueLockFileName();
};

#endif /* XMRIG_PASSTHROUGH_H */