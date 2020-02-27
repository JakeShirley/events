#include <iostream>
#include <string>

#include "MinecraftEventBus.h"
#include "Subscription.h"

struct EventA : public IEvent {
	EventA(int i) : foo(i) {}
	int foo = 0;
};

struct EventB : public IEvent {
	EventB(float i) : bar(i) {}
	float bar = 0;
};

class ExampleListener {
public:
	ExampleListener(IEventBus& eventBus, const std::string& listenerName)
		: mSubscription(this, eventBus) 
		, mListenerName(listenerName)
	{

		mSubscription
			.subscribe<EventA>() // Defaults to &ExampleListener1::handle
			.subscribe<EventB, &ExampleListener::handle_different>();
	}

	void handle(const EventA& event) {
		std::cout << mListenerName << " - A - " << event.foo << std::endl;
	}

	void handle_different(const EventB& event) {
		std::cout << mListenerName << " - B - " << event.bar << std::endl;
	}

private:
	const std::string mListenerName;
	Subscription<ExampleListener> mSubscription;
};

int main() {
	std::unique_ptr<IEventBus> eventBus = std::make_unique<MinecraftEventBus>();

	{
		std::cout << "Testing 1" << std::endl;
		ExampleListener listener1(*eventBus, "Listener 1");
		eventBus->publish(EventA(3));
		eventBus->publish(EventB(3.f));
	}

	std::cout << "Testing 2" << std::endl;
	ExampleListener listener2(*eventBus, "Listener 2");

	eventBus->publish(EventA(4));
	eventBus->publish(EventB(4.f));
}