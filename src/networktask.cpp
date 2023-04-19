#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <cstring>
#include <cstdio>
#include <unistd.h>

#include "logger.h"
#include "networktask.h"
#include "config.h"

void NetworkTask::decode(std::string taskdata){
    std::vector<std::string> task_list = split(taskdata, ';');
    /*logger->log("raw taskdata: "+taskdata);
    logger->log("task decode list:");
    for(int t=0;t<task_list.size();t++){
        logger->log(task_list[t]+"-"+task_list[t]);
    }*/
    for(size_t task = 0;task<task_list.size();task++){
        Ipv4Range result;
        std::vector<std::string> sections = split(task_list.at(task), ':');
        if(sections.size()==2){
            std::vector<std::string> port_ranges = split(sections[1], '-');
            if(port_ranges.size()==1){
                if(sections[1] == TAG_POPULAR){
                result.ip[0].port_mode = AddrEntryMode::MODE_POPULAR;
                }
                else if(sections[1] == TAG_RANDOM){
                    result.ip[0].port_mode = AddrEntryMode::MODE_RANDOM;
                }
                else if(sections[1] == TAG_ALL){
                    result.ip[0].port_mode = AddrEntryMode::MODE_RANGE;
                    result.ip[0].port = 1;
                    result.ip[1].port = UINT16_MAX;
                }
                else{
                    result.ip[0].port_mode = AddrEntryMode::MODE_RANGE;
                    result.ip[0].port = std::stoi(sections[1]);
                    result.ip[1].port = std::stoi(sections[1]);
                }
            }
            else{
                result.ip[0].port_mode = AddrEntryMode::MODE_RANGE;
                result.ip[0].port = std::stoi(port_ranges[0]);
                result.ip[1].port = std::stoi(port_ranges[1]);
            }

        }
        else{
            result.ip[0].port_mode = AddrEntryMode::MODE_RANGE;
            result.ip[0].port = 1;
            result.ip[1].port = UINT16_MAX;
        }

        std::vector<std::string> sub_ips = split(sections[0], '.');
        if(sub_ips.size()==1){
            if(sub_ips[0] == TAG_RANDOM){
                result.ip[0].ip[0].mode = AddrEntryMode::MODE_RANDOM;
                result.ip[0].ip[1].mode = AddrEntryMode::MODE_RANDOM;
                result.ip[0].ip[2].mode = AddrEntryMode::MODE_RANDOM;
                result.ip[0].ip[3].mode = AddrEntryMode::MODE_RANDOM;
            }
        }
        else if(sub_ips.size()==4){
            for(size_t sub=0;sub<sub_ips.size();sub++){
                std::vector<std::string> sub_ranges = split(sub_ips[sub], '-');
                if(sub_ranges.size()==1){
                    if(sub_ranges[0] == TAG_RANDOM){
                        result.ip[0].ip[sub].mode = AddrEntryMode::MODE_RANDOM;
                    }
                    else if(sub_ranges[0] == TAG_ALL){
                        result.ip[0].ip[sub].mode = AddrEntryMode::MODE_RANGE;
                        result.ip[0].ip[sub].i = 0;
                        result.ip[1].ip[sub].i = UINT8_MAX;
                    }
                    else{
                        result.ip[0].ip[sub].mode = AddrEntryMode::MODE_RANGE;
                        result.ip[0].ip[sub].i = std::stoi(sub_ranges[0]);
                        result.ip[1].ip[sub].i = std::stoi(sub_ranges[0]);
                    }
                }
                else if(sub_ranges.size()==2){
                    result.ip[0].ip[sub].mode = AddrEntryMode::MODE_RANGE;
                    result.ip[0].ip[sub].i = std::stoi(sub_ranges[0]);
                    result.ip[1].ip[sub].i = std::stoi(sub_ranges[1]);
                }
            }
        }
        tasks.push_back(result);
    }
}

