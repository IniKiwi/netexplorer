#include "protocolmap.h"
#include "exceptions.h"
#include <cstdio>
#include <ctype.h>
#include <cstring>
#include <fstream>

#define TOKEN_DEFAULT "default"

void ProtocolMap::clear(){
    m_protocol_default = "tcp";
    m_protocol_map.clear();
}

static inline char* pm_next_token(char* pos){
    while(*pos == ' ' || *pos == '\t' || *pos == '\r'){
        pos++;
    }
    return pos;
}

void ProtocolMap::import(std::string filename){
    FILE* file = fopen(filename.c_str(), "r");
    if(file == NULL) throw FileNotFoundException(filename);
    
    fseek(file, 0L, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0L, SEEK_SET);

    char* content = (char*)malloc(file_size+1);
    memset(content, 0, file_size+1);
    fread(content, file_size, 1, file);
    fclose(file);

    char* pos = content;
    while(1){
        if(*pos == 0){
            break;
        }
        else if(*pos == '#'){
            while(*pos != '\n'){
                pos++;
                if(*pos == '\0') break;
            }
        }
        else if(memcmp(pos,TOKEN_DEFAULT,strlen(TOKEN_DEFAULT)) == 0){
            pos += strlen(TOKEN_DEFAULT);
            pos = pm_next_token(pos);
            char* pos2 = pos;
            while(*pos2 != '\r' && *pos2 != '\n' && *pos2 != '#' ){
                if(*pos2 == '\0') break;
                pos2++;
            }
            m_protocol_default = std::string(pos, pos2-pos);
            pos += pos2-pos;
        }
        else if(isdigit(*pos)){
            uint16_t port = std::stoi(pos);
            while(*pos != ' ' && *pos != '\t'){
                pos++;
            }
            char* pos2 = pos;
            while(*pos2 != '\r' && *pos2 != '\n' && *pos2 != '#'){
                if(*pos2 == '\0') break;
                pos2++;
            }
            set(port, std::string(pos, pos2-pos));
            pos += pos2-pos;
        }
        else{
            pos++;
        }

    }
    free(content);
}

std::string ProtocolMap::get(uint16_t port){
    try{
        return m_protocol_map.at(port);
    }
    catch(std::exception){};
    return m_protocol_default;
}

void ProtocolMap::set(uint16_t port, std::string entry){
    m_protocol_map.insert_or_assign(port,entry);
}