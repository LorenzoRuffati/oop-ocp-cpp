#include "src/utils/utils.hpp"

namespace po = boost::program_options;

void on_filename(std::string name){
}

void validate_pass(std::string passwd){
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
        ("memory,m", "Use shared memory")
        ("file,f", po::value< std::string >()->notifier(on_filename), "Filename")    
        ("buffer,w", po::value<size_t>()->default_value(1024), "The size of the buffer to use, currently only meaningful for shared memory")
        ("readers,n", po::value<int>()->default_value(1),
                     "Use shared memory, the optional argument to this flag"
                     " represents the number of readers and is only meaningful"
                     " when defining the sender")
        ("pass,p", po::value<std::string>()->notifier(validate_pass), "A string (which is a valid file basename) used for coordination between sender and receiver")
        ;

        po::variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        if (vm.count("help")){
            std::cout << desc << '\n';
            valid = false;
            return;
        } else{
            method = Method::undefined;
            valid = true;
            if (vm.count("queue")){
                method = Method::queue;
                std::cout << "Using queue\n";
            }
            if (vm.count("memory")){
                if (method != Method::undefined){valid = false;}
                method = Method::shm;
                std::cout << "Using shared memory\n";
            }

            if (vm.count("receiver")){
                role = Role::receiver;
            } else if (vm.count("sender")){
                role = Role::sender;
            }

            optargs = (OptArgs){};
            if ((vm.count("pass")== 0) || (vm.count("file") == 0)){
                valid = false;
            } else {
                optargs.filename = vm["file"].as<std::string>();
                optargs.passwd = vm["pass"].as<std::string>();
            }
            optargs.readers = vm["readers"].as<int>();
            optargs.width = vm["buffer"].as<size_t>();
        }
    }
    catch (const po::error &ex){
        std::cerr << ex.what() << '\n';
    }
    catch (const OwnError &oer){
        valid = false;
    }
}

char * NotImplemented::what () {
    return "Not implemented";
}