#pragma once

#include "PointerHolder.h"
#include "SubscriberCaller.h"

#include <memory>
#include <vector>

template<typename EventBusT, typename SubscriberT>
class Subscription {
public:
	using SubscriberPointerHolder = typename EventBusT::template PointerHolderT<SubscriberT>;

	Subscription(const Subscription&) = delete;
	Subscription(Subscription&& rhs) = default;
	Subscription& operator=(const Subscription&) = delete;

	Subscription(SubscriberT* owner, EventBusT& bus)
		: mHandle(std::make_shared<SubscriberPointerHolder>(owner))
		, mEventBus(bus) {
		// TODO: Assert if T is null
	}

	~Subscription() {
		// Automatically unsubscribe
		for (auto&& unsubscriber : mUnsubscribers) {
			unsubscriber->remove();
		}
	}

	template
	<typename EventT, void (SubscriberT::*Method)(const EventT&) = &SubscriberT::handle>
	Subscription& subscribe() {
		
		auto subscriber = std::make_shared<ClassSubscriberCaller<EventBusT::BaseEventT, SubscriberT, EventT>>(Method);
		mEventBus.subscribe(mHandle, subscriber);

		// Note: These are not cleaned up at the moment,
		// so if you manually unsub than we will try to do
		// it again and no-op
		mUnsubscribers.emplace_back(std::make_unique<Unsubscriber<EventT>>(mEventBus, mHandle));
		
		// Return this to support builder pattern
		return *this;
	}

	template
	<typename EventT>
	Subscription& unsubscribe() {
		mEventBus.unsubscribe<EventT>(mHandle);

		// Return this to support builder pattern
		return *this;
	}

private:
	class IUnsubscriber {
	public:
		virtual ~IUnsubscriber() = default;
		virtual void remove() = 0;
	};

	template
	<typename EventT>
	class Unsubscriber : public IUnsubscriber {
	public:
		Unsubscriber(EventBusT& bus, std::shared_ptr<typename EventBusT::PointerHolderBaseT> handle)
			: mEventBus(bus)
			, mHandle(std::move(handle))
		{
		}

		virtual void remove() override {
			mEventBus.unsubscribe<EventT>(mHandle);
		}

	private:
		EventBusT& mEventBus;
		std::shared_ptr<typename EventBusT::PointerHolderBaseT> mHandle;
	};

	
	std::vector<std::unique_ptr<IUnsubscriber>> mUnsubscribers;
	std::shared_ptr<SubscriberPointerHolder> mHandle;
	EventBusT& mEventBus;
};