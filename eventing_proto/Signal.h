#pragma once

#include <algorithm>
#include <memory>
#include <vector>

class BaseSignal {
public:
	virtual ~BaseSignal() = default;
};

template<typename EventT>
class Signal : public BaseSignal {
	using InstanceType = std::weak_ptr<void>;
	using CallType = bool(*)(std::weak_ptr<void> &, const EventT &);

	template<class ListenerT, void(ListenerT::*Fn)(const EventT &) = &ListenerT::receive>
	static bool stub(std::weak_ptr<void> &wptr, const EventT& event) {
		bool result = false;

		std::shared_ptr<ListenerT> ptr = std::static_pointer_cast<ListenerT>(wptr.lock());
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

	template<class ListenerT, void(ListenerT::*Fn)(const EventT&)>
	void add(std::weak_ptr<ListenerT> ptr) {
		remove<ListenerT, Fn>(ptr);
		mCalls.emplace_back(std::move(ptr), &stub<ListenerT, Fn>);
	}

	template<class ListenerT, void(ListenerT::*Fn)(const EventT&)>
	void add(std::shared_ptr<ListenerT> ptr) {
		add<ListenerT, Fn>(std::weak_ptr<ListenerT>(ptr));
	}

	template<class ListenerT, void(ListenerT::*Fn)(const EventT&)>
	void remove(std::weak_ptr<ListenerT> ptr) {
		Call call(std::move(ptr), &stub<ListenerT, Fn>);
		mCalls.erase(std::remove(mCalls.begin(), mCalls.end(), call), mCalls.end());
	}

	template<class ListenerT, void(ListenerT::*Fn)(const EventT&)>
	void remove(std::shared_ptr<ListenerT> ptr) {
		remove(std::weak_ptr<ListenerT>(ptr));
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