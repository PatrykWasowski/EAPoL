#include "Application.h"


Application::Application (Controller& contrl) : controller (contrl) {
}


Application::~Application () {
}


void Application::initialize () {
	//window initialization

	//Logger::getInstance() << "Program Initialization - Build window" << std::endl;
	window.create (sf::VideoMode (550, 310), "EAPoL Client");
	window.setVerticalSyncEnabled (true);
	window.setPosition (sf::Vector2i (0, 0));
	window.setKeyRepeatEnabled (false);
	window.setFramerateLimit (60);

	controller.initManagers (window);

//	Logger::getInstance () << "Initialization Completed" << std::endl;
}

void Application::run () {
	
	while (window.isOpen ()) {
		sf::Event event;
		while (window.pollEvent (event)) {
			if (event.type == sf::Event::Closed)
					window.close ();
			controller.manageEvents (event);
		}
		window.clear (sf::Color(60,60,60,255));
		controller.draw ();
		window.display ();
	}

}