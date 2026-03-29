# GTest Unit Test Setup Guide (for AI)

You are setting up a GTest unit test for an embedded firmware C++ project on PC.
Follow each step EXACTLY. Do NOT skip steps. Do NOT think creatively.

## BEFORE YOU START: Read the sample project

A working sample project exists separately. Ask the user for the path.
Example: the user might say "sample is at D:\claude\UnitTest" or "/home/user/UnitTest".
We will call this path `{SAMPLE}` below.

Read these files IN ORDER to understand the pattern:

1. `{SAMPLE}/production/MotorController.cpp` — test target (the code being tested)
2. `{SAMPLE}/production/MotorController.h` — test target header
3. `{SAMPLE}/production/SensorManager.h` — production dependency (heavy, HW-dependent)
4. `{SAMPLE}/test/fakes/SensorManager.h` — fake header (lightweight replacement)
5. `{SAMPLE}/test/fakes/SensorManager_fake.cpp` — fake implementation (Link Seam, returns mock)
6. `{SAMPLE}/test/mocks/MockSensorManager.h` — mock class (controls behavior in test)
7. `{SAMPLE}/test/test_MotorController.cpp` — actual test code
8. `{SAMPLE}/CMakeLists.txt` — build configuration (include path priority)

Read them in this order. Understand how each file relates to the others.
Then follow the steps below to create the same pattern for YOUR test target.

---

## STEP 0: Identify the test target

Find the `.cpp` file you need to test. Example: `MotorController.cpp`

List ALL `#include` lines from:
- The `.cpp` file
- The `.h` file (same name)

Separate them into two groups:
- **Standard library** (`<cstdint>`, `<string>`, etc.) → IGNORE. Do nothing.
- **Project headers** (`"SensorManager.h"`, `"HalGpio.h"`, etc.) → These are your targets.

---

## STEP 1: Create fake headers

For EACH project header from STEP 0, create a fake header in `test/fakes/`.

### RULE: Cut the include chain

DO NOT copy the original header as-is.
DO NOT copy the original header's `#include` lines.

Instead, write a minimal version:

```cpp
// test/fakes/SomeModule.h
#pragma once
#include <cstdint>   // standard library only, if needed

namespace SomeNamespace {

class SomeModule {
public:
    virtual ~SomeModule() = default;
    // ONLY the functions that the test target calls:
    virtual ReturnType FunctionName(ParamType param);
};

} // namespace SomeNamespace
```

### How to know what functions to include

Look at the test target `.cpp` file. Find every call to this class. Those functions go in the fake header. Nothing else.

### What if the fake header needs a type from another header?

Example: A function uses `GpioPin` type, which is defined in `HalTypes.h`.

You have two choices:
1. **Define the type directly in the fake header** (if used in only one place)
2. **Create another fake header** for that type (if used in multiple fake headers)

Then try to build. If there are more missing types, repeat this step.

### What about the test target's own header?

`MotorController.h` is the test target. Do NOT fake it. Use the original.
But if `MotorController.h` has `#include "SomeOtherThing.h"`, fake THAT.

---

## STEP 2: Create fake cpp files (Link Seam)

For EACH class that the test target CALLS functions on, you need a fake `.cpp` file.

### Check: Is it a Singleton? (has `GetInstance()`)

**YES → Use Singleton Template:**

```cpp
// test/fakes/SomeModule_fake.cpp
#include "SomeModule.h"
#include "MockSomeModule.h"

namespace SomeNamespace {

static MockSomeModule* g_mock = nullptr;

void SetMockSomeModule(MockSomeModule* mock) {
    g_mock = mock;
}

SomeModule* SomeModule::GetInstance() {
    return g_mock;
}

// Default implementations for all virtual functions:
ReturnType SomeModule::FunctionName(ParamType param) {
    (void)param;
    return DefaultValue;
}

} // namespace SomeNamespace
```

**NO → Use Non-Singleton Template:**

