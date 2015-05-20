#include "Controller.h"


Controller::Controller () : console (sf::Vector2f (35, 180), 20, 6) {
}


Controller::~Controller () {
}

void Controller::initManagers (sf::RenderWindow& wind){
	btnManager.associateWindow (wind);
	btnManager.createButtons ();
	txtFldManager.associateWindow (wind);
	txtFldManager.createFields ();
	console.associateWindow (wind);
}

void Controller::draw () {
	btnManager.draw ();
	txtFldManager.draw ();
	console.draw ();
}

void Controller::manageEvents (const sf::Event& event) {
	Gui::GuiEvent ev = Gui::GuiEvent::MISSED;
	if (event.type == sf::Event::MouseButtonPressed) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			if (btnManager.checkButtonsClicked (event) != Gui::GuiEvent::MISSED) {
				ev = btnManager.checkButtonsClicked (event);
				console.addMessage (std::to_string (debug++));
				performAction (ev);
			}
			else if (txtFldManager.checkActivation (event) != Gui::GuiEvent::MISSED) {
				ev = txtFldManager.checkActivation (event);
			}
			else if (console.checkButtonsClicked (event) != Gui::GuiEvent::MISSED) {
				ev = console.checkButtonsClicked (event);
				console.reactOnButton (ev);
			}
			
		}
		debugEvent ("click ", ev);

	}
	if (event.type == sf::Event::TextEntered) {
		if (txtFldManager.checkIfAnyActive ())
			ev = txtFldManager.manageTextInput (event);
		debugEvent ("key ", ev);
	}
}

void Controller::debugEvent (std::string from, Gui::GuiEvent event) {
	std::cout << from << " ";
	switch (event) {
		case Gui::MISSED:
			std::cout << "MISSED "<< std::endl;
			break;
		case Gui::ACTIVATED:
			std::cout << "ACTIVATED" << std::endl;
			break;
		case Gui::CONNECT:

			std::cout << "CONNECT" << std::endl;
			break;
		case Gui::DISCONNECT:

			std::cout << "DISCONECT" << std::endl;
			break;
		case Gui::CHAR_TYPED:

			std::cout << "CHAR_TYPED" << std::endl;
			break;
		case Gui::CLOSE:

			std::cout << "CLOSE" << std::endl;
			break;
		case Gui::MINIMIZE:

			std::cout << "MINIMIZE" << std::endl;
			break;
		case Gui::SCROLL_DOWN:

			std::cout << "SCROLL DOWN" << std::endl;
			break;
		case Gui::SCROLL_UP:

			std::cout << "SCROLL_UP" << std::endl;
			break;
		default:
			break;
	}
}

void Controller::performAction (const Gui::GuiEvent& event) {

	switch (event) {

		case Gui::CONNECT:
			// launch thread
			break;
		case Gui::DISCONNECT:
			// order thread to stop
			break;
		case Gui::CLOSE:
			//close app
			break;
		case Gui::MINIMIZE:
			// minimize app
			break;
		default:
			break;
	}

}

Console& Controller::getConsole () {
	return console;
}

void Controller::sendMessagge (const std::string& msg) {
	console.addMessage (msg);
}