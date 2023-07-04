#include "lapi.h"

#include "protocolmap.h"
#include "logger.h"
#include "networktask.h"

#include "protocol/tcp.h"
#include "protocol/http.h"
#include "protocol/https.h"

/* Logger API*/

static int l_logger_new(lua_State* L){
    int use_stdout = 0;
    if(lua_isboolean(L, 1)){
        use_stdout = lua_toboolean(L, 1);
    }
    *reinterpret_cast<Logger**>(lua_newuserdata(L, sizeof(Logger*))) = new Logger(use_stdout, lua_tostring(L, 2));
    luaL_setmetatable(L, LOGGER_STR);
    return 1;
}

static int l_logger_delete(lua_State* L){
    delete *reinterpret_cast<Logger**>(lua_touserdata(L, 1));
    return 0;
}

static int l_logger_set_hide_fail(lua_State* L){
    (*reinterpret_cast<Logger**>(luaL_checkudata(L, 1, LOGGER_STR)))->set_hide_fail(lua_toboolean(L, 2));
    return 0;
}

static int l_logger_set_hide_skipped(lua_State* L){
    (*reinterpret_cast<Logger**>(luaL_checkudata(L, 1, LOGGER_STR)))->set_hide_skipped(lua_toboolean(L, 2));
    return 0;
}

static int l_logger_set_hide_access_denied(lua_State* L){
    (*reinterpret_cast<Logger**>(luaL_checkudata(L, 1, LOGGER_STR)))->set_hide_access_denied(lua_toboolean(L, 2));
    return 0;
}

static int l_logger_log(lua_State* L){
    (*reinterpret_cast<Logger**>(luaL_checkudata(L, 1, LOGGER_STR)))->log(std::string("\e[36m")+lua_tostring(L, 2));
    return 0;
}

static void l_register_logger(lua_State* L){
    lua_newtable(L);
    lua_pushcfunction(L, l_logger_new);
    lua_setfield(L, -2, "new");
    lua_setglobal(L, LOGGER_STR);

    luaL_newmetatable(L, LOGGER_STR);
    lua_pushcfunction(L, l_logger_delete); lua_setfield(L, -2, "__gc");
    lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, l_logger_set_hide_fail); lua_setfield(L, -2, "set_hide_fail");
    lua_pushcfunction(L, l_logger_set_hide_skipped); lua_setfield(L, -2, "set_hide_skipped");
    lua_pushcfunction(L, l_logger_set_hide_access_denied); lua_setfield(L, -2, "set_hide_access_denied");
    lua_pushcfunction(L, l_logger_log); lua_setfield(L, -2, "log");
    lua_pop(L, 1);
}


/* ProtocolMap API*/

static int l_protocolmap_new(lua_State* L){
    *reinterpret_cast<ProtocolMap**>(lua_newuserdata(L, sizeof(ProtocolMap*))) = new ProtocolMap();
    luaL_setmetatable(L, PROTOCOLMAP_STR);
    return 1;
}

static int l_protocolmap_delete(lua_State* L){
    delete *reinterpret_cast<ProtocolMap**>(lua_touserdata(L, 1));
    return 0;
}

static int l_protocolmap_import(lua_State* L){
    (*reinterpret_cast<ProtocolMap**>(luaL_checkudata(L, 1, PROTOCOLMAP_STR)))->import(lua_tostring(L, 2));
    return 0;
}

static int l_protocolmap_clear(lua_State* L){
    (*reinterpret_cast<ProtocolMap**>(luaL_checkudata(L, 1, PROTOCOLMAP_STR)))->clear();
    return 0;
}

static int l_protocolmap_set(lua_State* L){
    (*reinterpret_cast<ProtocolMap**>(luaL_checkudata(L, 1, PROTOCOLMAP_STR)))->set(lua_tointeger(L, 2),lua_tostring(L, 3));
    return 0;
}

static int l_protocolmap_get(lua_State* L){
    lua_pushstring(L,(*reinterpret_cast<ProtocolMap**>(luaL_checkudata(L, 1, PROTOCOLMAP_STR)))->get(lua_tointeger(L, 2)).c_str());
    return 1;
}

static void l_register_protocolmap(lua_State* L){
    lua_newtable(L);
    lua_pushcfunction(L, l_protocolmap_new);
    lua_setfield(L, -2, "new");
    lua_setglobal(L, PROTOCOLMAP_STR);

    luaL_newmetatable(L, PROTOCOLMAP_STR);
    lua_pushcfunction(L, l_protocolmap_delete); lua_setfield(L, -2, "__gc");
    lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, l_protocolmap_import); lua_setfield(L, -2, "import");
    lua_pushcfunction(L, l_protocolmap_clear); lua_setfield(L, -2, "clear");
    lua_pushcfunction(L, l_protocolmap_set); lua_setfield(L, -2, "set");
    lua_pushcfunction(L, l_protocolmap_get); lua_setfield(L, -2, "get");
    lua_pop(L, 1);
}

/* Ipv4Addr API*/

