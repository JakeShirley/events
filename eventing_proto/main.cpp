#include <iostream>
#include <string>

#include "IEventBus.h"
#include "MinecraftEventing.h"

MinecraftEventing Eventing;

struct EventA : public IMinecraftEvent {
	EventA(std::string msg) : message(msg) {}
	std::string message;
};

struct EventB : public IMinecraftEvent {
	EventB(float i) : value(i) {}
	float value = 0;
};

class ExampleListener {
public:
	ExampleListener(const std::string& name)
		: mSubscription(Eventing.make_subscription(this))
		, mName(name)
	{

		mSubscription
			.subscribe<EventA>() // Defaults to &ExampleListener::handle(const EventA&)
			.subscribe<EventB, &ExampleListener::handle_different>();
	}

	void handle(const EventA& event) {
		std::cout << mName << " received event A with a value of " << event.message << "'" << std::endl;
	}

	void handle_different(const EventB& event) {
		std::cout << mName << " received event B with a value of '" << event.value << "'" << std::endl;
	}

private:
	const std::string mName;
	MinecraftEventSubscription<ExampleListener> mSubscription;
};

class ExampleBroadcaster {
public:
	ExampleBroadcaster(const std::string& name)
		: mPublisher(Eventing.make_publisher<EventA, EventB>())
		, mName(name) {
	}

	void sendMessage(std::string msg) {
		EventA event(std::move(msg));
		mPublisher.publish(event);
	}

	void sendFloat(float f) {
		EventB event(f);
		mPublisher.publish(event);
	}

private:
	const std::string mName;
	MinecraftEventPublisher<EventA, EventB> mPublisher;
};

int main() {
	ExampleBroadcaster broadcaster("broadcaster1");

	{
		ExampleListener listener1("Listener 1");

		broadcaster.sendMessage("test message 1");
		broadcaster.sendFloat(3.f);
	}
	
	ExampleListener listener2("Listener 2");

	broadcaster.sendMessage("test message 2");
	broadcaster.sendFloat(7.f);
}