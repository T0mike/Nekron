cmake_minimum_required(VERSION 3.26)

# NOTE: update executable name in .github/workflows/cmake.yml:25 when changing executable name in this file
# for now, the project name is used as the executable name
set(MAIN_PROJECT_NAME "oop")
set(MAIN_EXECUTABLE_NAME "${MAIN_PROJECT_NAME}")


project(${MAIN_PROJECT_NAME})
# set(CMAKE_PROJECT_VERSION_MAJOR 0)
# set(CMAKE_PROJECT_VERSION_MINOR 0)
# set(CMAKE_PROJECT_VERSION_PATCH 1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/Options.cmake)
include(cmake/CompilerFlags.cmake)
include(cmake/CopyHelper.cmake)

###############################################################################

# external dependencies with FetchContent
option(USE_SFML "Build with SFML for graphics" OFF)

if(USE_SFML)
    include(FetchContent)

    set(FETCHCONTENT_QUIET OFF CACHE INTERNAL "Override verbose output" FORCE)
    set(FETCHCONTENT_UPDATES_DISCONNECTED ON CACHE INTERNAL "Override update strategy" FORCE)

    FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG        2.6.2
        GIT_SHALLOW    1
        GIT_PROGRESS   ON
    )

    FetchContent_MakeAvailable(SFML)
endif()

###############################################################################

# external dependencies with find_package

# find_package(Threads REQUIRED)

###############################################################################

# NOTE: update executable name in .github/workflows/cmake.yml:25 when changing name here
add_executable(${MAIN_EXECUTABLE_NAME}
    main.cpp
)
target_sources(${MAIN_EXECUTABLE_NAME}
    PRIVATE
        src/Weapon.cpp
        src/Player.cpp
        src/Zombie.cpp
        src/Game.cpp
        src/ZombieNormal.cpp
        src/ZombieTank.cpp
        src/ZombieBoss.cpp
        src/ZombieRunner.cpp
        src/ZombieSpitter.cpp
        src/ZombieFactory.cpp
        src/GameStatistics.cpp
)

# NOTE: Add all defined targets (e.g. executables, libraries, etc. )
# NOTE: RUN_SANITIZERS is optional, if it's not present it will default to true
set_compiler_flags(RUN_SANITIZERS ${USE_ASAN} TARGET_NAMES ${MAIN_EXECUTABLE_NAME})
# set_compiler_flags(TARGET_NAMES ${MAIN_EXECUTABLE_NAME} ${FOO} ${BAR})
# where ${FOO} and ${BAR} represent additional executables or libraries
# you want to compile with the set compiler flags

###############################################################################

# use SYSTEM so cppcheck and clang-tidy do not report warnings from these directories
target_include_directories(${MAIN_EXECUTABLE_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)
target_include_directories(${MAIN_EXECUTABLE_NAME} SYSTEM PRIVATE
    ext
)
# target_include_directories(${MAIN_EXECUTABLE_NAME} SYSTEM PRIVATE ${<SomeLib>_SOURCE_DIR}/include)
# target_link_directories(${MAIN_EXECUTABLE_NAME} PRIVATE ${<SomeLib>_BINARY_DIR}/lib)
if(USE_SFML)
    target_sources(${MAIN_EXECUTABLE_NAME}
        PRIVATE
            src/TextureManager.cpp
            src/CharacterSprite.cpp
            src/WorldRenderer.cpp
            src/Hud.cpp
            src/SoundManager.cpp
            src/GameApp.cpp
    )
    target_link_libraries(${MAIN_EXECUTABLE_NAME} sfml-graphics sfml-window sfml-system sfml-audio)
    target_compile_definitions(${MAIN_EXECUTABLE_NAME} PRIVATE USE_SFML)
    set_target_properties(${MAIN_EXECUTABLE_NAME} PROPERTIES OUTPUT_NAME "Nekron")
    if(WIN32)
        enable_language(RC)
        target_sources(${MAIN_EXECUTABLE_NAME} PRIVATE icon.rc)
        target_link_libraries(${MAIN_EXECUTABLE_NAME} sfml-main)
        set_target_properties(${MAIN_EXECUTABLE_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
        add_custom_command(TARGET ${MAIN_EXECUTABLE_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${sfml_SOURCE_DIR}/extlibs/bin/x64/openal32.dll
                $<TARGET_FILE_DIR:${MAIN_EXECUTABLE_NAME}>)
    endif()
endif()

###############################################################################

# copy binaries to "bin" folder; these are uploaded as artifacts on each release
# DESTINATION_DIR is set as "bin" in cmake/Options.cmake:6
install(TARGETS ${MAIN_EXECUTABLE_NAME} DESTINATION ${DESTINATION_DIR})
if(APPLE)
    install(FILES launcher.command DESTINATION ${DESTINATION_DIR})
endif()

copy_files(FILES tastatura.txt DIRECTORY assets COPY_TO_DESTINATION TARGET_NAME ${MAIN_EXECUTABLE_NAME})
# copy_files(FILES tastatura.txt config.json DIRECTORY images sounds COPY_TO_DESTINATION TARGET_NAME ${MAIN_EXECUTABLE_NAME})
# copy_files(DIRECTORY images sounds COPY_TO_DESTINATION TARGET_NAME ${MAIN_EXECUTABLE_NAME})
