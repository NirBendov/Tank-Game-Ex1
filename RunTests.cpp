#include "test/TestRunner.h"
#include "test/TankCollisionTest.h"
#include "test/ShellCollisionTest.h"
#include "test/MineInteractionTest.h"
#include "test/WallInteractionTest.h"
#include <iostream>

int main() {
    TestRunner runner;

    // Add all test setups
    setupTankCollisionTest(runner);
    setupShellCollisionTest(runner);
    setupMineInteractionTest(runner);
    setupWallInteractionTest(runner);

    // Run all tests
    runner.runAllTests();
    
    // Print results
    runner.printResults();

    return 0;
} 