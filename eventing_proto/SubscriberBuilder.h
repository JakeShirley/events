#pragma once

#include "Dispatcher.h"
#include "Subscriber.h"

// TODO
template 
<typename ListenerT, typename DispatcherT>
class SubscriptionBuilderHandle {
public:
	using SubHandleT = SubscriberHandle<ListenerT, DispatcherT>;

	SubscriptionBuilderHandle(std::shared_ptr<ListenerT> listener, std::weak_ptr<DispatcherT> dispatcher)
		: mHandle(listener.get(), std::move(dispatcher))
	{
	}

	template <typename EventT, typename MethodFn>
	SubscriptionBuilderHandle& with(MethodFn fn) {
		// TODO: Debug assert for listener/dispatcher to catch if we're building more after we finish
		// TODO: Register
		mHandle.add<EventT, MethodFn>(fn);

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
	<typename ListenerT>
	SubscriberHandle<ListenerT, DispatcherT> subscribe(std::shared_ptr<ListenerT> listener);

};

// TODO
template<>
class SubscriberBuilder<Dispatcher> {
public:

	SubscriberBuilder(std::shared_ptr<Dispatcher> dispatcher) 
		: mDispatcher(std::move(dispatcher)) {

	}

	template
	<typename ListenerT>
		SubscriptionBuilderHandle<ListenerT, Dispatcher> subscribe(std::shared_ptr<ListenerT> listener) {
		return SubscriptionBuilderHandle<ListenerT, Dispatcher>(listener, mDispatcher);
	}


private:
	std::weak_ptr<Dispatcher> mDispatcher;
};