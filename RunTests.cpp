#include "test/TestRunner.h"
#include "test/TankCollisionTest.h"
#include "test/ShellCollisionTest.h"
#include <iostream>

int main() {
    TestRunner runner;

    // Add all test setups
    setupTankCollisionTest(runner);
    setupShellCollisionTest(runner);
    // Add more test setups here...

    // Run all tests
    runner.runAllTests();
    
    // Print results
    runner.printResults();

    return 0;
} 