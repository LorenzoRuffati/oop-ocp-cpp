#include <iostream>
#include <memory>
#include "help.hpp"
#include "src/ipc/ipc.hpp"
#include "src/actors/actor.hpp"
#include "src/utils/types.hpp"
#include "src/utils/utils.hpp"

int main(int argc, char** argv) {
    std::cout << "Hello World! " << mult() << std::endl;
    ParsedRes parsed = ParsedRes(argc, argv);
    std::cout << parsed.optargs.filename << std::endl;
    if (!parsed.valid){
        return -1;
    }

    std::unique_ptr<Actor> actr = ActorFactory::create(parsed.role, parsed.optargs);
    std::cout << "Created actor" << std::endl;

    std::unique_ptr<IPC> ipc = IPCFactory::get_ipc(parsed.method, parsed.role, parsed.optargs);
    std::cout << "Running" << std::endl;

    int ret = actr->execute(*ipc);
    return ret;
}