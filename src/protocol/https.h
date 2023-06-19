#pragma once

#include "tls.h"
#include "http.h"

HttpRequestResult* https_get(SSL* ssl, Ipv4Addr addr, std::string path);
int https_action(Ipv4Addr addr, std::string path, NetworkTask* task);