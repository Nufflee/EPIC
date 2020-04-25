#pragma once

#include <stddef.h>

void out8(u16 port, u8 value);
u8 in8(u16 port);
u16 in16(u16 port);