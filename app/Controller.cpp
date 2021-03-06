﻿#include "Controller.h"


Controller::Controller () : console (sf::Vector2f (35, 180), 60, 12), choicePanel(sf::Vector2f(210, 30), 8) {
}

Controller::~Controller () {
}

void Controller::initManagers (sf::RenderWindow& wind){
	window = &wind;
	btnManager.associateWindow (wind);
	btnManager.createButtons ();
	txtFldManager.associateWindow (wind);
	txtFldManager.createFields ();
	console.associateWindow (wind);
	choicePanel.associateWindow (wind);
	credits.setString ("Autorzy:  Szymon Mysiak,  Patryk Wasowski,  Maciej Lotz,  Andrzej Siadkowski,  Copyright © 2015,  All rights reserved");
	credits.setPosition ((float)40.0, window->getSize().y - (float)15.0);
	font.loadFromFile ("./Resources/data_control/data-latin.ttf");
	credits.setFont (font);
	credits.setCharacterSize (10);
	credits.setColor (sf::Color (140, 140, 140, 255));
}

void Controller::draw () {
	btnManager.draw ();
	txtFldManager.draw ();
	console.draw ();
	choicePanel.draw ();
	window->draw (credits);
}

void Controller::manageEvents (const sf::Event& event) {
	Gui::GuiEvent ev = Gui::GuiEvent::MISSED;
	if (event.type == sf::Event::MouseButtonPressed) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			if (btnManager.checkButtonsClicked (event) != Gui::GuiEvent::MISSED) {
				ev = btnManager.checkButtonsClicked (event);
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

	}// end mouse button pressed
	
	if (event.type == sf::Event::TextEntered) {
		if (txtFldManager.checkIfAnyActive ())
			ev = txtFldManager.manageTextInput (event);
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
			if (choicePanel.getOption () == 0)
				console.addMessage ("You have to choose interface!");
			else {
				connect ();
				connecting = true;
			}			
			break;
		case Gui::DISCONNECT:
			if (c)
				disconnect ();
			c = false;
			connecting = false;
			break;
		case Gui::CLOSE:	
			r = false;
			if (c)
				performAction (Gui::GuiEvent::DISCONNECT);
			else {
				if (connecting)
					performAction (Gui::GuiEvent::DISCONNECT);
				else
					performAction (Gui::GuiEvent::CONNECT);
			}
			std::cout <<"ok";
			window->close ();
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
	mtx.lock ();
	console.addMessage (msg);
	mtx.unlock ();
}
void Controller::setCriticalSection (CriticalSectionPack* pck, CriticalSectionPack* pck2) {
	connectPack = pck;
	disconnectPack = pck2;
}

void Controller::connect () {
	std::unique_lock<std::mutex> lck (connectPack->mtx);
	(connectPack->ready) = true;
	connectPack->cv.notify_all ();
}

void Controller::disconnect () {
	std::unique_lock<std::mutex> lck (disconnectPack->mtx);
	(disconnectPack->ready) = true;
	disconnectPack->cv.notify_all ();
}


bool Controller::running () {
	mtx.lock ();
	bool rr = r;
	mtx.unlock ();
	return rr;
}

std::vector<std::string> Controller::getData () {
	mtx.lock ();
	std::vector<std::string> str = txtFldManager.getData ();
	mtx.unlock ();
	return str;
}

void Controller::addOption (const std::string& opt) {
	mtx.lock ();
	choicePanel.addOption (opt);
	mtx.unlock ();
}

int Controller::getOption () {
	mtx.lock ();
	int i = choicePanel.getOption ();
	mtx.unlock ();
	return i;
}

void Controller::clearOptions () {
	mtx.lock ();
	choicePanel.clear();
	mtx.unlock ();
}

void Controller::setButtonLocked (const int& id, const bool& lock) {
	mtx.lock ();
	btnManager.changeBtnActivity (id, lock);
	mtx.unlock ();
}

bool Controller::getConnection () {
	mtx.lock ();
	bool a = c;
	mtx.unlock ();
	return a;
}

void Controller::setConnection (bool con) {
	mtx.lock ();
	c = con;
	mtx.unlock ();
}

bool Controller::getConnecting () {
	mtx.lock ();
	bool a = connecting;
	mtx.unlock ();
	return connecting;
}

void Controller::setConnecting (bool con) {
	mtx.lock ();
	connecting = con;
	mtx.unlock ();
}

void Controller::selectLastPort () {
	if (!choicePanel.selectLastActive ())
		console.addMessage ("You have to choose interface");
}

void Controller::setLastPort (const unsigned int& lastPort) {
	choicePanel.setLastActive (lastPort);
}

unsigned int Controller::getLastPort () {
	mtx.lock ();
	unsigned int i = choicePanel.getLastActive ();
	mtx.unlock ();
	return i;
}

void Controller::setMacAddress (const std::string& mac) {
	mtx.lock ();
	std::string copy = mac;
	txtFldManager.setMacAddress (copy);
	mtx.unlock ();
}

void Controller::addCharToMac (const char& c) {
	mtx.lock ();
	char copy = c;
	txtFldManager.addCharToMac(copy);
	mtx.unlock ();
}