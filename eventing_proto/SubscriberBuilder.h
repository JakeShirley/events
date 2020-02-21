#pragma once

#include "Dispatcher.h"
#include "Subscriber.h"

// TODO
template 
<typename SubscriberT, typename DispatcherT>
class SubscriptionBuilderHandle {
public:
	using SubHandleT = SubscriberHandle<SubscriberT, DispatcherT>;

	SubscriptionBuilderHandle(std::shared_ptr<SubscriberT> listener, std::weak_ptr<DispatcherT> dispatcher)
		: mHandle(listener.get(), std::move(dispatcher))
	{
	}

	template <typename EventT, void(SubscriberT::*MethodT)(const EventT &)>
	SubscriptionBuilderHandle& with() {
		// TODO: Debug assert for listener/dispatcher to catch if we're building more after we finish
		mHandle.add<EventT, MethodT>();

		return *this;
	}

	// Finishes the build statement and returns a handle
	SubHandleT build() {
		return std::move(mHandle);
	}
private:
	SubHandleT mHandle;
};

// TODO
template
<typename DispatcherT>
class SubscriberBuilder {

	template
	<typename SubscriberT>
	SubscriberHandle<SubscriberT, DispatcherT> subscribe(std::shared_ptr<SubscriberT> listener);

};

// TODO
template<>
class SubscriberBuilder<Dispatcher> {
public:

	SubscriberBuilder(std::shared_ptr<Dispatcher> dispatcher) 
		: mDispatcher(std::move(dispatcher)) {

	}

	template
	<typename SubscriberT>
	SubscriptionBuilderHandle<SubscriberT, Dispatcher> subscribe(std::shared_ptr<SubscriberT> listener) {
		return SubscriptionBuilderHandle<SubscriberT, Dispatcher>(listener, mDispatcher);
	}


private:
	std::weak_ptr<Dispatcher> mDispatcher;
};