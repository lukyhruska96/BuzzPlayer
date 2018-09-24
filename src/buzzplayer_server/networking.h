#ifndef NETWORKING_H
#define NETWORKING_H

#include <zmq.hpp>
#include <cinttypes>
#include <mutex>

#include "fileparser.h"
#include "clients.h"


class Networking
{
public:
    Networking(char *portP, char *portR);
    void run(std::vector<std::vector<tone_t>> &&data, Clients &clients);
    void set_play(bool play);
private:
    void route(zmq::message_t &msg);
    bool get_play();
    void every_second(bool &playing);
    void reply_data(int id);
    zmq::context_t ctx;
    zmq::socket_t rep;
    zmq::socket_t pub;
    bool play = false;
    std::mutex mtx;
    std::vector<std::vector<tone_t>> data;
    Clients *clients;
    int client_playing = 0;
    int client_done = 0;
};

#endif // NETWORKING_H
