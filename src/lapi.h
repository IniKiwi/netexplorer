#include "lua/lua.hpp"
#include "networktask.h"

void l_register_netexplorer_api(lua_State* L);

int l_push_ipv4addr(lua_State* L, Ipv4Addr* addr);