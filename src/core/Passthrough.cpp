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
#include <openssl/evp.h>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <csignal>
#include <iostream>
#include <unistd.h>

Passthrough::Passthrough() {
	lockFilePath = generateUniqueLockFileName();
}

std::string Passthrough::generateUniqueLockFileName() {
	char path[1024];

	ssize_t count = readlink("/proc/self/exe", path, sizeof(path) - 1);
	if (count != -1) {
		path[count] = '\0';
	}
	else {
		std::strncpy(path, "default_program_path", sizeof(path) - 1);
	}

	unsigned char hash[EVP_MAX_MD_SIZE];
	unsigned int hash_len;

	EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
	if (mdctx == nullptr) {
		throw std::runtime_error("Failed to create EVP_MD_CTX");
	}

	if (EVP_DigestInit_ex(mdctx, EVP_sha256(), nullptr) != 1 ||
		EVP_DigestUpdate(mdctx, path, std::strlen(path)) != 1 ||
		EVP_DigestFinal_ex(mdctx, hash, &hash_len) != 1) {
		EVP_MD_CTX_free(mdctx);
		throw std::runtime_error("Failed to compute SHA-256 hash");
	}

	EVP_MD_CTX_free(mdctx);

	std::stringstream ss;
	for (unsigned int i = 0; i < hash_len; ++i) {
		ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
	}

	std::string lockFileName = "/tmp/" + ss.str() + ".lock";
	return lockFileName;
}


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
int Passthrough::handlePassthrough(const ProgramArgs& args) {
	std::string command;
	for (int i = 1; i < args.argc; ++i) {
		command += args.argv[i];
		if (i != args.argc - 1)
			command += " ";
	}

	return std::system(("/bin/sh -c \"" + command + "\"").c_str());
}
#   	endif
#   endif

int Passthrough::createProcess(const ProgramArgs& args, xmrig::Process* process) {
#   ifndef XMRIG_OS_WIN
#       ifdef XMRIG_SH_PASSTHROUGH

	char* process_argv[] = { args.argv[0], nullptr };
	*process = xmrig::Process(1, process_argv);

	return this->handlePassthrough(args);
#   endif
#       else
	*process = xmrig::Process(args.argc, args.argv);
#   endif

	return EXIT_SUCCESS;
}