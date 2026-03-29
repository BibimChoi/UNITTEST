#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "MotorController.h"
#include "MockSensorManager.h"
#include "MockLogger.h"
#include "MockHalGpio.h"

using namespace testing;
using namespace Firmware;

class MotorControllerTest : public Test {
protected:
    void SetUp() override {
        // GetInstance()가 Mock 포인터를 반환하도록 세팅
        SetMockSensorManager(&mockSensor_);
        SetMockLogger(&mockLogger_);
        motor_.SetGpio(&mockGpio_);
    }

    void TearDown() override {
        SetMockSensorManager(nullptr);
        SetMockLogger(nullptr);
    }

    NiceMock<MockSensorManager> mockSensor_;
    NiceMock<MockLogger> mockLogger_;
    NiceMock<Hal::MockHalGpio> mockGpio_;
    MotorController motor_;
};

// =============================================================
// EmergencyStop 테스트
// =============================================================

// ★ 정상 케이스: Logger::GetInstance()->GetErrorCount()가 증가하면 NONE 반환
TEST_F(MotorControllerTest, EmergencyStop_LogSuccess_ReturnsNone) {
    // Logger::GetInstance()->GetErrorCount() 가 0 → 1로 증가
    EXPECT_CALL(mockLogger_, GetErrorCount())
        .WillOnce(Return(0))     // prevErrors = 0
        .WillOnce(Return(1));    // currErrors = 1 (증가함 → 로그 성공)
    EXPECT_CALL(mockLogger_, LogError(_)).Times(1);

    MotorError result = motor_.EmergencyStop();

    // ★ Mock 반환값에 의해 결과가 결정됨
    EXPECT_EQ(result, MotorError::NONE);
    EXPECT_EQ(motor_.GetState(), MotorState::STOPPED);
}

// ★ 로그 실패 케이스: GetErrorCount()가 안 올라가면 LOG_FAIL 반환
TEST_F(MotorControllerTest, EmergencyStop_LogFail_ReturnsLogFail) {
    // Logger::GetInstance()->GetErrorCount() 가 변하지 않음 → 로그 실패
    EXPECT_CALL(mockLogger_, GetErrorCount())
        .WillOnce(Return(5))     // prevErrors = 5
        .WillOnce(Return(5));    // currErrors = 5 (안 올라감 → 로그 실패)
    EXPECT_CALL(mockLogger_, LogError(_)).Times(1);

    MotorError result = motor_.EmergencyStop();

    // ★ Mock이 에러카운트 안 올리니까 LOG_FAIL
    EXPECT_EQ(result, MotorError::LOG_FAIL);
    EXPECT_EQ(motor_.GetState(), MotorState::STOPPED);
}

// ★ GPIO 동작 검증: 모터 정지 명령이 나가는지
TEST_F(MotorControllerTest, EmergencyStop_StopsMotorGpio) {
    EXPECT_CALL(mockLogger_, GetErrorCount())
        .WillOnce(Return(0))
        .WillOnce(Return(1));
    EXPECT_CALL(mockLogger_, LogError(HasSubstr("EMERGENCY"))).Times(1);

    // ★ GPIO로 모터 OFF 신호가 나가는지 검증
    EXPECT_CALL(mockGpio_, SetPin(Hal::GpioPin::MOTOR_EN, Hal::GpioState::LOW))
        .Times(1);

    motor_.EmergencyStop();
}

// =============================================================
// RunSequence 테스트
// =============================================================

// ★ 정상: SensorManager::GetInstance()->IsCalibrated() true,
//         SensorManager::GetInstance()->ReadTemperature() 정상 → NONE
TEST_F(MotorControllerTest, RunSequence_NormalCondition_ReturnsNone) {
    EXPECT_CALL(mockSensor_, IsCalibrated()).WillOnce(Return(true));
    EXPECT_CALL(mockSensor_, ReadTemperature()).WillOnce(Return(25));
    EXPECT_CALL(mockLogger_, LogInfo(_)).Times(1);

    MotorError result = motor_.RunSequence();

    EXPECT_EQ(result, MotorError::NONE);
    EXPECT_EQ(motor_.GetState(), MotorState::RUNNING_FWD);
}

// ★ 캘리브레이션 실패: Mock이 false 반환 → NOT_CALIBRATED
TEST_F(MotorControllerTest, RunSequence_NotCalibrated_ReturnsNotCalibrated) {
    // SensorManager::GetInstance()->IsCalibrated() 가 false
    EXPECT_CALL(mockSensor_, IsCalibrated()).WillOnce(Return(false));
    EXPECT_CALL(mockLogger_, LogError(_)).Times(1);

    // ReadTemperature는 호출되면 안 됨 (일찍 리턴하니까)
    EXPECT_CALL(mockSensor_, ReadTemperature()).Times(0);

    MotorError result = motor_.RunSequence();

    EXPECT_EQ(result, MotorError::NOT_CALIBRATED);
    EXPECT_EQ(motor_.GetState(), MotorState::ERROR);
}

// ★ 과온: Mock이 100도 반환 → OVER_TEMP
TEST_F(MotorControllerTest, RunSequence_OverTemp_ReturnsOverTemp) {
    EXPECT_CALL(mockSensor_, IsCalibrated()).WillOnce(Return(true));
    // SensorManager::GetInstance()->ReadTemperature() 가 100 반환
    EXPECT_CALL(mockSensor_, ReadTemperature()).WillOnce(Return(100));
    EXPECT_CALL(mockLogger_, LogError(_)).Times(1);

    MotorError result = motor_.RunSequence();

    EXPECT_EQ(result, MotorError::OVER_TEMP);
    EXPECT_EQ(motor_.GetState(), MotorState::ERROR);
}

// ★ 경계값: 정확히 80도 → 통과 (80 > 80 = false)
TEST_F(MotorControllerTest, RunSequence_ExactMaxTemp_ReturnsNone) {
    EXPECT_CALL(mockSensor_, IsCalibrated()).WillOnce(Return(true));
    EXPECT_CALL(mockSensor_, ReadTemperature()).WillOnce(Return(80));

    MotorError result = motor_.RunSequence();

    EXPECT_EQ(result, MotorError::NONE);
}

// ★ SetGpio 검증
TEST_F(MotorControllerTest, SetGpio_Null_ReturnsFalse) {
    MotorController fresh;
    bool result = fresh.SetGpio(nullptr);
    EXPECT_FALSE(result);
}

TEST_F(MotorControllerTest, SetGpio_Valid_ReturnsTrue) {
    MotorController fresh;
    Hal::MockHalGpio gpio;
    EXPECT_CALL(gpio, ConfigureOutput(Hal::GpioPin::MOTOR_EN)).Times(1);
    EXPECT_CALL(gpio, ConfigureOutput(Hal::GpioPin::MOTOR_DIR)).Times(1);

    bool result = fresh.SetGpio(&gpio);
    EXPECT_TRUE(result);
}
