#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "Subscriber.h"

class BaseSignal {
public:
	virtual ~BaseSignal() = default;
};

template<typename EventT>
class Signal : public BaseSignal {
	using InstanceType = std::weak_ptr<void>;
	using CallType = bool(*)(std::weak_ptr<void>& wptr, const EventT& event);

	template<class SubscriberT, typename MethodT, MethodT Fn>
	static bool stub(std::weak_ptr<void>& wptr, const EventT& event) {
		bool result = false;

		std::shared_ptr<SubscriberT> ptr = std::static_pointer_cast<SubscriberT>(wptr.lock());
		if (ptr) {
			(ptr.get()->*Fn)(event);
			result = true;
		}

		return result;
	}

	struct Call {
		InstanceType instance;
		CallType call;

		Call(InstanceType i, CallType c)
			: instance(std::move(i))
			, call(std::move(c)) {
		}

		bool operator==(const Call& other) const {
			return
				(instance.lock() == other.instance.lock()) &&
				(call == other.call);
		}
	};

public:
	
	// TODO: Add overloads for weak ptrs?

	template<class SubscriberT, void(SubscriberT::*MethodT)(const EventT &), MethodT Fn>
	void add(SubscriberHandleWeak<SubType> handle) {
		// TODO Handle lambdas

		// Remove incase we already exist
		remove<SubscriberT, MethodT, Fn>(handle);

		// Insert
		Call call(std::move(handle.handle), &stub<SubscriberT, MethodT, Fn>);
		mCalls.emplace_back(std::move(call));
	}
	
	template<class SubscriberT, void(SubscriberT::*MethodT)(const EventT &), MethodT Fn>
	void remove(SubscriberHandleWeak<SubscriberT> handle) {
		Call call(std::move(handle.handle), &stub<SubscriberT, MethodT, Fn>);
		mCalls.erase(std::remove(mCalls.begin(), mCalls.end(), call), mCalls.end());
	}

	void publish(const EventT& event) {
		for (auto itr = mCalls.begin(); itr != mCalls.end();) {
			bool valid = (itr->call)(itr->instance, event);

			if (!valid) {
				itr = mCalls.erase(itr);
			}
			else {
				itr++;
			}
		}
	}

	std::size_t size() const noexcept {
		return mCalls.size();
	}

private:
	std::vector<Call> mCalls;
};