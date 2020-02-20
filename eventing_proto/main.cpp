#include <iostream>
#include <string>

#include "Dispatcher.h"

struct EventA {
	int foo = 1;
};

struct EventB {
	int bar = 2;
};

struct EventC {
	std::string name;
};

class ExampleListener1 {
public:
	void handle(const EventA& event) {
		std::cout << "ExampleListener1 - A - " << event.foo << std::endl;
	}

	void handle(const EventB& event) {
		std::cout << "ExampleListener1 - B - " << event.bar << std::endl;
	}

	void handle(const EventC& event) {
		std::cout << "ExampleListener1 - C - " << event.name << std::endl;
	}

	void handle(const int& event) {
		std::cout << "ExampleListener1 - int - " << event << std::endl;
	}
};

int main() {
	Dispatcher pub;

	{
		auto listener1 = std::make_shared<ExampleListener1>();

		pub.add<ExampleListener1, EventA, &ExampleListener1::handle>(listener1);
		pub.add<ExampleListener1, EventB, &ExampleListener1::handle>(listener1);
		pub.add<ExampleListener1, int, &ExampleListener1::handle>(listener1);

		pub.publish(EventA());
		pub.publish(6);
		pub.publish(EventB());
		pub.publish(EventC());
	}

	pub.publish(6);
	pub.publish(EventB());
	pub.publish(EventC());
}
