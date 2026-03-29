cmake_minimum_required(VERSION 3.14)
project(MotorControllerUT LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- GoogleTest 가져오기 (FetchContent) ----
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
# Windows에서 gtest와 CRT 충돌 방지
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ---- 테스트 실행 파일 ----
add_executable(motor_controller_ut
    # ★ 테스트 대상: 프로덕션 소스는 이것만 컴파일!
    production/MotorController.cpp

    # ★ Fake 구현체: Singleton GetInstance()를 Mock으로 대체
    test/fakes/SensorManager_fake.cpp
    test/fakes/Logger_fake.cpp
    test/fakes/HalGpio_fake.cpp
    test/fakes/SomethingElse_fake.cpp

    # ★ 테스트 코드
    test/test_MotorController.cpp
)

# ★★★ 핵심: include 경로 우선순위 ★★★
# test/fakes 가 먼저 → SensorManager.h, Logger.h, HalGpio.h 등이
#                       프로덕션 대신 가벼운 fake 헤더로 잡힘
# test/mocks 가 그 다음 → MockXxx.h 들
# production 은 마지막 → MotorController.h (테스트 대상만 여기서 가져감)
target_include_directories(motor_controller_ut PRIVATE
    test/fakes       # ← 1순위: fake 헤더 (의존성 체인 끊기)
    test/mocks       # ← 2순위: mock 클래스
    production       # ← 3순위: 테스트 대상 헤더
)

# MSVC에서 UTF-8 소스 파일 인코딩 강제
if(MSVC)
    target_compile_options(motor_controller_ut PRIVATE /utf-8)
endif()

target_link_libraries(motor_controller_ut
    GTest::gtest_main
    GTest::gmock
)

# CTest 연동
include(GoogleTest)
gtest_discover_tests(motor_controller_ut)
