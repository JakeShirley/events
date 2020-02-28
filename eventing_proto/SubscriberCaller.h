#pragma once

// if we wanted to remove a specific function we would add a comparison function here, that would compare the 
// method ptr.
template <typename BaseEventT>
struct ISubscriberCaller {
	virtual ~ISubscriberCaller() = default;
	virtual void receive(IPointerHolder& callee, const BaseEventT& event) = 0;
};

template<typename BaseEventT, typename ClassType, typename EventType>
struct ClassSubscriberCaller : public ISubscriberCaller<BaseEventT> {

	using MemberFn = void(ClassType::*)(const EventType &);
	ClassSubscriberCaller(MemberFn method)
		: mMethod(method) {

	}

	virtual ~ClassSubscriberCaller() {}

	virtual void receive(IPointerHolder& callee, const BaseEventT& baseEvent) override {
		static_assert(std::is_base_of_v<BaseEventT, EventType>, "Needs to derive from base event type");
		const EventType& event = static_cast<const EventType&>(baseEvent);

		PointerHolder<ClassType>& holder = static_cast<PointerHolder<ClassType>&>(callee);
		ClassType& ownerRef = *holder.owner;
		(ownerRef.*mMethod)(event);
	}

private:
	MemberFn mMethod;
};