Ipv4Addr NetworkTask::get_next_ipv4(){
    lock.lock();
    if(requests == 0 && current_task == 0){
        current_ipv4 = tasks[0].ip[0];
        current_ipv4.calculate_random();
    }
    if(current_ipv4.port_mode == AddrEntryMode::MODE_RANDOM){
        current_ipv4.calculate_random();
    }
    else if(current_ipv4.port_mode == AddrEntryMode::MODE_RANGE){
        if(current_ipv4.port < tasks[current_task].ip[1].port){
            current_ipv4.port++;
            goto _exit;
        }
        else{
            current_ipv4.port = tasks[current_task].ip[0].port;
        }
        bool is_rand = false;
        for(int sub=3;sub >= 0;sub--){
            if(current_ipv4.ip[sub].mode == AddrEntryMode::MODE_RANDOM){
                is_rand = true;
                current_ipv4.ip[sub].i = std::rand()%UINT8_MAX;
            }
            if(current_ipv4.ip[sub].mode == AddrEntryMode::MODE_RANGE){
                if(is_rand == false){
                    if(current_ipv4.ip[sub].i < tasks[current_task].ip[1].ip[sub].i){
                        current_ipv4.ip[sub].i++;
                        goto _exit;
                        //break;
                    }
                    else{
                        if(sub == 0){
                            //logger->log("end of task: "+std::to_string(current_task)+"/"+std::to_string(tasks.size()));
                            current_task++;
                            if(tasks.size() > current_task){
                                current_ipv4 = tasks[current_task].ip[0];
                                current_ipv4.calculate_random();
                                goto _exit;
                            }
                            else{
                                ended = true;
                                goto _exit;
                            }
                        }
                        current_ipv4.ip[sub].i = tasks[current_task].ip[0].ip[sub].i;
                    }
                }
            }
        }
    }
    /*if(current_ipv4.is_random()){
        for(size_t sub=0;sub<4;sub++){
            if(current_ipv4.ip[sub].mode == AddrEntryMode::MODE_RANDOM){
                current_ipv4.ip[sub].i = std::rand()%UINT8_MAX;
            }
            if(current_ipv4.ip[sub].mode == AddrEntryMode::MODE_RANGE){
                // keep original value
            }
        }
        if(current_ipv4.port_mode == AddrEntryMode::MODE_RANDOM){
            current_ipv4.port = std::rand()%UINT16_MAX;
        }
        else if(current_ipv4.port_mode == AddrEntryMode::MODE_RANGE){
            // keep original value
        }
        else if(current_ipv4.port_mode == AddrEntryMode::MODE_POPULAR){
            // TODO
        }
    }*/
    _exit:
    requests++;
    if(max_requests == 0){

    }
    else if(requests > max_requests){
        ended = true;
    }
    Ipv4Addr r = current_ipv4;
    lock.unlock();
    return r;
}

void NetworkTask::run(){
    logger->log(std::string("\e[32m\e[1m")+PRODUCT_STR);
    logger->log("task list:");
    for(int t=0;t<tasks.size();t++){
        if(tasks[t].ip[0].is_random()){
            logger->log("\trandom...");
            break;
        }
        else{
            logger->log("\t"+tasks[t].ip[0].to_string()+" / "+tasks[t].ip[1].to_string());
        }
    }
    for(int i=0;i<threads_num;i++){
        NetworkTaskThread* t = new NetworkTaskThread(this);
        threads.push_back(t);
        t->start();
    }
    for(int i=0;i<threads_num;i++){
        threads.at(i)->wait();
    }
}

void NetworkTaskThread::start(){
    task_thread = new std::thread(&NetworkTaskThread::run,this);
}

void NetworkTaskThread::run(){
    while(1){
        if(task->is_ended()){
            break;
        }
        Ipv4Addr addr = task->get_next_ipv4();
        std::string ip_str = addr.to_string_ip();

        int sockfd = socket(AF_INET,SOCK_STREAM,0);
        struct timeval timeout;      
        timeout.tv_sec = 0;
        timeout.tv_usec = task->get_timeout();
        
        setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        setsockopt (sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

        struct sockaddr_in server;
        std::memset(&server,0,sizeof(struct sockaddr_in));
        server.sin_addr.s_addr = inet_addr(addr.to_string_ip().c_str());
        server.sin_family = AF_INET;
        server.sin_port = htons(addr.port);

        if(connect(sockfd, (struct sockaddr*)&server, sizeof(server)) != -1){
            std::string msg;
            task->get_logger()->log_request(RequestStatus::OK, addr, msg);
        }
        else{
            task->get_logger()->log_request(RequestStatus::FAIL, addr, "");
        }
        close(sockfd);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}