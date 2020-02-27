#pragma once

#include "IEvent.h"
#include "TypeId.h"
#include <memory>

struct IPtrHolderBase {
	virtual ~IPtrHolderBase() = default;
};

template<typename ClassType>
struct PtrHolder : public IPtrHolderBase {
	PtrHolder(ClassType* _owner) : owner(_owner) {}
	virtual ~PtrHolder() = default;
	ClassType* owner;
};


// if we wanted to remove a specific function we would add a comparison function here, that would compare the 
// method ptr.
struct ISubscriberCallerBase {
	virtual ~ISubscriberCallerBase() = default;
	virtual void receive(IPtrHolderBase& callee, const IEvent& event) = 0;
};

template<typename ClassType, typename EventType>
struct ClassSubscriberCaller : public ISubscriberCallerBase {

	using MemberFn = void(ClassType::*)(const EventType &);
	ClassSubscriberCaller(MemberFn method)
		: mMethod(method) {

	}

	virtual ~ClassSubscriberCaller() {}

	virtual void receive(IPtrHolderBase& callee, const IEvent& baseEvent) override {
		static_assert(std::is_base_of_v<IEvent, EventType>, "Needs to derive from IEvent");
		const EventType& event = static_cast<const EventType&>(baseEvent);

		PtrHolder<ClassType>& holder = static_cast<PtrHolder<ClassType>&>(callee);
		ClassType& ownerRef = *holder.owner;
		(ownerRef.*mMethod)(event);
	}

private:
	MemberFn mMethod;
};

class IEventBus {
public:
	virtual ~IEventBus() {}
	
	virtual void publish(EventId id, const IEvent&) = 0;
	virtual void subscribe(EventId id, std::shared_ptr<IPtrHolderBase> holder, std::shared_ptr<ISubscriberCallerBase> subcriber) = 0;
	virtual void unsubscribe(EventId id, std::shared_ptr<IPtrHolderBase> holder) = 0;

	template<typename EventType>
	void publish(const EventType& event) {
		publish(_getEventType<EventType>(), event);
	}

	// these could be base types
	template<typename ClassType, typename EventType>
	void subscribe(std::shared_ptr<PtrHolder<ClassType>> holder, std::shared_ptr<ClassSubscriberCaller<ClassType, EventType>> subscriber) {
		
		std::shared_ptr<IPtrHolderBase> holderBase = std::static_pointer_cast<IPtrHolderBase>(holder);

		subscribe(_getEventType<EventType>(), holderBase, std::static_pointer_cast<ISubscriberCallerBase>(std::move(subscriber)));
	}

	template<typename EventType>
	void unsubscribe(std::shared_ptr<IPtrHolderBase> holder) {
		unsubscribe(_getEventType<EventType>(), holder);
	}

private:
	template <typename EventType>
	EventId _getEventType() {
		static_assert(std::is_base_of_v<IEvent, EventType>, "Events need to derive from IEvent");
		return type_id<IEvent, EventType>().value();
	}
};