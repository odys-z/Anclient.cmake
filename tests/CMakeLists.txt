
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.15.2
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_executable(anclient_tests
    # t01_common.cpp
    t02_anclient_session.cpp
    t02_doclientier.cpp
    expect/doctier.hpp
)

add_custom_command(TARGET anclient_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/ast-cpy"
        "$<TARGET_FILE_DIR:anclient_tests>/ast-cpy"

    COMMENT "Copying JSON settings to build directory"
)

find_package(nlohmann_json CONFIG REQUIRED)

target_link_libraries(anclient_tests
    PRIVATE 
        OpenSSL::Crypto OpenSSL::SSL
        nlohmann_json::nlohmann_json
        cpr::cpr
        anson
        anclient
        gtest_main
)

target_compile_options(anclient_tests PRIVATE /showIncludes)

add_compile_definitions(ANSON_VERBOSE=4)

if(MSVC)
    target_compile_options(anclient_tests PRIVATE "/bigobj")
endif()
