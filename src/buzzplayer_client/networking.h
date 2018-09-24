#ifndef NETWORKING_H
#define NETWORKING_H
#include <zmq.hpp>
#include <cstdint>
#include <atomic>
#include <future>
#include <list>

struct tone_t{
    std::uint16_t dur;
    float freq;
};

class Networking
{
public:
    Networking(char *ip, char *portR, char *portP);
    void run();
    static bool tplay(std::list<tone_t> &&data, std::atomic_int *sync);
private:
    void route(zmq::message_t &type);

    std::atomic_int sync_count;
    std::future<bool> fut;
    bool playing = false;

    zmq::context_t ctx;
    zmq::socket_t sub;
    zmq::socket_t req;
    zmq::socket_t inproc;

    std::list<tone_t> play_data;

    char id[64];
};

#endif // NETWORKING_H
