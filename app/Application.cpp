#include "Application.h"


Application::Application (Controller* contrl) : controller (contrl) {
}


Application::~Application () {
}


void Application::initialize () {
	//window initialization

	window.create (sf::VideoMode (800, 510), "EAPoL Client");
	window.setVerticalSyncEnabled (true);
	window.setPosition (sf::Vector2i (0, 0));
	window.setKeyRepeatEnabled (false);
	window.setFramerateLimit (60);

	controller->initManagers (window);
}

void Application::run () {
	
	while (window.isOpen ()) {
		sf::Event event;
		while (window.pollEvent (event)) {
			if (event.type == sf::Event::Closed)
			controller->manageEvents (event);
		}
		window.clear (sf::Color(60,60,60,255));
		controller->draw ();
		window.display ();
	}

}