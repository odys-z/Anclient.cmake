
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.15.2
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_executable(anclient_tests
    t02_anclient_tests.cpp
    t01_common.cpp
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
        nlohmann_json::nlohmann_json
        cpr::cpr
        Botan::botan
        anson
        anclient
        gtest_main
)

target_compile_options(anclient_tests PRIVATE /showIncludes)

add_compile_definitions(ANSON_VERBOSE=4)

if(MSVC)
    target_compile_options(anclient_tests PRIVATE "/bigobj")
endif()
