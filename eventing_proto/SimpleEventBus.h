#pragma once

#include "IEventBus.h"

#include <memory>
#include <unordered_map>

template <typename T>
class SimpleEventBus : public IEventBus<T> {
public:
	using BaseEventT = typename IEventBus<T>::BaseEventT;
	using PointerHolderBaseT = typename IEventBus<T>::PointerHolderBaseT;
	using SubscriberCallerT = typename IEventBus<T>::SubscriberCallerT;
	template <typename U>
	using PointerHolderT = typename IEventBus<T>::template PointerHolderT<U>;

	virtual ~SimpleEventBus() {}

	virtual void publish(EventId id, const BaseEventT& eventBase) override {
		auto eventItr = mEventMap.find(id);
		if (eventItr != mEventMap.end()) {
			auto& eventSubscribers = eventItr->second;
			auto eventSubscribersItr = eventSubscribers.begin();
			while (eventSubscribersItr != eventSubscribers.end()) {
				auto& subscriber = *eventSubscribersItr;

				if (auto holder = subscriber.holder.lock()) {
					subscriber.caller->receive(*holder, eventBase);
					++eventSubscribersItr;
				}
				else {
					eventSubscribersItr = eventSubscribers.erase(eventSubscribersItr);
				}
			}
		}
	}

	virtual void subscribe(EventId id, std::shared_ptr<PointerHolderBaseT> holder, std::shared_ptr<SubscriberCallerT> subcriber) override {
		auto eventItr = mEventMap.find(id);
		if (eventItr == mEventMap.end()) {
			eventItr = mEventMap.emplace(id, std::vector<EventSubscriber>()).first;
		}

		EventSubscriber sub;
		sub.comparisonPtr = holder.get();
		sub.holder = holder;
		sub.caller = subcriber;
		// probably want to check if this holder is already on this event.. but we can also do a exhaustive search
		eventItr->second.emplace_back(std::move(sub));
	}

	virtual void unsubscribe(EventId id, std::shared_ptr<PointerHolderBaseT> holder) override {
		auto it = mEventMap.find(id);
		if (it != mEventMap.end()) {
			PointerHolderBaseT* ptr = holder.get();
			auto& eventCallbacks = it->second;

			for (auto callbackItr = eventCallbacks.begin(); callbackItr != eventCallbacks.end();) {
				if (callbackItr->comparisonPtr == ptr) {
					callbackItr = eventCallbacks.erase(callbackItr);
				}
				else {
					++callbackItr;
				}
			}
		}
	}

private:
	struct EventSubscriber {
		PointerHolderBaseT* comparisonPtr;
		std::weak_ptr<PointerHolderBaseT> holder;
		std::shared_ptr<SubscriberCallerT> caller;
	};

	std::unordered_map<EventId, std::vector<EventSubscriber>> mEventMap;
};

