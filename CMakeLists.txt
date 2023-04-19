cmake_minimum_required(VERSION 3.0)

project(inikiwi-netexplorer)

set(
    SRCS
    src/main.cpp
    src/util.cpp
    src/networktask.cpp
    src/logger.cpp
)

add_executable(inikiwi-netexplorer ${SRCS} ${HEADERS})
target_link_libraries(inikiwi-netexplorer PRIVATE sqlite3 pthread )