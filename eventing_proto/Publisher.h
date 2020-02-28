#pragma once

#include <type_traits>

namespace detail {
	template<typename T, typename... Ts>
	struct contains : std::disjunction<std::is_same<T, Ts>...> {
	};

	template<typename T, typename... Ts>
	constexpr bool contains_v = contains<T, Ts...>::value;

	template<typename T, typename... Ts>
	struct has_base : std::disjunction<std::is_base_of<T, Ts>...> {
	};

	template<typename T, typename... Ts>
	constexpr bool has_base_v = has_base<T, Ts...>::value;
}

template <typename EventBusT, typename ... EventTypeTs>
class Publisher {
public:
	using BaseEventT = typename EventBusT::BaseEventT;

	static_assert(detail::has_base_v<BaseEventT, EventTypeTs ...>, "All types published need to inherit from base event type");

	Publisher(EventBusT& eventBus) 
		: mEventBus(eventBus) {
	}

	template <typename EventTypeT>
	void publish(const EventTypeT& event) {
		static_assert(detail::contains_v<EventTypeT, EventTypeTs ...>, "All types given to publish need to be supported by Publisher type");
		// do the thing
		mEventBus.publish(event);
	}

private:
	EventBusT& mEventBus;
};