#pragma once

#include "IEventBus.h"

#include <memory>
#include <unordered_map>

class MinecraftEventBus : public IEventBus {
public:
	virtual ~MinecraftEventBus() {}

	virtual void publish(EventId id, const IEvent& eventBase) override {
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

	virtual void subscribe(EventId id, std::shared_ptr<IPtrHolderBase> holder, std::shared_ptr<ISubscriberCallerBase> subcriber) override {
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

	virtual void unsubscribe(EventId id, std::shared_ptr<IPtrHolderBase> holder) override {
		auto it = mEventMap.find(id);
		if (it != mEventMap.end()) {
			IPtrHolderBase* ptr = holder.get();
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
		IPtrHolderBase* comparisonPtr;
		std::weak_ptr<IPtrHolderBase> holder;
		std::shared_ptr<ISubscriberCallerBase> caller;
	};

	std::unordered_map<EventId, std::vector<EventSubscriber>> mEventMap;
};

