#include "src/utils/utils.hpp"

namespace po = boost::program_options;

void on_filename(std::string name){
    std::cout << "Got filename: " << name << std::endl;
}

ParsedRes::ParsedRes(int argc, char** argv)
{
    try{
        po::options_description desc{"Options"};
        desc.add_options()
        ("help,h", "Show this help message")
        ("sender,s", "Sender")
        ("receiver,r", "Receiver")
        ("queue,q", "Use a queue")
        //("pi", po::value<float>()->default_value(3.14f), "Pi")
        ("file,f", po::value< std::string >()->notifier(on_filename), "Filename")    
        //("email", po::value< std::vector<std::string> >()->composing(), "email")
        ;

        po::variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        if (vm.count("help")){
            std::cout << desc << '\n';
            valid = false;
            return;
        } else{
            valid = true;
            if (vm.count("queue")){
                method = Method::queue;
                std::cout << "Using queue\n";
            }

            if (vm.count("receiver")){
                role = Role::receiver;
            } else if (vm.count("sender")){
                role = Role::sender;
            }
            
            optargs = (OptArgs){};
            optargs.filename = vm["file"].as<std::string>();
        }
    }
    catch (const po::error &ex){
        std::cerr << ex.what() << '\n';
    }
}

char * NotImplemented::what () {
    return "Not implemented";
}