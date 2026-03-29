#include "SensorManager.h"
#include "MockSensorManager.h"

namespace Firmware {

static MockSensorManager* g_mockSensorManager = nullptr;

void SetMockSensorManager(MockSensorManager* mock) {
    g_mockSensorManager = mock;
}

// ★ 포인터 반환: Mock 객체의 주소를 그대로 돌려줌
SensorManager* SensorManager::GetInstance() {
    return g_mockSensorManager;
}

int32_t SensorManager::ReadTemperature() { return 0; }
int32_t SensorManager::ReadPressure() { return 0; }
bool SensorManager::IsCalibrated() { return false; }
void SensorManager::TriggerMeasurement() {}

} // namespace Firmware