int l_push_ipv4addr(lua_State* L, Ipv4Addr* addr){
    *reinterpret_cast<Ipv4Addr**>(lua_newuserdata(L, sizeof(Ipv4Addr*))) = addr;
    luaL_setmetatable(L, IPV4ADDR_STR);
}

static void l_register_ipv4addr(lua_State* L){
    luaL_newmetatable(L, IPV4ADDR_STR);
    lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
    lua_pop(L, 1);
}

/* NetworkTask API */

static int l_networktask_new(lua_State* L){
    Logger* logger = *reinterpret_cast<Logger**>(luaL_checkudata(L, 1, LOGGER_STR));
    ProtocolMap* pm = *reinterpret_cast<ProtocolMap**>(luaL_checkudata(L, 2, PROTOCOLMAP_STR));
    if(logger == NULL || pm == NULL){
        lua_pushnil(L);
        return 1;
    }
    *reinterpret_cast<NetworkTask**>(lua_newuserdata(L, sizeof(NetworkTask*))) = new NetworkTask(logger, pm, L);
    luaL_setmetatable(L, NETWORKTASK_STR);
    return 1;
}

static int l_networktask_delete(lua_State* L){
    delete *reinterpret_cast<NetworkTask**>(lua_touserdata(L, 1));
    return 0;
}

