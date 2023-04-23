#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <cstring>
#include <cstdio>
#include <unistd.h>

#include "logger.h"
#include "networktask.h"
#include "config.h"

int NetworkTask::decode(std::string taskdata){
    std::vector<std::string> task_list = split(taskdata, ';');
    /*logger->log("raw taskdata: "+taskdata);
    logger->log("task decode list:");
    for(int t=0;t<task_list.size();t++){
        logger->log(task_list[t]+"-"+task_list[t]);
    }*/
    for(size_t task = 0;task<task_list.size();task++){
        Task result;
        result.hostname = "";
        //Ipv4Range result;
        std::vector<std::string> protocol = split_l(task_list.at(task), "://");
        //m_logger->log(std::to_string(protocol.size()));
        //m_logger->log(protocol[0]);
        //m_logger->log(protocol[1]);
        std::vector<std::string> sections;
        if(protocol.size()==1){
            sections = split(protocol[0], ':');
            result.protocol = "tcp-unknow";
            result.range.ip[0].protocol = "tcp-unknow";
        }
        else if(protocol.size()==2){
            sections = split(protocol[1], ':');
            result.protocol = protocol[0];
            result.range.ip[0].protocol = protocol[0];
        }
        else{
            return -1;
        }
        
        if(sections.size()==2){
            std::vector<std::string> port_ranges = split(sections[1], '-');
            if(port_ranges.size()==1){
                if(sections[1] == TAG_POPULAR){
                result.range.ip[0].port_mode = AddrEntryMode::MODE_POPULAR;
                }
                else if(sections[1] == TAG_RANDOM){
                    result.range.ip[0].port_mode = AddrEntryMode::MODE_RANDOM;
                }
                else if(sections[1] == TAG_ALL){
                    result.range.ip[0].port_mode = AddrEntryMode::MODE_RANGE;
                    result.range.ip[0].port = 1;
                    result.range.ip[1].port = UINT16_MAX;
                }
                else{
                    result.range.ip[0].port_mode = AddrEntryMode::MODE_RANGE;
                    result.range.ip[0].port = std::stoi(sections[1]);
                    result.range.ip[1].port = std::stoi(sections[1]);
                }
            }
            else{
                result.range.ip[0].port_mode = AddrEntryMode::MODE_RANGE;
                result.range.ip[0].port = std::stoi(port_ranges[0]);
                result.range.ip[1].port = std::stoi(port_ranges[1]);
            }

        }
        else{
            result.range.ip[0].port_mode = AddrEntryMode::MODE_RANGE;
            result.range.ip[0].port = 1;
            result.range.ip[1].port = 1000;
        }

        int r = -1;
        
        try{
            r = decode_ipv4(sections[0], result);
        }
        catch(std::exception e){}

        if(r == -1){
            r = decode_domain(sections[0], result);
        }
        if(r == -1){
            continue;
        }
    }
    return 0;
}

int NetworkTask::decode_ipv4(std::string rawdata, Task base){
    Task result = base;
    std::vector<std::string> sub_ips = split(rawdata, '.');
    if(sub_ips.size()==1){
        if(sub_ips[0] == TAG_RANDOM){
            result.range.ip[0].ip[0].mode = AddrEntryMode::MODE_RANDOM;
            result.range.ip[0].ip[1].mode = AddrEntryMode::MODE_RANDOM;
            result.range.ip[0].ip[2].mode = AddrEntryMode::MODE_RANDOM;
            result.range.ip[0].ip[3].mode = AddrEntryMode::MODE_RANDOM;
        }
        else{
            return -1;
        }
    }
    else if(sub_ips.size()==4){
        for(size_t sub=0;sub<sub_ips.size();sub++){
            std::vector<std::string> sub_ranges = split(sub_ips[sub], '-');
            if(sub_ranges.size()==1){
                if(sub_ranges[0] == TAG_RANDOM){
                    result.range.ip[0].ip[sub].mode = AddrEntryMode::MODE_RANDOM;
                }
                else if(sub_ranges[0] == TAG_ALL){
                    result.range.ip[0].ip[sub].mode = AddrEntryMode::MODE_RANGE;
                    result.range.ip[0].ip[sub].i = 0;
                    result.range.ip[1].ip[sub].i = UINT8_MAX;
                }
                else{
                    result.range.ip[0].ip[sub].mode = AddrEntryMode::MODE_RANGE;
                    result.range.ip[0].ip[sub].i = std::stoi(sub_ranges[0]);
                    result.range.ip[1].ip[sub].i = std::stoi(sub_ranges[0]);
                }
            }
            else if(sub_ranges.size()==2){
                result.range.ip[0].ip[sub].mode = AddrEntryMode::MODE_RANGE;
                result.range.ip[0].ip[sub].i = std::stoi(sub_ranges[0]);
                result.range.ip[1].ip[sub].i = std::stoi(sub_ranges[1]);
            }
            else{
                return -1;
            }
        }
    }
    else{
        return -1;
    }
    m_tasks.push_back(result);
}

