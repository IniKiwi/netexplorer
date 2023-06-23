#include "networktask.h"
#include "logger.h"

#include <sys/signal.h>

const char cli_help_msg[] = "IniKiwi's netexplorer\n" \
"the manual is on https://github.com/IniKiwi/netexplorer\n";

#define TAG_EXPLORE "explore"
#define TAG_EXPLORE_FILE_RAW "explore-file-raw"
#define TAG_SEARCH "search"

uint32_t timeout = 500000;
uint32_t requests = 0;
uint32_t threads = 5;
bool hide_fail = false;
bool hide_access_denied = false;
bool hide_skipped = false;

enum Tasks{
    TASK_EXPLORE,
    TASK_EXPLORE_FILE_RAW,
    TASK_SEARCH
};

int task;
int task_argc;
std::string rawoutput_filename = "";
std::string log_filename = "";
std::string protocol_map_filename = "";


int main(int argc, char *argv[]){
    signal(SIGPIPE, SIG_IGN);
    srand(time(NULL));
    Logger* logger = new Logger(true,"output.log");
    ProtocolMap* protocolmap = new ProtocolMap();
    for(int a=1;a<argc;a++){
        if(std::string(argv[a]) == TAG_EXPLORE){
            if(argc < a+1){exit(1);}
            task = Tasks::TASK_EXPLORE;
            task_argc = a;
        }
        if(std::string(argv[a]) == TAG_EXPLORE_FILE_RAW){
            if(argc < a+1){exit(1);}
            task = Tasks::TASK_EXPLORE_FILE_RAW;
            task_argc = a;
        }
        if(std::string(argv[a]) == TAG_SEARCH){
            if(argc < a+1){exit(1);}
            task = Tasks::TASK_SEARCH;
            task_argc = a;
        }
        if(std::string(argv[a]) == "-j"){
            if(argc < a+1){exit(1);}
            threads = atoi(argv[a+1]);
        }
        if(std::string(argv[a]) == "-t"){
            if(argc < a+1){exit(1);}
            timeout = atoi(argv[a+1]);
        }
        if(std::string(argv[a]) == "-r"){
            if(argc < a+1){exit(1);}
            requests = atoi(argv[a+1]);
        }
        if(std::string(argv[a]) == "--hide-fail"){
            hide_fail = true;
        }
        if(std::string(argv[a]) == "--hide-access-denied"){
            hide_access_denied = true;
        }
        if(std::string(argv[a]) == "--hide-skipped"){
            hide_skipped = true;
        }
        if(std::string(argv[a]) == "--raw-output"){
            if(argc < a+1){exit(1);}
            rawoutput_filename = argv[a+1];
        }
        if(std::string(argv[a]) == "--protocol-map"){
            if(argc < a+1){exit(1);}
            protocol_map_filename = argv[a+1];
        }
    }

    logger->set_hide_fail(hide_fail);
    logger->set_hide_access_denied(hide_access_denied);
    logger->set_hide_skipped(hide_skipped);
    if(protocol_map_filename != ""){
        protocolmap->import(protocol_map_filename);
    }
    else{
        if(access("protocolmap.txt",F_OK) != -1){
            protocolmap->import("protocolmap.txt");
        }
    }
    if(task == Tasks::TASK_EXPLORE){
        NetworkTask* task = new NetworkTask(logger, protocolmap);
        task->decode(argv[task_argc+1]);
        task->set_timeout(timeout);
        task->set_threads_num(threads);
        task->set_max_requests(requests);
        task->set_raw_output_filename(rawoutput_filename);
        task->run();
        task->write_raw_results();
    }
    else if(task == Tasks::TASK_EXPLORE_FILE_RAW){
        NetworkTask* task = new NetworkTask(logger, protocolmap);
        task->set_timeout(timeout);
        task->set_threads_num(threads);
        task->set_max_requests(requests);
        task->set_raw_output_filename(rawoutput_filename);
        task->decode_file(argv[task_argc+1]);
        task->run();
        task->write_raw_results();
    }

    delete logger;
}