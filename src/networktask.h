#pragma once
#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <thread>
#include <mutex>

#include "util.h"

#define TAG_POPULAR "pop"
#define TAG_RANDOM "rand"
#define TAG_ALL "*"

enum AddrEntryMode{
    MODE_RANGE,
    MODE_POPULAR,
    MODE_RANDOM
};

enum NetworkProtocol{
    TCP,
    UDP
};

typedef struct Ipv4Sub{
    uint8_t i;
    uint8_t mode;
} Ipv4Sub;

typedef struct Ipv4Addr{
    Ipv4Sub ip[4];
    uint16_t port;
    uint8_t port_mode;

    bool operator<(struct Ipv4Addr& addr){
        return (ip[0].i < addr.ip[0].i) &&
            (ip[1].i < addr.ip[1].i) &&
            (ip[2].i < addr.ip[2].i) &&
            (ip[3].i < addr.ip[3].i) &&
            (port < addr.port);
    }
    bool operator>(struct Ipv4Addr& addr){
        return (ip[0].i > addr.ip[0].i) &&
            (ip[1].i > addr.ip[1].i) &&
            (ip[2].i > addr.ip[2].i) &&
            (ip[3].i > addr.ip[3].i) &&
            (port > addr.port);
    }
    bool operator>=(struct Ipv4Addr& addr){
        return (ip[0].i >= addr.ip[0].i) &&
            (ip[1].i >= addr.ip[1].i) &&
            (ip[2].i >= addr.ip[2].i) &&
            (ip[3].i >= addr.ip[3].i) &&
            (port >= addr.port);
    }
    bool operator<=(struct Ipv4Addr& addr){
        return (ip[0].i <= addr.ip[0].i) &&
            (ip[1].i <= addr.ip[1].i) &&
            (ip[2].i <= addr.ip[2].i) &&
            (ip[3].i <= addr.ip[3].i) &&
            (port <= addr.port);
    }
    bool operator==(struct Ipv4Addr& addr){
        return (ip[0].i == addr.ip[0].i) &&
            (ip[1].i == addr.ip[1].i) &&
            (ip[2].i == addr.ip[2].i) &&
            (ip[3].i == addr.ip[3].i) &&
            (port == addr.port);
    }

    bool is_random(){
        return (ip[0].mode == MODE_RANDOM) ||
            (ip[1].mode == MODE_RANDOM) ||
            (ip[2].mode == MODE_RANDOM) ||
            (ip[3].mode == MODE_RANDOM) ||
            (port_mode == MODE_RANDOM);
    }
    void calculate_random(){
        for(size_t sub=0;sub<4;sub++){
            if(ip[sub].mode == AddrEntryMode::MODE_RANDOM){
                ip[sub].i = std::rand()%UINT8_MAX;
            }
        }
        if(port_mode == AddrEntryMode::MODE_RANDOM){
            port = std::rand()%UINT16_MAX;
        }
    }
    std::string to_string_ip(){
        std::string result;
        for(size_t sub=0;sub<4;sub++){
            result += std::to_string(ip[sub].i);

            if(sub!=3){
                result += ".";
            }
        }
        return result;
    }
    std::string to_string(){
        std::string result = to_string_ip();
        result += ":";
        result += std::to_string(port);
        return result;
    }
} Ipv4Addr;

typedef struct Ipv4Range{
    Ipv4Addr ip[2];
} Ipv4Range;

class NetworkTaskThread;
class Logger;

class NetworkTask{
    private:
    std::vector<NetworkTaskThread*> m_threads;
    std::vector<Ipv4Range> m_tasks;
    Logger* m_logger;
    size_t m_current_task = 0;
    Ipv4Addr m_current_ipv4;
    size_t m_max_requests = 0;
    size_t m_requests = 0;
    size_t m_popular_idx = 0;
    uint32_t m_timeout = 500000;
    bool m_ended = false;
    size_t m_threads_num = 5;

    std::mutex lock;
    public:
    NetworkTask(Logger* _logger, std::string taskdata): m_logger(_logger){
        decode(taskdata);
    }
    void decode(std::string taskdata);

    bool is_ended(){
        return m_ended;
    }

    Logger* get_logger(){return m_logger;}
    uint32_t get_timeout(){return m_timeout;}
    void set_timeout(uint32_t _timeout){m_timeout = _timeout;}
    void set_threads_num(uint32_t _threads){m_threads_num = _threads;}
    void set_max_requests(uint32_t _requests){m_max_requests = _requests;}

    Ipv4Addr get_next_ipv4();

    void run();
};

class NetworkTaskThread{
    private:
    NetworkTask* task;
    std::thread* task_thread;
    std::mutex lock;
    public:
    NetworkTaskThread(NetworkTask* _task): task(_task){}
    void start();
    void run();
    void wait(){task_thread->join();}
};