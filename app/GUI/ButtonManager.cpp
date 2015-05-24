#include "ButtonManager.h"


ButtonManager::ButtonManager () {

}


ButtonManager::~ButtonManager () {
}


void ButtonManager::associateWindow (sf::RenderWindow& wind) {
	window = &wind;
}

void ButtonManager::createButtons () {
	buttonList.push_back (new Button ("Resources/button.png", sf::Vector2f (465.f, 30.f), Gui::GuiEvent::CONNECT));
	buttonList.push_back (new Button ("Resources/closeConnection.png", sf::Vector2f (465.f, 100.f), Gui::GuiEvent::DISCONNECT));
	//buttonList.push_back (new Button ("Resources/close.png", sf::vector2f (window->getSize()-30. , 10.f), Gui::GuiEvent::CLOSE));
	//buttonList.push_back (new Button ("Resources/minimize.png", sf::vector2f (window->getSize()-60. , 10.f), Gui::GuiEvent::MINIMIZE));
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
