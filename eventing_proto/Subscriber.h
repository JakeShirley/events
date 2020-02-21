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
		: handle(std::move(_handle)) 
	{}

	SubscriberHandleWeak(const SubscriberHandleWeak&) = default;
	SubscriberHandleWeak(SubscriberHandleWeak&&) = default;

	HandleT handle;
};

// TODO
template<typename S, typename D>
class SubscriberHandle {
public:
	using SubscriberT = S;
	using DispatcherT = D;
	using WeakHandleT = SubscriberHandleWeak<SubscriberT>;

	SubscriberHandle(const SubscriberHandle&) = delete;
	SubscriberHandle(SubscriberHandle&& rhs) = default;
	SubscriberHandle& operator=(const SubscriberHandle&) = delete;


	SubscriberHandle(SubscriberT* owner, std::weak_ptr<DispatcherT> dispatcher)
		: mInternalHandle(std::make_shared<detail::InternalWrapper<SubscriberT>>(owner))
		, mDispatcher(std::move(dispatcher)) {
		// TODO: Assert if T is null
	}

	~SubscriberHandle() {
		// Automatically unsubscribe
		for (auto&& unsubscriber : mUnsubscribers) {
			unsubscriber->remove(make_weak());
		}
	}

	template
	<typename EventT, void(SubscriberT::*MethodT)(const EventT &)>
	void add() {
		if (auto dispatcher = mDispatcher.lock()) {
			dispatcher->add<SubscriberT, EventT, MethodT>(make_weak());

			// Note: These are not cleaned up at the moment,
			// so if you manually unsub than we will try to do
			// it again and no-op
			mUnsubscribers.emplace_back(std::make_unique<Unsubscriber<DispatcherT, SubscriberT, EventT, MethodT>>(mDispatcher));
		}
	}

	template
	<typename EventT, void(SubscriberT::*MethodT)(const EventT &)>
	void remove() {
		if (auto dispatcher = mDispatcher.lock()) {
			dispatcher->remove<SubscriberT, EventT, MethodT>(make_weak());
		}
	}

	// TODO: Register
	// TODO: Unregister


	WeakHandleT make_weak() const {
		return WeakHandleT(mInternalHandle);
	}

private:
	class IUnsubscriber {
	public:
		virtual ~IUnsubscriber() = default;
		virtual void remove(SubscriberHandleWeak<SubscriberT> handle) = 0;
	};

	template
	<typename DispatcherT, typename SubscriberT, typename EventT, void(SubscriberT::*MethodT)(const EventT &)>
	class Unsubscriber : public IUnsubscriber {
	public:
		Unsubscriber(std::weak_ptr<DispatcherT> dispatcher)
			: mDispatcher(std::move(dispatcher))
		{
		}

		virtual void remove(SubscriberHandleWeak<SubscriberT> handle) override {
			if (auto dispatcher = mDispatcher.lock()) {
				dispatcher->remove<SubscriberT, EventT, MethodT>(std::move(handle));
			}
		}

	private:
		std::weak_ptr<DispatcherT> mDispatcher;
	};
	
	std::vector<std::unique_ptr<IUnsubscriber>> mUnsubscribers;
	std::shared_ptr<detail::InternalWrapper<SubscriberT>> mInternalHandle;
	std::weak_ptr<DispatcherT> mDispatcher;
};