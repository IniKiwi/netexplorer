#pragma once
#include "networktask.h"
#include <string>
#include <fstream>
#include <mutex>

//bolt - green - OK! - normal color - normal text
#define MSG_OK "\e[1m\e[32mOK\e[39m\e[0m"
#define MSG_FAIL "\e[1m\e[31mFAIL\e[39m\e[0m"
#define MSG_SKIPPED "\e[1m\e[33mSKIPPED\e[39m\e[0m"

enum RequestStatus{
    OK,
    FAIL,
    SKIPPED
};

class Logger{
    private:
    std::mutex lock;
    bool use_stdout;
    bool hide_fail;
    std::ofstream logfile;

    public:
    Logger(bool _use_stdout, std::string output): use_stdout(_use_stdout) {
        logfile.open(output, std::ios::out | std::ios::app);
    }
    ~Logger(){
        if(logfile.is_open()){
            logfile.close();
        }
    }

    std::string get_time_str();
    void set_hide_fail(bool v){hide_fail = v;}
    void log(std::string msg);
    void log_request(int status, Ipv4Addr addr, std::string msg);
};