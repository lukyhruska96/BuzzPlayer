#include "networking.h"
#include "beep.h"
#include <chrono>
#include <iostream>

using namespace std::chrono;

Networking::Networking(char *ip, char *portR, char *portP) : ctx(), req(ctx, ZMQ_REQ), sub(ctx, ZMQ_SUB), inproc(ctx, ZMQ_REQ), sync_count()
{
    char req_addr[30];
    char sub_addr[30];
    std::sprintf(req_addr, "tcp://%s:%s", ip, portR);
    std::sprintf(sub_addr, "tcp://%s:%s", ip, portP);
    req.connect(req_addr);
    sub.connect(sub_addr);
}

void Networking::run()
{
    zmq::message_t msg(3);
    std::memcpy(msg.data(), "HI\0", 3);
    req.send(msg, 0);
    msg = zmq::message_t();
    req.recv(&msg);
    memcpy(&id, msg.data(), msg.size());
    id[msg.size()] = (char)0;
    sub.setsockopt(ZMQ_SUBSCRIBE, id, msg.size());
    sub.setsockopt(ZMQ_SUBSCRIBE, "ALL", 3);
    while(1){
        zmq::message_t my_id;
        zmq::message_t type;
        sub.recv(&my_id);
        sub.recv(&type);
        route(type);
    }
}

void Networking::route(zmq::message_t &type)
{
    if(std::strcmp((char *)type.data(), "DATA") == 0){
        zmq::message_t req_msg(5);
        zmq::message_t resp;
        std::memcpy(req_msg.data(), "DATA\0", 5);
        req.send(req_msg, ZMQ_SNDMORE);
        req.send(id, std::strlen(id)+1, 0);
        req.recv(&resp);
        std::size_t block_size = sizeof(std::uint16_t)+sizeof(float);
        std::size_t block_count = resp.size()/block_size;
        for(int i = 0; i < block_count; i++){
            struct tone_t tmp;
            std::memcpy(&tmp.dur, (void *)((char *)resp.data()+(block_size*i)), sizeof(std::uint16_t));
            std::memcpy(&tmp.freq, (void *)((char *)resp.data()+(block_size*i)+sizeof(std::uint16_t)), sizeof(float));
            play_data.push_back(std::move(tmp));
        }
    }
    else if(std::strcmp((char *)type.data(), "ALIVE?") == 0){
        req.send("YEP\0", 4, ZMQ_SNDMORE);
        req.send(id, std::strlen(id)+1, 0);
        zmq::message_t ok;
        req.recv(&ok);
    }
    else if(std::strcmp((char *)type.data(), "PLAY") == 0){
        sync_count = 0;
        playing = true;
        fut = std::async(std::launch::async, Networking::tplay, std::move(play_data), &sync_count);
        play_data = std::list<tone_t>();
    }
    else if(std::strcmp((char *)type.data(), "SYNC") == 0){
        sync_count.fetch_add(1);
    }
    if(playing && fut.wait_for(seconds(0)) == std::future_status::ready){
        zmq::message_t done_msg(5);
        std::memcpy(done_msg.data(), "DONE\0", 5);
        req.send(done_msg, 0);
        playing = false;
        zmq::message_t ok;
        req.recv(&ok);
        exit(0);
    }
}

bool Networking::tplay(std::list<tone_t> &&data, std::atomic_int *sync)
{
    Beep beep;
    int last_sync = 0;
    float time = 0.0;
    std::uint32_t sec = 0;
    milliseconds last = duration_cast<milliseconds >(system_clock::now().time_since_epoch());
    float time_to_play = 0.0;
    while(!data.empty()){
        int tmp_sync = *sync;
        if(tmp_sync != last_sync){
            time = tmp_sync;
            last_sync = tmp_sync;
            last = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
        }
        else{
            milliseconds tmp_dur = duration_cast<milliseconds>(std::chrono::system_clock::now().time_since_epoch());
            time += std::chrono::duration_cast<milliseconds>(tmp_dur - last).count()/1000;
            last = tmp_dur;
        }
        if(time >= time_to_play){
            tone_t tone = data.front();
            time_to_play += tone.dur/1000.0;
            if(time < time_to_play)
                beep.play(tone.freq, (time_to_play - time)*1000);
            data.pop_front();
        }
    }
    return true;
}
