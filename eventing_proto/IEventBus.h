#pragma once

#include "PointerHolder.h"
#include "SubscriberCaller.h"
#include "TypeId.h"
#include <memory>

using EventId = uint16_t;

template <typename EventBusT, typename SubscriberT>
class Subscription;

template <typename T>
class IEventBus {
public:
	using BaseEventT = T;
	using SubscriberCallerT = ISubscriberCallerBase<BaseEventT>;
	using PointerHolderBaseT = IPtrHolderBase;
	template <typename U>
	using PointerHolderT = PtrHolder<U>;

	virtual ~IEventBus() {}
	
	virtual void publish(EventId id, const BaseEventT&) = 0;
	virtual void subscribe(EventId id, std::shared_ptr<PointerHolderBaseT> holder, std::shared_ptr<SubscriberCallerT> subcriber) = 0;
	virtual void unsubscribe(EventId id, std::shared_ptr<PointerHolderBaseT> holder) = 0;

	template<typename EventType>
	void publish(const EventType& event) {
		publish(_getEventType<EventType>(), event);
	}

	// these could be base types
	template<typename ClassType, typename EventType>
	void subscribe(std::shared_ptr<PtrHolder<ClassType>> holder, std::shared_ptr<ClassSubscriberCaller<BaseEventT, ClassType, EventType>> subscriber) {
		
		std::shared_ptr<PointerHolderBaseT> holderBase = std::static_pointer_cast<PointerHolderBaseT>(holder);

		subscribe(_getEventType<EventType>(), holderBase, std::static_pointer_cast<SubscriberCallerT>(std::move(subscriber)));
	}

	template<typename EventType>
	void unsubscribe(std::shared_ptr<PointerHolderBaseT> holder) {
		unsubscribe(_getEventType<EventType>(), holder);
	}

private:
	template <typename EventType>
	EventId _getEventType() {
		static_assert(std::is_base_of_v<BaseEventT, EventType>, "Events need to derive from base event");
		return type_id<BaseEventT, EventType>().value();
	}
};