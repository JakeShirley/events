/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include <memory>

#include "Publisher.h"
#include "Subscription.h"

struct IEvent {
	virtual ~IEvent() = default;
};

template <typename T>
class IEventBus;

template <typename T, typename U>
class Subscription;

using IMinecraftEvent = IEvent;
using MinecraftEventBus = IEventBus<IMinecraftEvent>;

template <typename SubscriberT>
using MinecraftEventSubscription = Subscription<MinecraftEventBus, SubscriberT>;

template <typename ... EventTypeTs>
using MinecraftEventPublisher = Publisher< MinecraftEventBus, IMinecraftEvent, EventTypeTs...>;

class MinecraftEventing {
public:
	MinecraftEventing();
	~MinecraftEventing();

	template <typename SubscriberT>
	MinecraftEventSubscription<SubscriberT> make_subscription(SubscriberT* subscriber) const {
		return MinecraftEventSubscription<SubscriberT>(subscriber, *mEventBus);
	}

	template <typename ... EventTypeTs>
	MinecraftEventPublisher<EventTypeTs ...> make_publisher() const {
		return MinecraftEventPublisher<EventTypeTs ...>(*mEventBus);
	}
	
private:
	std::unique_ptr<MinecraftEventBus> mEventBus;

};