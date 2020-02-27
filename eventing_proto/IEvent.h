#pragma once

#include <inttypes.h>

using EventId = uint16_t;

struct IEvent {
	virtual ~IEvent() = default;
};