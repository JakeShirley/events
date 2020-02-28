/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#include "MinecraftEventing.h"

#include "SimpleEventBus.h"

MinecraftEventing::MinecraftEventing() 
	: mEventBus(std::make_unique<SimpleEventBus<IMinecraftEvent>>()) {

}

MinecraftEventing::~MinecraftEventing() = default;
