#include "src/utils/utils.hpp"

namespace po = boost::program_options;

void on_filename(std::string name){
    std::cout << "Got filename: " << name << std::endl;
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
        //("pi", po::value<float>()->default_value(3.14f), "Pi")
        ("file,f", po::value< std::string >()->notifier(on_filename), "Filename")    
        //("email", po::value< std::vector<std::string> >()->composing(), "email")
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
            if ((vm.count("pass")== 0) || (vm.count("file") == 0)){
                valid = false;
            } else {
                optargs.filename = vm["file"].as<std::string>();
                optargs.passwd = vm["pass"].as<std::string>();
            }
            
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