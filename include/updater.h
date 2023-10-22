#ifndef UPDATER_H
#define UPDATER_H

#include <cstdint>
#include "esp_ota_ops.h"
#include "fabgl.h"

void receiveFirmware(fabgl::Terminal *terminal, uint32_t update_size);

#endif