int NetworkTask::decode_domain(std::string rawdata, Task base){
    struct hostent* remotehost;
    m_logger->log("\e[1msearching host data for \""+rawdata+"\"...");
    base.hostname = rawdata;
    base.range.ip->hostname = rawdata;
    remotehost = gethostbyname(rawdata.c_str());

    if (remotehost == NULL){
        m_logger->log(std::string("\t")+MSG_ERROR+hstrerror(h_errno));
        return -1;
    }
    m_logger->log(std::string("\tname: \t\t")+remotehost->h_name);
    std::string aliases = "\taliases: \t";
    for(int i=0;remotehost->h_aliases[i] != NULL;i++){
        aliases += remotehost->h_aliases[i];
    }
    m_logger->log(aliases);

    std::string addrs = "\taddr: \t\t";
    struct in_addr** addr_list = (struct in_addr **)remotehost->h_addr_list;
    size_t addr_s;
    for(addr_s=0; addr_list[addr_s] != NULL; addr_s++) {
        
    }
    int t = 0;
    for(int i=0;i < addr_s;i++){
        addrs += inet_ntoa(*addr_list[i]);
        if(decode_ipv4(inet_ntoa(*addr_list[i]), base) < 0){

        }
        else{
            t++;
        }
        if(i != addr_s-1){
            addrs += ";";
        }
    }
    m_logger->log(addrs);

    std::string reverse = "\treverse: \t";
    for(int i=0;i < addr_s;i++){
        struct hostent *hp;
        if ((hp = gethostbyaddr((const void *)addr_list[i], sizeof(struct in_addr), AF_INET)) != NULL) {
            reverse += hp->h_name;
            if(i != addr_s-1){
                addrs += ", ";
            }
        }
    }
    m_logger->log(reverse);
}

Ipv4Addr NetworkTask::get_next_ipv4(){
    lock.lock();
    if(m_requests == 0 && m_current_task == 0){
        m_current_ipv4 = m_tasks[0].range.ip[0];
        m_current_ipv4.calculate_random();
    }
    if(m_current_ipv4.port_mode == AddrEntryMode::MODE_RANDOM){
        m_current_ipv4.calculate_random();
    }
    else if(m_current_ipv4.port_mode == AddrEntryMode::MODE_RANGE){
        if(m_current_ipv4.port < m_tasks[m_current_task].range.ip[1].port){
            m_current_ipv4.port++;
            goto _exit;
        }
        else{
            m_current_ipv4.port = m_tasks[m_current_task].range.ip[0].port;
        }
        bool is_rand = false;
        for(int sub=3;sub >= 0;sub--){
            if(m_current_ipv4.ip[sub].mode == AddrEntryMode::MODE_RANDOM){
                is_rand = true;
                m_current_ipv4.ip[sub].i = std::rand()%UINT8_MAX;
            }
            if(m_current_ipv4.ip[sub].mode == AddrEntryMode::MODE_RANGE){
                if(is_rand == false){
                    if(m_current_ipv4.ip[sub].i < m_tasks[m_current_task].range.ip[1].ip[sub].i){
                        m_current_ipv4.ip[sub].i++;
                        goto _exit;
                        //break;
                    }
                    else{
                        if(sub == 0){
                            //logger->log("end of task: "+std::to_string(current_task)+"/"+std::to_string(tasks.size()));
                            m_current_task++;
                            if(m_tasks.size() > m_current_task){
                                m_current_ipv4 = m_tasks[m_current_task].range.ip[0];
                                m_current_ipv4.calculate_random();
                                goto _exit;
                            }
                            else{
                                m_ended = true;
                                goto _exit;
                            }
                        }
                        m_current_ipv4.ip[sub].i = m_tasks[m_current_task].range.ip[0].ip[sub].i;
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
    m_requests++;
    if(m_max_requests == 0){

    }
    else if(m_requests > m_max_requests){
        m_ended = true;
        write_raw_results();
    }
    if(m_last_raw_write+1000 < m_requests){
        write_raw_results();
    }
    Ipv4Addr r = m_current_ipv4;
    lock.unlock();
    return r;
}

void NetworkTask::write_raw_results(){
    if(m_raw_output_filename == "") return;
    std::ofstream file;
    file.open(m_raw_output_filename, std::ios::out | std::ios::app);
    if(file.is_open()){
        size_t s = m_raw_output_list.size();
        for(size_t i=0;i<s;i++){
            file << m_raw_output_list[i];
            file << ";";
        }
        file.close();
    }
    m_raw_output_list.clear();
    m_last_raw_write = m_requests;
}

void NetworkTask::run(){
    if(m_ok == false) return;
    m_logger->log(std::string("\e[32m\e[1m")+PRODUCT_STR);
    m_logger->log("task list:");
    for(int t=0;t<m_tasks.size();t++){
        if(m_tasks[t].range.ip[0].is_random()){
            m_logger->log("\trandom...");
            break;
        }
        else{
            m_logger->log("\t"+m_tasks[t].protocol+"://"+m_tasks[t].range.ip[0].to_string()+" / "+m_tasks[t].range.ip[1].to_string());
        }
    }
    for(int i=0;i<m_threads_num;i++){
        NetworkTaskThread* t = new NetworkTaskThread(this);
        m_threads.push_back(t);
        t->start();
    }
    for(int i=0;i<m_threads_num;i++){
        m_threads.at(i)->wait();
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
            task->push_raw_result(addr);
        }
        else{
            task->get_logger()->log_request(RequestStatus::FAIL, addr, "");
        }
        close(sockfd);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}