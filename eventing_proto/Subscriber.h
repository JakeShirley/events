#pragma once

#include <memory>

namespace detail {
	// TODO
	template<typename S>
	struct InternalWrapper {
		using SubscriberT = S;

		InternalWrapper(SubscriberT* _subscriber) : subscriber(_subscriber) {}
		SubscriberT* subscriber = nullptr;
	};
};

// TODO
template<typename S>
struct SubscriberHandleWeak {
	using SubscriberT = S;
	using HandleT = std::weak_ptr<detail::InternalWrapper<SubscriberT>>;

	SubscriberHandleWeak(HandleT _handle) 
		: handle(std::move(handle)) 
	{}

	HandleT handle;
};

// TODO
template<typename S, typename D>
struct SubscriberHandle {
	using SubscriberT = S;
	using DispatcherT = D;
	using WeakHandleT = SubscriberHandleWeak<SubscriberT>;

	SubscriberHandle(const SubscriberHandle&) = delete;
	SubscriberHandle& operator=(const SubscriberHandle&) = delete;

	SubscriberHandle(SubscriberHandle&& rhs) {
		mInternalHandle = std::move(rhs.mInternalHandle);
		mDispatcher = std::move(rhs.mDispatcher);
	}

	SubscriberHandle(SubscriberT* owner, std::weak_ptr<DispatcherT> dispatcher)
		: mInternalHandle(std::make_shared<detail::InternalWrapper<SubscriberT>>(owner))
		, mDispatcher(std::move(dispatcher)) {
		// TODO: Assert if T is null
	}

	~SubscriberHandle() {
		// TODO: Unregister all
	}

	template
	<typename EventT, void(SubscriberT::*MethodT)(const EventT &), MethodT Fn>
	void add() {
		if (auto dispatcher = mDispatcher.lock()) {
			dispatcher->add<SubscriberT, EventT, MethodT>(make_weak());
		}
	}

	// TODO: Register
	// TODO: Unregister


	WeakHandleT make_weak() const {
		return WeakHandleT(mInternalHandle);
	}

private:
	std::shared_ptr<detail::InternalWrapper<SubscriberT>> mInternalHandle;
	std::weak_ptr<DispatcherT> mDispatcher;
};