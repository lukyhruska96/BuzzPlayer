#include "clients.h"
#include <iostream>

#define CPU_TIME (((double)std::clock())/CLOCKS_PER_SEC*10000)

using namespace std::chrono;

Clients::Clients()
{
}

Clients::~Clients()
{

}

int Clients::add_client()
{
    client_info tmp;
    tmp.id = clients.size();
    tmp.last_answer = duration_cast<milliseconds >(system_clock::now().time_since_epoch());;
    tmp.status = NEW;
    clients.push_back(tmp);
    std::cout << "Added new client with id " << clients.size()-1 << "." << std::endl;
    return tmp.id;
}

void Clients::check_clients()
{
    milliseconds now = duration_cast<milliseconds >(system_clock::now().time_since_epoch());;
    for(int i = 0;i<clients.size(); i++){
        if(now - clients[i].last_answer > milliseconds(6000)) {
            clients[i].status = DEAD;
            std::cout << "User " << i << " is DEAD now." << std::endl;
        }
    }
}

std::string Clients::client_status_str(int id){
    if(id >= clients.size()) return "";
    auto client = get_client(id);
    switch(client.status){
        case NEW:
            return "NEW";
        case DEAD:
            return "DEAD";
        case ALIVE:
            return "ALIVE";
        case READY_TO_PLAY:
            return "READY TO PLAY";
        default:
            return "";
    }

}
client_status_e Clients::client_status(int id){
    return clients[id].status;
}

const client_info& Clients::get_client(int id){
    return clients[id];
}

int Clients::id_alive_client(int id)
{
    int c = 0;
    for(int i = 0; i<id; i++) if(get_client(i).status != DEAD) c++;
    return c;
}

int Clients::alive_count()
{
    int c = 0;
    int size = client_count();
    for(int i = 0; i<size; i++) if(get_client(i).status == ALIVE) c++;
    return c;
}

int Clients::client_count()
{
    return clients.size();
}

void Clients::change_status(int id, client_status_e status)
{
    if(id >= client_count()) return;
    if(status != clients[id].status) std::cout << "User " << id << "status changed from " << client_status_str(id);
    client_status_e old_status = clients[id].status;
    clients[id].status = status;
    if(status != old_status) std::cout << " to " << client_status_str(id) << "." << std::endl;
    if(status == ALIVE) clients[id].last_answer = duration_cast<milliseconds >(system_clock::now().time_since_epoch());
}

bool Clients::all_ready_to_play()
{
    for(client_info tmp : clients) if(tmp.status != READY_TO_PLAY && tmp.status != DEAD) return false;
    return true;
}
