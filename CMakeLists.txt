cmake_minimum_required(VERSION 3.10)
project(TankGame)

# Set C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
set(GAME_SOURCES
    game_objects/Tank.cpp
    game_objects/Shell.cpp
    game_objects/Moveable.cpp
    game_objects/Direction.cpp
    board/GameBoard.cpp
    player/Player.cpp
    constants/boardconstants.cpp
)

# Add test sources
set(TEST_SOURCES
    RunTests.cpp
    test/TestRunner.cpp
    test/TankCollisionTest.cpp
    test/ShellCollisionTest.cpp
    test/GameTest.cpp
)

# Create test executable
add_executable(RunTests ${TEST_SOURCES} ${GAME_SOURCES})

# Add include directories
target_include_directories(RunTests PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/game_objects
    ${CMAKE_SOURCE_DIR}/board
    ${CMAKE_SOURCE_DIR}/player
    ${CMAKE_SOURCE_DIR}/constants
    ${CMAKE_SOURCE_DIR}/test
)

# Add compiler flags if needed
if(MSVC)
    target_compile_options(RunTests PRIVATE /W4)
else()
    target_compile_options(RunTests PRIVATE -Wall -Wextra)
endif() 