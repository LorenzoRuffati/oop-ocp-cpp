#include <iostream>
#include <memory>
#include "help.h"
#include "src/ipc/ipc.h"
#include "src/actors/actor.h"
#include "src/utils/types.h"
#include "src/utils/utils.h"

int main(int argc, char** argv) {
    std::cout << "Hello World! " << mult() << std::endl;
    ParsedRes parsed = ParsedRes(argc, argv);

    std::unique_ptr<Actor> actr = ActorFactory::create(parsed.role, parsed.optargs);
    std::unique_ptr<IPC> ipc = IPCFactory::get_ipc(parsed.method, parsed.role, parsed.optargs);
    std::cout << "Running" << std::endl;

    int ret = actr->execute(*ipc);
    return ret;
}