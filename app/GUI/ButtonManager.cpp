#include "ButtonManager.h"


ButtonManager::ButtonManager () {

}


ButtonManager::~ButtonManager () {
}


void ButtonManager::associateWindow (sf::RenderWindow& wind) {
	window = &wind;
}

void ButtonManager::createButtons () {
	buttonList.push_back (new Button ("Resources/button.png", sf::Vector2f (715.f, 30.f), Gui::GuiEvent::CONNECT, false));
	buttonList.push_back (new Button ("Resources/closeConnection.png", sf::Vector2f (715.f, 100.f), Gui::GuiEvent::DISCONNECT, true));
	//buttonList.push_back (new Button ("Resources/close.png", sf::Vector2f (window->getSize().x-20. ,5.f), Gui::GuiEvent::CLOSE));
	//buttonList.push_back (new Button ("Resources/minimize.png", sf::Vector2f (window->getSize().x-40. , 5.f), Gui::GuiEvent::MINIMIZE));
}

void ButtonManager::draw () {
	for (auto &i : buttonList) {
		i->draw (window);
	}
}

Gui::GuiEvent ButtonManager::checkButtonsClicked (sf::Event event) {
	sf::Vector2f position ((float) event.mouseButton.x, (float) event.mouseButton.y);
	Gui::GuiEvent ev = Gui::GuiEvent::MISSED;
	for (auto &i : buttonList) {
		if (i->checkClicked (position) != Gui::GuiEvent::MISSED) {
			ev = i->checkClicked (position);
			break;
		}
	}
	return ev;
}

void ButtonManager::changeBtnActivity (const int& position, const bool& activity) {
	if (position > (int)buttonList.size ())
		return;
	buttonList [position]->setLocked (activity);

}