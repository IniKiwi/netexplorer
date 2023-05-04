cmake_minimum_required(VERSION 3.0)

project(inikiwi-netexplorer)

set(
    LUA_SRC
    src/lua/lapi.c
    src/lua/lcode.c
    src/lua/lctype.c
    src/lua/ldebug.c
    src/lua/ldo.c
    src/lua/ldump.c
    src/lua/lfunc.c
    src/lua/lgc.c
    src/lua/llex.c
    src/lua/lmem.c
    src/lua/lobject.c
    src/lua/lopcodes.c
    src/lua/lparser.c
    src/lua/lstate.c
    src/lua/lstring.c
    src/lua/ltable.c
    src/lua/ltm.c
    src/lua/lundump.c
    src/lua/lvm.c
    src/lua/lzio.c
    src/lua/lauxlib.c
    src/lua/lbaselib.c
    src/lua/lcorolib.c
    src/lua/ldblib.c
    src/lua/liolib.c
    src/lua/lmathlib.c
    src/lua/loadlib.c
    src/lua/loslib.c
    src/lua/lstrlib.c
    src/lua/ltablib.c
    src/lua/lutf8lib.c
    src/lua/linit.c
)

set(
    SRCS
    src/main.cpp
    src/util.cpp
    src/networktask.cpp
    src/logger.cpp
    src/protocol/http.cpp
    src/protocol/tcp.cpp
    src/parser/html.cpp
)

add_executable(inikiwi-netexplorer ${SRCS} ${LUA_SRC} ${HEADERS})
target_link_libraries(inikiwi-netexplorer PRIVATE sqlite3 pthread )
set(CMAKE_BUILD_TYPE Relase)