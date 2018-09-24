#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <getopt.h>
#include "fileparser.h"
#include "networking.h"

void print_help(){
    std::cout << "Buzzplayer client" << std::endl;
    std::cout << "Usage: buzz_client [SWITCHES] ... [FILE PATH]" << std::endl;
    std::cout << "Synchronized networked 8bit player" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Help:" << std::endl;
    std::cout << "\t--help, -h\tShow this help" << std::endl;
    std::cout << "\t--portR, -r\tSpecify custom receiver port" << std::endl;
    std::cout << "\t--portP, -p\tSpecify custom publisher port" << std::endl;
}

int main(int argc, char *argv[])
{
    char *file_path = argv[argc-1];
    char portP[16] = "8021";
    char portR[16] = "8020";
    char c;
    while(1){
        static struct option long_options[] = {
            {"help", no_argument, 0, 'h'},
            {"portR", required_argument, 0, 'r'},
            {"portP", required_argument, 0, 'p'}
        };
        int opt_index;
        c = getopt_long(argc, argv, "hp:r:", long_options, &opt_index);
        if(c == -1)
            break;
        switch(c){
            case 'p':
                std::strcpy(portP, optarg);
                break;
            case 'r':
                std::strcpy(portR, optarg);
                break;
            case 'h':
                print_help();
                exit(1);
        }
    }
    for(int i = 1; i<argc-1; i++){
        if(std::strcmp(argv[i], "--portR") == 0 || std::strcmp(argv[i], "-r") == 0){
            std::memset(portR, 0, 16);
            std::strcpy(portR, argv[i+1]);
        }
        else if(std::strcmp(argv[i], "--portP") == 0 || std::strcmp(argv[i], "-p") == 0){
            std::memset(portP, 0, 16);
            std::strcpy(portP, argv[i+1]);
        }
        else if(std::strcmp(argv[i], "--help") == 0 || std::strcmp(argv[i], "-h") == 0){
            print_help();
            exit(1);
        }
    }
    FileParser fp(file_path);
    std::vector<std::vector<tone_t> > data;
    try{
        data = fp.parse();
    }
    catch(std::invalid_argument e){
        std::cerr << e.what() << std::endl;
        exit(1);
    }
    std::cout << "Waiting for " << data.size() << " clients to connect." << std::endl;
    Clients clients;
    Networking net(portP, portR);
    net.run(std::move(data), clients);

    return 0;
}