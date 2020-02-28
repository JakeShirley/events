#include <iostream>
#include <string>

#include "IEventBus.h"
#include "MinecraftEventing.h"
#include "Publisher.h"

MinecraftEventing Eventing;

struct EventA : public IMinecraftEvent {
	EventA(int i) : foo(i) {}
	int foo = 0;
};

struct EventB : public IMinecraftEvent {
	EventB(float i) : bar(i) {}
	float bar = 0;
};

class ExampleFoo {
public:
	ExampleFoo(const std::string& listenerName)
		: mSubscription(Eventing.make_subscription(this))
		, mListenerName(listenerName)
	{

		mSubscription
			.subscribe<EventA>()
			.subscribe<EventB, &ExampleFoo::handle_different>();
	}

	void handle(const EventA& event) {
		std::cout << mListenerName << " - A - " << event.foo << std::endl;
	}

	void handle_different(const EventB& event) {
		std::cout << mListenerName << " - B - " << event.bar << std::endl;
	}

private:
	const std::string mListenerName;
	MinecraftEventSubscription<ExampleFoo> mSubscription;
};

int main() {
	auto publisher = Eventing.make_publisher<EventA, EventB>();

	{
		std::cout << "Testing 1" << std::endl;
		ExampleFoo listener1("Listener 1");
		publisher.publish(EventA(3));
		publisher.publish(EventB(3.f));
	}

	std::cout << "Testing 2" << std::endl;
	ExampleFoo listener2("Listener 2");

	publisher.publish(EventA(3));
	publisher.publish(EventB(3.f));
}