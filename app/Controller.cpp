#include "Controller.h"


Controller::Controller () : console (sf::Vector2f (35, 180), 20, 6), choicePanel(sf::Vector2f(210, 30), 8) {
}


Controller::~Controller () {
}

void Controller::initManagers (sf::RenderWindow& wind){
	btnManager.associateWindow (wind);
	btnManager.createButtons ();
	txtFldManager.associateWindow (wind);
	txtFldManager.createFields ();
	console.associateWindow (wind);
	choicePanel.associateWindow (wind);
}

void Controller::draw () {
	btnManager.draw ();
	txtFldManager.draw ();
	console.draw ();
	choicePanel.draw ();
}

void Controller::manageEvents (const sf::Event& event) {
	Gui::GuiEvent ev = Gui::GuiEvent::MISSED;
	if (event.type == sf::Event::MouseButtonPressed) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			if (btnManager.checkButtonsClicked (event) != Gui::GuiEvent::MISSED) {
				ev = btnManager.checkButtonsClicked (event);
			//	console.addMessage (std::to_string (debug));
				performAction (ev);
			}
			else if (txtFldManager.checkActivation (event) != Gui::GuiEvent::MISSED) {
				ev = txtFldManager.checkActivation (event);
			}
			else if (console.checkButtonsClicked (event) != Gui::GuiEvent::MISSED) {
				ev = console.checkButtonsClicked (event);
				console.reactOnButton (ev);
			}
			else if (choicePanel.checkButtonsClicked (event) != Gui::GuiEvent::MISSED) {
				ev=choicePanel.checkButtonsClicked (event);
				choicePanel.reactOnButton (ev);
			}
			else if (choicePanel.checkOptionsClicked (event) != Gui::GuiEvent::MISSED) {
				ev = choicePanel.checkOptionsClicked (event);
			}
		}
	//	debugEvent ("click ", ev);

	}// end mouse button pressed
	
	if (event.type == sf::Event::TextEntered) {
		if (txtFldManager.checkIfAnyActive ())
			ev = txtFldManager.manageTextInput (event);
	//	debugEvent ("key ", ev);
	}
	if (event.type == sf::Event::Closed) {
		performAction (Gui::GuiEvent::CLOSE);
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
			connect ();
			break;
		case Gui::DISCONNECT:
			choicePanel.addOption (std::to_string (debug++));
			break;
		case Gui::CLOSE:	
			runMutex.lock ();
			r = false;
			runMutex.unlock ();
			connect ();
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
void Controller::setCriticalSection (std::mutex*m, bool* r, std::condition_variable* c) {
	mtx = m;
	ready = r;
	cv = c;
}

void Controller::connect () {
	std::unique_lock<std::mutex> lck (*mtx);
	(*ready) = true;
	cv->notify_all ();
}

bool Controller::running () {
	return r;
}

std::vector<std::string> Controller::getData () {
	return txtFldManager.getData ();
}
void Controller::addOption (const std::string& opt) {
	choicePanel.addOption (opt);
}

std::string Controller::getOption () {
	return choicePanel.getOption ();
}