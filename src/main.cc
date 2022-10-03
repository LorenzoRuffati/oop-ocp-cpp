#include <iostream>
#include "help.h"
#include "src/ipc/ipc.h"
#include "src/actors/actor.h"
#include "src/utils/types.h"
#include "src/utils/utils.h"

int main(int argc, char** argv) {
    std::cout << "Hello World! " << mult();
    ParsedRes parsed = ParsedRes(argc, argv);

    Actor actr = ActorFactory::create(parsed.role, parsed.optargs);
    IPC ipc = IPCFactory::get_ipc(parsed.method, parsed.role, parsed.optargs);

    return actr.execute(ipc);
}