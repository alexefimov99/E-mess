cmake_minimum_required(VERSION 3.30)
project(E-mess VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Widgets
    Network
    # Sql
)

cmake_policy(SET CMP0167 NEW)

find_package(Boost 1.89.0 REQUIRED)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found! Install: sudo apt install libboost-all-dev")
endif()

set(PROJECT_SOURCES
    src/main.cpp
    src/ui/mainwindow/widget.ui

    src/ui/mainwindow/widget.cpp
    src/ui/mainwindow/widget.h

    src/ui/sidebar/sidebar.cpp
    src/ui/sidebar/sidebar.h

    src/ui/chats/chats_common.cpp
    src/ui/chats/chats_common.h
    src/ui/chats/user_message_box.cpp
    src/ui/chats/user_message_box.h

    src/core/network/networkconnection.cpp
    src/core/network/networkconnection.h

    src/core/notifications/notification_manager.cpp
    src/core/notifications/notification_manager.h

    src/core/settings/settings.cpp
    src/core/settings/settings.h

    src/utils/own_utils.cpp
    src/utils/own_utils.h
    src/utils/logger.cpp
    src/utils/logger.h
)

qt_add_executable(${PROJECT_NAME}
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${Boost_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Network
    ${Boost_LIBRARIES}
)

option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.15.2
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_library(${PROJECT_NAME}_lib STATIC ${PROJECT_SOURCES})

    target_link_libraries(${PROJECT_NAME}_lib PUBLIC
        Qt6::Widgets
        Qt6::Network
        ${Boost_LIBRARIES}
    )

    target_include_directories(${PROJECT_NAME}_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${Boost_INCLUDE_DIRS}
    )

    set(TEST_SOURCES
        tests/main_tests.cpp
        tests/settings/settings_tests.cpp
    )

    add_executable(${PROJECT_NAME}_tests ${TEST_SOURCES})

    target_compile_definitions(${PROJECT_NAME}_tests PRIVATE
        TESTS_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests"
    )

    target_link_libraries(${PROJECT_NAME}_tests PRIVATE
        ${PROJECT_NAME}_lib
        GTest::gtest_main
        GTest::gmock
    )

    include(GoogleTest)
    gtest_discover_tests(${PROJECT_NAME}_tests)

    add_custom_target(run_tests
        COMMAND ${PROJECT_NAME}_tests
        DEPENDS ${PROJECT_NAME}_tests
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running all tests..."
        USES_TERMINAL
    )
endif()

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()

message(STATUS "Qt Version: ${Qt6_VERSION}")
