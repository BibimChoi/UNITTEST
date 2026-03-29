## ============================================================
##  MotorController Unit Test - Makefile (MinGW GCC on Windows)
## ============================================================

CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -g -O0

# ---- GoogleTest 경로 ----
GTEST_DIR := third_party/googletest/googletest
GMOCK_DIR := third_party/googletest/googlemock

# ---- Include 경로 (★ 순서가 핵심!) ----
#  1순위: test/fakes   → 무거운 프로덕션 헤더 대신 가벼운 fake 헤더
#  2순위: test/mocks   → MockXxx.h
#  3순위: production   → 테스트 대상 헤더 (MotorController.h)
#  4순위: GTest/GMock
INCLUDES := \
	-Itest/fakes \
	-Itest/mocks \
	-Iproduction \
	-I$(GTEST_DIR)/include \
	-I$(GMOCK_DIR)/include \
	-I$(GTEST_DIR) \
	-I$(GMOCK_DIR)

# ---- 소스 파일 ----

# 테스트 대상: 프로덕션 소스는 이것만!
PROD_SRC := production/MotorController.cpp

# Fake 구현체: Singleton GetInstance()를 Mock으로 대체
FAKE_SRC := \
	test/fakes/SensorManager_fake.cpp \
	test/fakes/Logger_fake.cpp \
	test/fakes/HalGpio_fake.cpp

# 테스트 코드
TEST_SRC := test/test_MotorController.cpp

# GTest/GMock 소스 (라이브러리 대신 직접 컴파일)
GTEST_SRC := \
	$(GTEST_DIR)/src/gtest-all.cc \
	$(GMOCK_DIR)/src/gmock-all.cc \
	$(GTEST_DIR)/src/gtest_main.cc

# ---- 빌드 출력 ----
BUILD_DIR := build
TARGET    := $(BUILD_DIR)/motor_controller_ut.exe

# Object 파일
PROD_OBJ  := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(PROD_SRC))
FAKE_OBJ  := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(FAKE_SRC))
TEST_OBJ  := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(TEST_SRC))
GTEST_OBJ := $(patsubst %.cc,$(BUILD_DIR)/%.o,$(GTEST_SRC))

ALL_OBJ   := $(PROD_OBJ) $(FAKE_OBJ) $(TEST_OBJ) $(GTEST_OBJ)

# ============================================================
#  Rules
# ============================================================

.PHONY: all test clean

all: $(TARGET)

test: $(TARGET)
	@echo ""
	@echo "======== Running Unit Tests ========"
	@./$(TARGET) --gtest_color=yes
	@echo "===================================="

$(TARGET): $(ALL_OBJ)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread

# .cpp → .o
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# .cc → .o (GTest/GMock 소스)
$(BUILD_DIR)/%.o: %.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
