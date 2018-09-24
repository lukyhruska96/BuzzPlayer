#include "networking.h"
#include <chrono>
#include <string>
#include <cstdio>
#include <sstream>
#include <iostream>

using namespace std::chrono;

Networking::Networking(char *portP, char *portR) : ctx(), pub(ctx, ZMQ_PUB), rep(ctx, ZMQ_REP)
{
    char rep_addr[30];
    char pub_addr[30];
    std::sprintf(rep_addr, "tcp://*:%s", portR);
    std::sprintf(pub_addr, "tcp://*:%s", portP);

    rep.bind(rep_addr);
    pub.bind(pub_addr);
}

void Networking::run(std::vector<std::vector<tone_t> > &&data, Clients &clients){
    this->data = data;
    this->clients = &clients;
    zmq::pollitem_t rep_poll = {rep, 0, ZMQ_POLLIN, 0};
    bool data_sent = false;
    bool playing = false;
    milliseconds last_second_ms;
    while(1){
        zmq::poll(&rep_poll, ZMQ_POLLIN, 10);
        if(rep_poll.revents && ZMQ_POLLIN){
            zmq::message_t msg;
            rep.recv(&msg);
            this->route(msg);
        }
        if(this->get_play()){
            if(!data_sent){
                pub.send((void*)"ALL", 3, ZMQ_SNDMORE);
                pub.send((void*)"DATA\0", 5, 0);
                data_sent = true;
                client_playing = this->clients->alive_count();
            }
            else if(this->clients->all_ready_to_play()){
                if(!playing){
                    pub.send((void*)"ALL", 3, ZMQ_SNDMORE);
                    pub.send((void*)"PLAY\0", 5);
                    std::cout << "Starting to play." << std::endl;
                    playing = true;
                    last_second_ms = duration_cast<milliseconds >(system_clock::now().time_since_epoch());
                }
            }
        }
        milliseconds tmp_ms =duration_cast<milliseconds >(system_clock::now().time_since_epoch());
        if(tmp_ms - last_second_ms > milliseconds(1000)){
            last_second_ms = tmp_ms;
            every_second(playing);
        }
        if(this->clients->alive_count() >= this->data.size() && !get_play()){
            set_play(true);
            std::cout << "Getting ready to play." << std::endl;
        }
    }
}

void Networking::set_play(bool play)
{
    this->play = play;
}

void Networking::route(zmq::message_t &msg){
    zmq::message_t reply;
    if(std::strcmp((char *) msg.data(), "HI") == 0){
        int id = clients->add_client();
        std::string str_id(std::to_string(id));
        reply.rebuild(str_id.length());
        memcpy(reply.data(), str_id.c_str(), str_id.length());
        rep.send(reply, 0);
    }
    else if(std::strcmp((char *) msg.data(), "YEP") == 0){
        zmq::message_t id_msg;
        rep.recv(&id_msg);
        clients->client_alive(std::atoi((char *)id_msg.data()));
        rep.send("OK\0", 3, 0);
    }
    else if(std::strcmp((char*) msg.data(), "DATA") == 0){
        zmq::message_t id_msg;
        rep.recv(&id_msg);
        int id = std::atoi((char*)id_msg.data());
        reply_data(clients->id_alive_client(id));
    }
    else if(std::strcmp((char*) msg.data(), "DONE") == 0){
        client_done++;
        rep.send("OK\0", 3, 0);
        if(client_done >= client_playing){
            set_play(false);
            client_done = client_playing = 0;
            std::cout << "Done." << std::endl;
            exit(0);
        }
    }
    clients->check_clients();
}


bool Networking::get_play()
{
    return this->play;
}

void Networking::every_second(bool &playing)
{
    if(playing){
        pub.send((void*)"ALL", 3, ZMQ_SNDMORE);
        pub.send((void*)"SYNC\0", 5);
    }
    else{
        pub.send((void*)"ALL", 3, ZMQ_SNDMORE);
        pub.send((void*)"ALIVE?\0", 7);
    }
}

void Networking::reply_data(int id)
{
    std::vector<tone_t> &tone_row = data.at(id % data.size());
    std::size_t block_size = sizeof(std::uint16_t)+sizeof(float);
    zmq::message_t reply(block_size * tone_row.size());
    clients->change_status(id, READY_TO_PLAY);
    for(int i = 0; i<tone_row.size(); i++){
        std::memcpy((void*)((char *)reply.data()+(i*block_size)), &tone_row.at(i).dur, sizeof(std::uint16_t));
        std::memcpy((void *)((char *)reply.data()+(i*block_size)+sizeof(std::uint16_t)), &tone_row.at(i).freq, sizeof(float));
    }
    rep.send(reply, 0);
}