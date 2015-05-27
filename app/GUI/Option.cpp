#include "Option.h"


Option::Option (const sf::Font& font) {
	text.setColor (sf::Color (200, 200, 200, 255));
	text.setFont (font);
	text.setCharacterSize (12);
	text.setString ("");
	background.setSize (sf::Vector2f (460, 16));
	background.setFillColor (sf::Color (30, 30, 30, 255));
}


Option::~Option () {
}



void Option::draw (sf::RenderWindow* window) {
	if (active)
		window->draw (background);
	window->draw (text);

}

Gui::GuiEvent Option::checkClicked (const float& posX, const float& posY) {
	sf::FloatRect rect = background.getGlobalBounds ();
	if (rect.contains (posX, posY)) {
		active = true;
		return Gui::GuiEvent::ACTIVATED;
	}
	else {
		return Gui::GuiEvent::MISSED;
	}
}

Gui::GuiEvent Option::checkClicked (const sf::Vector2f& pos) {
	sf::FloatRect rect = background.getGlobalBounds ();
	if (rect.contains (pos)) {
		active = true;
		return Gui::GuiEvent::ACTIVATED;
	}
	else {
		return Gui::GuiEvent::MISSED;
	}
}


void Option::setPostion (const sf::Vector2f& position) {
	// position is position of the rectangle

	background.setPosition (position);
	text.setPosition (position.x + 5, position.y + 2);


}


void Option::setText (const std::string& txt) {
	data = txt;
	text.setString (parseText (data));
	//text.setString (data);
}

std::string Option::getText () {
	return data;
}

void Option::resetActive () {
	active = false;
}

std::string Option::parseText (std::string temp) {
	if (temp.size () > 16)
	return temp.substr (16, temp.length ());
	return temp;
}