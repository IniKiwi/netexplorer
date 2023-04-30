#pragma once

#include "../networktask.h"

int tcp_connect(Ipv4Addr addr, size_t timeout);

int tcp_action(Ipv4Addr addr, NetworkTask* task);