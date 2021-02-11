#include <iostream>
#include <turbo/Engine.hpp>

int main() {
    turbo::EventHandler<unsigned int, float> event_handler([](unsigned int i, float j) {
        std::cout << "i: " << i << ", j: " << j << std::endl;
    });
    turbo::Event<unsigned int, float> testEvent;
    testEvent += event_handler;
    testEvent(4, -4.84);
    testEvent(8, 17.5);
    testEvent(19, 0.1);
    return 0;
}