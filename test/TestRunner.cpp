#include "TestRunner.h"
#include <iostream>

TestRunner::TestRunner() : passedTests(0), failedTests(0) {}

void TestRunner::addTest(const GameTest& test) {
    tests.push_back(test);
}

void TestRunner::runAllTests() {
    std::cout << "Running " << tests.size() << " tests..." << std::endl;
    
    for (const auto& test : tests) {
        if (test.execute()) {
            passedTests++;
        } else {
            failedTests++;
        }
    }
}

void TestRunner::printResults() const {
    std::cout << "\nTest Results:" << std::endl;
    std::cout << "Total tests: " << (passedTests + failedTests) << std::endl;
    std::cout << "Passed: " << passedTests << std::endl;
    std::cout << "Failed: " << failedTests << std::endl;
    
    if (failedTests == 0) {
        std::cout << "\nAll tests passed successfully!" << std::endl;
    } else {
        std::cout << "\nSome tests failed. Check the output above for details." << std::endl;
    }
} 