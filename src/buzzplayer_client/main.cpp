#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <zmq.hpp>
#include <iostream>
#include <cstdio>

#include "networking.h"

void print_help(){
    printf("Buzzplayer client\n");
    printf("Usage: buzz_client [SWITCHES] ...\n");
    printf("Synchronized networked 8bit player\n");
    printf("\n");
    printf("Help:\n");
    printf("\t--help, -h\tShow this help\n");
    printf("\t--portR, -r\tSpecify custom receiver port\n");
    printf("\t--portP, -p\tSpecify custom publisher port\n");
    printf("\t--ip, -i\tSpecify custom IP address\n");
}

int main(int argc, char *argv[])
{
    char portR[5] = "8020";
    char portP[5] = "8021";
    char ip[16] = "localhost";
    char c;
    while(1){
        static struct option long_options[] = {
            {"help", no_argument, 0, 'h'},
            {"portR", required_argument, 0, 'r'},
            {"portP", required_argument, 0, 'p'},
            {"ip", required_argument, 0, 'i'}
        };
        int opt_index;
        c = getopt_long(argc, argv, "hp:r:i:",
        long_options, &opt_index);
        if(c == -1)
            break;
        switch(c){
            case 'i':
                std::strcpy(ip, optarg);
                break;
            case 'p':
                std::strcpy(portP, optarg);
                break;
            case 'r':
                std::strcpy(portR, optarg);
                break;
            case 'h':
                print_help();
                exit(0);
        }
    }
    Networking net(ip, portR, portP);
    net.run();

    return 0;
}
