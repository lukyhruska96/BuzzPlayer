#ifndef CLIENTS_H
#define CLIENTS_H

#include <string>
#include <vector>
#include <mutex>
#include <chrono>

enum client_status_e{
    NEW, DEAD, ALIVE, READY_TO_PLAY
};

// structure for list element
struct client_info{
    std::uint32_t id;
    client_status_e status;
    std::chrono::milliseconds last_answer;
};

class Clients
{
public:
    Clients();
    ~Clients();
    int add_client();
    void check_clients();
    inline void client_alive(int id){change_status(id, ALIVE);}
    inline void client_ready_to_play(int id){change_status(id, READY_TO_PLAY);}
    inline void client_dead(int id){change_status(id, DEAD);}
    client_status_e client_status(int id);
    std::string client_status_str(int id);
    const client_info& get_client(int id);
    int id_alive_client(int id);
    int alive_count();
    int client_count();
    void change_status(int id, client_status_e status);
    bool all_ready_to_play();
    std::size_t size(){ return clients.size();}
private:
    std::vector<client_info> clients;
    double lastCheck = 0;
};

#endif // CLIENTS_H
