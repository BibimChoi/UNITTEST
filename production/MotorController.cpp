#include "MotorController.h"
#include "SensorManager.h"    // Singleton 의존성 #1
#include "Logger.h"           // Singleton 의존성 #2
#include "HalGpio.h"          // HW 의존성
#include "SomethingElse.h"

namespace Firmware {

MotorController::MotorController()
    : gpio_(nullptr)
    , something_(new SomethingElse())
    , state_(MotorState::STOPPED)
    , maxTemp_(80)
{
}

MotorError MotorController::RunSequence() {
    // ★ 포인터로 GetInstance() 호출
    SensorManager* pSensor = SensorManager::GetInstance();
    Logger* pLogger = Logger::GetInstance();

    if (pSensor == nullptr || pLogger == nullptr) {
        state_ = MotorState::ERROR;
        return MotorError::SENSOR_FAIL;
    }

    // 1) 캘리브레이션 체크
    if (!pSensor->IsCalibrated()) {
        pLogger->LogError("Motor: sensor not calibrated");
        state_ = MotorState::ERROR;
        return MotorError::NOT_CALIBRATED;
    }

    // 2) 온도 체크
    int32_t temp = pSensor->ReadTemperature();
    if (temp > maxTemp_) {
        pLogger->LogError("Motor: over temperature");
        StopMotor();
        state_ = MotorState::ERROR;
        return MotorError::OVER_TEMP;
    }

    // 3) 정상 시퀀스
    pLogger->LogInfo("Motor: starting sequence");
    StartMotor(true);
    state_ = MotorState::RUNNING_FWD;

    return MotorError::NONE;
}

MotorError MotorController::EmergencyStop() {
    // ★ 포인터로 GetInstance() 호출
    Logger* pLogger = Logger::GetInstance();

    if (pLogger == nullptr) {
        StopMotor();
        state_ = MotorState::STOPPED;
        return MotorError::LOG_FAIL;
    }

    // 에러 카운트 확인 후 로그
    uint32_t prevErrors = pLogger->GetErrorCount();
    pLogger->LogError("Motor: EMERGENCY STOP");
    uint32_t currErrors = pLogger->GetErrorCount();

    StopMotor();
    state_ = MotorState::STOPPED;

    // 로그가 제대로 기록됐는지 확인
    if (currErrors <= prevErrors) {
        return MotorError::LOG_FAIL;
    }

    // SomethingElse로 최종 상태 보고
    if (something_ && !something_->DoWork(0)) {
        return MotorError::LOG_FAIL;
    }

    return MotorError::NONE;
}

MotorState MotorController::GetState() const {
    return state_;
}

bool MotorController::SetGpio(Hal::HalGpio* gpio) {
    if (gpio == nullptr) {
        return false;
    }
    gpio_ = gpio;
    gpio_->ConfigureOutput(Hal::GpioPin::MOTOR_EN);
    gpio_->ConfigureOutput(Hal::GpioPin::MOTOR_DIR);
    return true;
}

void MotorController::StartMotor(bool forward) {
    if (gpio_) {
        gpio_->SetPin(Hal::GpioPin::MOTOR_DIR,
                       forward ? Hal::GpioState::HIGH : Hal::GpioState::LOW);
        gpio_->SetPin(Hal::GpioPin::MOTOR_EN, Hal::GpioState::HIGH);
    }
}

void MotorController::StopMotor() {
    if (gpio_) {
        gpio_->SetPin(Hal::GpioPin::MOTOR_EN, Hal::GpioState::LOW);
    }
}

} // namespace Firmware
