# Anclient.cmake source, pure header files

cmake_minimum_required(VERSION 3.30)
# project(anclient LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(anclient INTERFACE
    io/odysz/gen/doctier.hpp
    io/odysz/clients.h
    io/odysz/jclient/syn.h
    io/odysz/semantic/tier/docs.h
    io/oz/anclient/socketier.h
)

add_library(anclient::anclient ALIAS anclient)

# 1. Register the Header Set
target_sources(anclient INTERFACE
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        io/odysz/anson.h
)

target_include_directories(anson INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(anson INTERFACE
    OpenSSL::Crypto OpenSSL::SSL
    EnTT::EnTT
    nlohmann_json::nlohmann_json
    cpr::cpr
    anson
)

# clangd helper, see also .clangd/index
if(ANCLIENT_BUILD_TESTS)
    # set(ANSON_BUILD_TESTS ON) not working
    add_executable(
        anclient_clangd_helper
        dev/ping.cpp
        io/odysz/jclient/syn.h
    )
    target_link_libraries(anclient_clangd_helper PRIVATE anson)
endif()
