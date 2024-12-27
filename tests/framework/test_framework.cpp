#include "test_framework.h"
#include <iostream>
#include <vector>
#include <exception>

TestCase::TestCase(const std::string& name, std::function<void()> testFn)
    : name(name), testFn(testFn) {}

void TestCase::run() {
    try {
        testFn();
        result = true;
    } catch (const std::exception& e) {
        result = false;
        errorMessage = e.what();
    }
}

bool TestCase::passed() const {
    return result;
}

std::string TestCase::getName() const {
    return name;
}

std::string TestCase::getErrorMessage() const {
    return errorMessage;
}

TestRegistry& TestRegistry::instance() {
    static TestRegistry registry;
    return registry;
}

void TestRegistry::addTest(const TestCase& test) {
    tests.push_back(test);
}

void TestRegistry::runAll() {
    int passedTests = 0;
    int totalTests = tests.size();

    for (auto& test : tests) {
        test.run();
        if (test.passed()) {
            ++passedTests;
            std::cout << test.getName() << ": PASSED\n";
        } else {
            std::cout << test.getName() << ": FAILED - " << test.getErrorMessage() << "\n";
        }
    }

    std::cout << "\nSummary: " << passedTests << " / " << totalTests << " tests passed.\n";
}

TestRegistrar::TestRegistrar(const std::string& name, std::function<void()> fn) {
    TestRegistry::instance().addTest(TestCase(name, fn));
}
