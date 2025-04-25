#pragma once
#include <vector>
#include <string>
#include "GameTest.h"

class TestRunner {
private:
    std::vector<GameTest> tests;
    int passedTests;
    int failedTests;

public:
    TestRunner();
    void addTest(const GameTest& test);
    void runAllTests();
    void printResults() const;
}; 