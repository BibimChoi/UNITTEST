#include "SomethingElse.h"

namespace Firmware {

bool SomethingElse::DoWork(int32_t param) { (void)param; return true; }
int32_t SomethingElse::GetStatus() const { return 0; }

} // namespace Firmware
