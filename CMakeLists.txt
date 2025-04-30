cmake_minimum_required(VERSION 3.10)
project(tanks_game LANGUAGES CXX)

# ——————————————————————————————————————————————
# 1. C++ Standard & Compiler Settings
# ——————————————————————————————————————————————
set(CMAKE_CXX_STANDARD        20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS      OFF)

# Enforce exact compiler flags for g++
# This will apply to all targets
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()

# ——————————————————————————————————————————————
# 2. Build Options
# ——————————————————————————————————————————————
option(BUILD_TESTING "Enable unit tests" ON)

# ——————————————————————————————————————————————
# 3. Source Files & Includes
# ——————————————————————————————————————————————
set(GAME_SOURCES
    Main.cpp
    game_objects/Tank.cpp
    game_objects/Shell.cpp
    game_objects/Moveable.cpp
    game_objects/Direction.cpp
    board/GameBoard.cpp
    player/Player.cpp
    constants/boardconstants.cpp
    algorithms/PathFinder.cpp
    algorithms/algorithmPlayerTwo.cpp
    file_handlers/InputHandler.cpp
)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/game_objects
    ${CMAKE_SOURCE_DIR}/board
    ${CMAKE_SOURCE_DIR}/player
    ${CMAKE_SOURCE_DIR}/constants
    ${CMAKE_SOURCE_DIR}/algorithms
    ${CMAKE_SOURCE_DIR}/file_handlers
)

# ——————————————————————————————————————————————
# 4. Main Executable
# ——————————————————————————————————————————————
add_executable(tanks_game ${GAME_SOURCES})

target_include_directories(tanks_game PRIVATE ${CMAKE_SOURCE_DIR})

# ——————————————————————————————————————————————
# 5. Testing (GoogleTest via FetchContent)
# ——————————————————————————————————————————————
if(BUILD_TESTING)
  enable_testing()
  include(FetchContent)

  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
  )
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  set(TEST_SOURCES
      test/RunTests.cpp
      test/TestRunner.cpp
      test/TankCollisionTest.cpp
      test/ShellCollisionTest.cpp
      test/GameTest.cpp
  )

  add_executable(RunTests ${TEST_SOURCES} ${GAME_SOURCES})
  target_link_libraries(RunTests PRIVATE gtest_main)

  add_test(NAME AllUnitTests COMMAND RunTests)
endif()

# ——————————————————————————————————————————————
# 6. Installation (optional)
# ——————————————————————————————————————————————
install(TARGETS tanks_game RUNTIME DESTINATION bin)
if(BUILD_TESTING)
  install(TARGETS RunTests RUNTIME DESTINATION bin/tests)
endif()