cmake_minimum_required(VERSION 3.28)
set(PROJECT_NAME E-mess)
# Version: major.minor.maintenance
project(${PROJECT_NAME} VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)

option(BUILD_TESTS "Build tests" ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
cmake_policy(SET CMP0167 NEW)
find_package(Boost 1.86.0 REQUIRED system)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Boost not found!")
endif()

if(BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(googletest)

    enable_testing()
    include(GoogleTest)
endif()

set(NON_EDITABLE_SOURSES
    main.cpp
)

set(UI_SOURCES
    widget.ui
)

set(PROJECT_SOURCES
    widget.h                                widget.cpp
    sidebar/sidebar.h                       sidebar/sidebar.cpp
    chats/chats_common.h                    chats/chats_common.cpp
    chats/user_message_box.h                chats/user_message_box.cpp
    notifications/notification_manager.h    notifications/notification_manager.cpp
    settings/settings.h                     settings/settings.cpp
    utils/own_utils.h                       utils/own_utils.cpp
    utils/logger.h                          utils/logger.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${PROJECT_NAME}
        MANUAL_FINALIZATION
        ${NON_EDITABLE_SOURSES}
        ${PROJECT_SOURCES}
    )
else()
    add_executable(${PROJECT_NAME}
        ${NON_EDITABLE_SOURSES}
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    ${Boost_LIBRARIES}
)

if(BUILD_TESTS)
    set(LIB_SOURCES
        ${UI_SOURCES}
        ${PROJECT_SOURCES}
    )

    add_library(${PROJECT_NAME}_lib STATIC ${LIB_SOURCES})
    target_link_libraries(${PROJECT_NAME}_lib PUBLIC
        Qt${QT_VERSION_MAJOR}::Widgets
        ${Boost_LIBRARIES}
    )

    set(TEST_SOURCES
        tests/main_tests.cpp
        tests/settings/settings_tests.cpp
    )

    add_executable(${PROJECT_NAME}_tests
        ${TEST_SOURCES}
    )

    target_compile_definitions(${PROJECT_NAME}_tests PRIVATE
        TESTS_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests"
    )

    target_link_libraries(${PROJECT_NAME}_tests PRIVATE
        ${PROJECT_NAME}_lib
        GTest::gtest
        GTest::gmock
        Qt${QT_VERSION_MAJOR}::Widgets
    )

    gtest_discover_tests(${PROJECT_NAME}_tests)

    add_custom_target(run_tests
        COMMAND ${PROJECT_NAME}_tests
        DEPENDS ${PROJECT_NAME}_tests
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running tests..."
    )
endif()

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.${PROJECT_NAME})
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${PROJECT_NAME})
endif()
