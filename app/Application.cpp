#include "Application.h"


Application::Application (Controller* contrl) : controller (contrl) {
}


Application::~Application () {
}


void Application::initialize () {
	//window initialization

	sf::ContextSettings settings;
	window.create (sf::VideoMode (800, 400), "EAPoL Client", sf::Style::Close|sf::Style::Titlebar, settings);
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
		controller->manageEvents (event);
		}
		window.clear (sf::Color(60,60,60,255));
		controller->draw ();
		window.display ();
	}

}