```cpp
// test/fakes/SomeModule_fake.cpp
#include "SomeModule.h"

namespace SomeNamespace {

// Default implementations only. No mock pointer needed.
ReturnType SomeModule::FunctionName(ParamType param) {
    (void)param;
    return DefaultValue;
}

} // namespace SomeNamespace
```

Non-singleton objects are injected directly in the test (e.g., `obj.SetGpio(&mockGpio)`).

---

## STEP 3: Create Mock classes

For EACH class you want to control in tests, create a Mock in `test/mocks/`.

```cpp
// test/mocks/MockSomeModule.h
#pragma once
#include "SomeModule.h"       // this picks up the FAKE header
#include <gmock/gmock.h>

namespace SomeNamespace {

class MockSomeModule : public SomeModule {
public:
    MockSomeModule() = default;

    MOCK_METHOD(ReturnType, FunctionName, (ParamType param), (override));
    // Add one MOCK_METHOD for each virtual function in the fake header.
    // If the function is const, add (const, override) instead of (override).
};

// ONLY for singletons:
void SetMockSomeModule(MockSomeModule* mock);

} // namespace SomeNamespace
```

---

## STEP 4: Configure CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.14)
project(YourTestProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Test executable
add_executable(your_test
    # 1. Test target (production cpp ONLY for the file under test)
    production/MotorController.cpp

    # 2. Fake implementations
    test/fakes/SensorManager_fake.cpp
    test/fakes/Logger_fake.cpp
    test/fakes/HalGpio_fake.cpp
    test/fakes/SomethingElse_fake.cpp

    # 3. Test code
    test/test_MotorController.cpp
)

# Include path priority (THIS ORDER MATTERS)
target_include_directories(your_test PRIVATE
    test/fakes       # 1st: fake headers override production headers
    test/mocks       # 2nd: mock classes
    production       # 3rd: test target header only
)

# MSVC UTF-8 support (for non-ASCII comments)
if(MSVC)
    target_compile_options(your_test PRIVATE /utf-8)
endif()

target_link_libraries(your_test
    GTest::gtest_main
    GTest::gmock
)

include(GoogleTest)
gtest_discover_tests(your_test)
```

### IMPORTANT RULES for CMakeLists.txt:
- `production/` cpp: ONLY the test target. NOT its dependencies.
- `test/fakes/` cpp: ALL fake implementations.
- Include order: `test/fakes` > `test/mocks` > `production`. NEVER change this order.

### Multiple test targets

If you need to test MORE THAN ONE production cpp file (e.g., MotorController.cpp AND SensorManager.cpp),
DO NOT put them in the same `add_executable`. Create SEPARATE executables for each.

The reason: if `SensorManager.cpp` is the test target, it cannot also have a fake (`SensorManager_fake.cpp`).
But `MotorController` tests need `SensorManager_fake.cpp`. They conflict.

```cmake
# --- Test 1: MotorController ---
add_executable(motor_controller_ut
    production/MotorController.cpp          # test target
    test/fakes/SensorManager_fake.cpp       # SensorManager is FAKE here
    test/fakes/Logger_fake.cpp
    test/test_MotorController.cpp
)
target_include_directories(motor_controller_ut PRIVATE
    test/fakes test/mocks production
)
target_link_libraries(motor_controller_ut GTest::gtest_main GTest::gmock)

# --- Test 2: SensorManager ---
add_executable(sensor_manager_ut
    production/SensorManager.cpp            # test target
    test/fakes/MotorController_fake.cpp     # MotorController is FAKE here
    test/fakes/Logger_fake.cpp
    test/test_SensorManager.cpp
)
target_include_directories(sensor_manager_ut PRIVATE
    test/fakes test/mocks production
)
target_link_libraries(sensor_manager_ut GTest::gtest_main GTest::gmock)
```

RULE: One `add_executable` per test target. The test target is REAL, everything else is FAKE.

---

## STEP 5: Write the test file

```cpp
// test/test_MotorController.cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MotorController.h"      // test target (from production/)
#include "MockSensorManager.h"    // mocks (from test/mocks/)
#include "MockLogger.h"
#include "MockHalGpio.h"

using namespace testing;
using namespace Firmware;

class MotorControllerTest : public Test {
protected:
    void SetUp() override {
        // Singletons: inject mock via SetMock function
        SetMockSensorManager(&mockSensor_);
        SetMockLogger(&mockLogger_);

        // Non-singletons: inject directly
        motor_.SetGpio(&mockGpio_);
    }

    void TearDown() override {
        // Singletons: clean up
        SetMockSensorManager(nullptr);
        SetMockLogger(nullptr);
    }

    NiceMock<MockSensorManager> mockSensor_;
    NiceMock<MockLogger> mockLogger_;
    NiceMock<Hal::MockHalGpio> mockGpio_;
    MotorController motor_;
};

TEST_F(MotorControllerTest, YourTestName) {
    // 1. Set expectations (what the mock should do)
    EXPECT_CALL(mockSensor_, IsCalibrated()).WillOnce(Return(true));

    // 2. Call the function under test
    MotorError result = motor_.RunSequence();

    // 3. Verify the result
    EXPECT_EQ(result, MotorError::NONE);
}
```

---

## STEP 6: Build and fix errors

Run the build. Then follow this loop:

```
LOOP:
  1. Build.
  2. If build succeeds → DONE. Go to STEP 7.
  3. If build fails → read the FIRST error message.

  Error: "cannot open include file 'XYZ.h'"
  → Create test/fakes/XYZ.h with minimal content. Go to 1.

  Error: "unresolved external symbol ... ClassName::FunctionName"  (LNK2001 / LNK2019)
  → Add that function to the fake .cpp file. Go to 1.

  Error: "unknown type name 'SomeType'"
  → Add SomeType definition to the relevant fake header. Go to 1.

  Error: "syntax error" with C4819 warning about codepage
  → Add /utf-8 compile option in CMakeLists.txt. Go to 1.

  Error: something else
  → Read the error carefully. It is probably a missing type, missing include,
    or missing function body. Fix the ONE thing the error says. Go to 1.

DO NOT try to fix multiple errors at once.
Fix the FIRST error, rebuild, then fix the next.
```

---

## STEP 7: Run the test

```bash
cd build
ctest --output-on-failure -C Debug
```

If all tests pass, you are done.

---

## Quick Reference: File Structure

```
project/
  production/
    MotorController.h          ← test target header (USE AS-IS)
    MotorController.cpp        ← test target source (USE AS-IS)
    SensorManager.h            ← dependency (DO NOT compile)
    Logger.h                   ← dependency (DO NOT compile)
  test/
    fakes/
      SensorManager.h          ← fake header (lightweight replacement)
      SensorManager_fake.cpp   ← fake impl (returns mock pointer)
      Logger.h                 ← fake header
      Logger_fake.cpp          ← fake impl
      HalGpio.h                ← fake header
      HalGpio_fake.cpp         ← fake impl
      HalTypes.h               ← fake header (needed by HalGpio.h)
    mocks/
      MockSensorManager.h      ← mock class
      MockLogger.h             ← mock class
      MockHalGpio.h            ← mock class
    test_MotorController.cpp   ← test code
  CMakeLists.txt
```

---

## Summary (Decision Flowchart)

```
For each #include in the test target:
  |
  Is it a standard library? (<cstdint>, <string>, etc.)
  YES → skip
  NO  ↓
  |
  Create test/fakes/ThatHeader.h
    - #pragma once
    - only standard library includes
    - only types/functions the test target actually uses
    - this CUTS the include chain
  |
  Does the test target call functions on this class?
  NO  → done for this header
  YES ↓
  |
  Is it a singleton? (has GetInstance())
  YES → create fake cpp with mock pointer + SetMock function
      → create mock class in test/mocks/
  NO  → create fake cpp with default implementations only
      → create mock class in test/mocks/
      → inject mock directly in test SetUp()
  |
  Build. Fix errors one by one. Repeat.
```
