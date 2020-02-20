#pragma once

#include <inttypes.h>
#include <memory>
#include <unordered_map>

#include "Signal.h"
#include "TypeId.h"

class Dispatcher {
public:
	using SignalTypeId = uint32_t;

	template
	<typename ListenerT, typename EventT, void(ListenerT::*Fn)(const EventT&)>
	void add(std::shared_ptr<ListenerT> listener) {
		auto signalPtr = _getOrCreateSignal<EventT>();

		if (signalPtr) {
			return signalPtr->add<ListenerT, Fn>(listener);
		}
		else {
			// TODO: Fail here
		}
	}

	template
	<typename EventT>
	void publish(const EventT& event) {
		//static_assert(std::is_base_of<BaseEvent, EventT>::value, "Event type must inherit from BaseEvent");

		auto signalPtr = _getOrCreateSignal<EventT>();
		if (signalPtr) {
			return signalPtr->publish(event);
		}
	}


private:

	template <typename EventT>
	Signal<EventT>* _getOrCreateSignal() {
		//static_assert(std::is_base_of<BaseEvent, EventT>::value, "Event type must inherit from BaseEvent");

		using SignalT = Signal<EventT>;
		const auto signalTypeId = type_id<BaseSignal, SignalT>().value();

		auto signalItr = mSignals.find(signalTypeId);
		if (signalItr != mSignals.end()) {
			return static_cast<SignalT*>(signalItr->second.get());
		}
		else {
			auto newSignalItr = mSignals.emplace(signalTypeId, std::make_unique<SignalT>());
			return static_cast<SignalT*>(newSignalItr.first->second.get());
		}
	}

	template <typename EventT>
	Signal<EventT>* _getSignal(SignalTypeId id) {
		//static_assert(std::is_base_of<BaseEvent, EventT>::value, "Event type must inherit from BaseEvent");

		using SignalT = Signal<EventT>;
		const auto signalTypeId = type_id<BaseSignal, SignalT>().value();

		auto signalItr = mSignals.find(signalTypeId);
		if (signalItr != mSignals.end()) {
			return static_cast<SignalT*>(signalItr->second.get());
		}
		return nullptr;
	}

	std::unordered_map<SignalTypeId, std::unique_ptr<BaseSignal>> mSignals;
};