static int l_networktask_decode(lua_State* L){
    NetworkTask* task = *reinterpret_cast<NetworkTask**>(luaL_checkudata(L, 1, NETWORKTASK_STR));
    if(lua_isstring(L, 2)){
        task->decode(lua_tostring(L, 2));
    }
    if(lua_istable(L, 2)){
        lua_pushnil(L);
        while(lua_next(L, 2) != 0){
            task->decode(lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    }
    return 0;
}

static int l_networktask_decode_file(lua_State* L){
    NetworkTask* task = *reinterpret_cast<NetworkTask**>(luaL_checkudata(L, 1, NETWORKTASK_STR));
    const char* string = luaL_checkstring(L, 2);
    if(string != NULL){
        task->decode_file(lua_tostring(L, 2));
    }
    if(lua_istable(L, 2)){
        lua_pushnil(L);
        while(lua_next(L, 2) != 0){
            task->decode_file(lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    }
    return 0;
}

static int l_networktask_set_threads(lua_State* L){
    (*reinterpret_cast<NetworkTask**>(luaL_checkudata(L, 1, NETWORKTASK_STR)))->set_threads_num(luaL_checkinteger(L, 2));
    return 0;
}

static int l_networktask_set_raw_output(lua_State* L){
    (*reinterpret_cast<NetworkTask**>(luaL_checkudata(L, 1, NETWORKTASK_STR)))->set_raw_output_filename(luaL_checkstring(L, 2));
    return 0;
}

static int l_networktask_set_timeout(lua_State* L){
    (*reinterpret_cast<NetworkTask**>(luaL_checkudata(L, 1, NETWORKTASK_STR)))->set_timeout(luaL_checkinteger(L, 2));
    return 0;
}

static int l_networktask_set_max_requests(lua_State* L){
    (*reinterpret_cast<NetworkTask**>(luaL_checkudata(L, 1, NETWORKTASK_STR)))->set_max_requests(luaL_checkinteger(L, 2));
    return 0;
}

static int l_networktask_get_timeout(lua_State* L){
    NetworkTask* task = *reinterpret_cast<NetworkTask**>(luaL_checkudata(L, 1, NETWORKTASK_STR));
    lua_pushinteger(L, task->get_timeout());
    return 1;
}

static int l_networktask_get_next_ipv4addr(lua_State* L){
    NetworkTask* task = *reinterpret_cast<NetworkTask**>(luaL_checkudata(L, 1, NETWORKTASK_STR));
    *reinterpret_cast<Ipv4Addr*>(lua_newuserdata(L, sizeof(Ipv4Addr))) = task->get_next_ipv4();
    luaL_setmetatable(L, IPV4ADDR_STR);
    return 1;
}

static int l_networktask_run(lua_State* L){
    (*reinterpret_cast<NetworkTask**>(luaL_checkudata(L, 1, NETWORKTASK_STR)))->run();
    return 0;
}

static int l_networktask_register_onresult_http(lua_State* L){
    int callback_reference = luaL_ref( L, LUA_REGISTRYINDEX );
    (*reinterpret_cast<NetworkTask**>(luaL_checkudata(L, 1, NETWORKTASK_STR)))->set_lua_protocol_callback("http", callback_reference);
    return 0;
}

static int l_networktask_register_onresult_https(lua_State* L){
    int callback_reference = luaL_ref( L, LUA_REGISTRYINDEX );
    (*reinterpret_cast<NetworkTask**>(luaL_checkudata(L, 1, NETWORKTASK_STR)))->set_lua_protocol_callback("https", callback_reference);
    return 0;
}

static int l_networktask_register_onresult_tcp(lua_State* L){
    int callback_reference = luaL_ref( L, LUA_REGISTRYINDEX );
    (*reinterpret_cast<NetworkTask**>(luaL_checkudata(L, 1, NETWORKTASK_STR)))->set_lua_protocol_callback("tcp", callback_reference);
    return 0;
}


static void l_register_networktask(lua_State* L){
    lua_newtable(L);
    lua_pushcfunction(L, l_networktask_new);
    lua_setfield(L, -2, "new");
    lua_setglobal(L, NETWORKTASK_STR);

    luaL_newmetatable(L, NETWORKTASK_STR);
    lua_pushcfunction(L, l_networktask_delete); lua_setfield(L, -2, "__gc");
    lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, l_networktask_decode); lua_setfield(L, -2, "decode");
    lua_pushcfunction(L, l_networktask_decode_file); lua_setfield(L, -2, "decode_file");
    lua_pushcfunction(L, l_networktask_set_max_requests); lua_setfield(L, -2, "set_max_requests");
    lua_pushcfunction(L, l_networktask_set_raw_output); lua_setfield(L, -2, "set_raw_output");
    lua_pushcfunction(L, l_networktask_set_threads); lua_setfield(L, -2, "set_threads");
    lua_pushcfunction(L, l_networktask_set_timeout); lua_setfield(L, -2, "set_timeout");
    lua_pushcfunction(L, l_networktask_get_timeout); lua_setfield(L, -2, "get_timeout");
    lua_pushcfunction(L, l_networktask_get_next_ipv4addr); lua_setfield(L, -2, "get_next_ipv4addr");
    lua_pushcfunction(L, l_networktask_run); lua_setfield(L, -2, "run");
    lua_pushcfunction(L, l_networktask_register_onresult_http); lua_setfield(L, -2, "register_onresult_http");
    lua_pushcfunction(L, l_networktask_register_onresult_https); lua_setfield(L, -2, "register_onresult_https");
    lua_pushcfunction(L, l_networktask_register_onresult_tcp); lua_setfield(L, -2, "register_onresult_tcp");
    lua_pop(L, 1);
}

/* TCP API */



/* HTTP API */

static int l_httprequestresult_get_status(lua_State* L){
    HttpRequestResult* result = *reinterpret_cast<HttpRequestResult**>(luaL_checkudata(L, 1, HTTPREQUESTRESULT_STR));
    lua_pushinteger(L, std::stoi(result->get_status()));
    return 1;
}

static int l_httprequestresult_get_header(lua_State* L){
    HttpRequestResult* result = *reinterpret_cast<HttpRequestResult**>(luaL_checkudata(L, 1, HTTPREQUESTRESULT_STR));
    lua_pushstring(L, result->get_header(luaL_checkstring(L, 2)).c_str());
    return 1;
}

static int l_httprequestresult_get_full_header(lua_State* L){
    HttpRequestResult* result = *reinterpret_cast<HttpRequestResult**>(luaL_checkudata(L, 1, HTTPREQUESTRESULT_STR));
    lua_pushstring(L, result->get_full_header().c_str());
    return 1;
}

static int l_httprequestresult_get_version(lua_State* L){
    HttpRequestResult* result = *reinterpret_cast<HttpRequestResult**>(luaL_checkudata(L, 1, HTTPREQUESTRESULT_STR));
    lua_pushstring(L, result->get_version().c_str());
    return 1;
}

static int l_httprequestresult_get_content_size(lua_State* L){
    HttpRequestResult* result = *reinterpret_cast<HttpRequestResult**>(luaL_checkudata(L, 1, HTTPREQUESTRESULT_STR));
    lua_pushinteger(L, result->get_content_size());
    return 1;
}

static int l_httprequestresult_get_content(lua_State* L){
    HttpRequestResult* result = *reinterpret_cast<HttpRequestResult**>(luaL_checkudata(L, 1, HTTPREQUESTRESULT_STR));
    lua_pushlstring(L, result->get_content_ptr(), result->get_content_size());
    return 1;
}

static void l_register_http(lua_State* L){
    luaL_newmetatable(L, HTTPREQUESTRESULT_STR);
    lua_pushvalue(L, -1); lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, l_httprequestresult_get_status); lua_setfield(L, -2, "get_status");
    lua_pushcfunction(L, l_httprequestresult_get_header); lua_setfield(L, -2, "get_header");
    lua_pushcfunction(L, l_httprequestresult_get_full_header); lua_setfield(L, -2, "get_full_header");
    lua_pushcfunction(L, l_httprequestresult_get_version); lua_setfield(L, -2, "get_version");
    lua_pushcfunction(L, l_httprequestresult_get_content_size); lua_setfield(L, -2, "get_content_size");
    lua_pushcfunction(L, l_httprequestresult_get_content); lua_setfield(L, -2, "get_content");

    lua_pop(L, 1);
}

void l_register_netexplorer_api(lua_State* L){
    luaL_openlibs(L);
    l_register_logger(L);
    l_register_protocolmap(L);
    l_register_ipv4addr(L);
    l_register_networktask(L);
    l_register_http(L);
}
