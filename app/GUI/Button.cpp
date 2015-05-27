#include "Button.h"


Button::Button (const std::string& filename, sf::Vector2f position, Gui::GuiEvent buttonType, const bool lck) {
	if(!texture.loadFromFile (filename))
		exit(1);
	image.setTexture (texture);
	image.setPosition (position);
	retType = buttonType;
	locked = lck;

	shadow.setFillColor (sf::Color (100, 100, 100, 50));
	shadow.setPosition (position);
	sf::Vector2f size;
	size.x = (float)texture.getSize ().x;
	size.y = (float) texture.getSize ().y;
	shadow.setSize (size);
}

Button::Button () {
}


Button::~Button () {
}


void Button::draw (sf::RenderWindow* window) {
	window->draw (image);
	if (locked)
		window->draw (shadow);
}

Gui::GuiEvent Button::checkClicked (const float& posX, const float& posY) {
	sf::FloatRect rect = image.getGlobalBounds ();
	if (rect.contains (posX, posY))
		return retType;
	else
		return Gui::GuiEvent::MISSED;

}

Gui::GuiEvent Button::checkClicked (const sf::Vector2f& pos) {
	if (locked)
		return Gui::GuiEvent::MISSED;
	sf::FloatRect rect = image.getGlobalBounds ();
	if (rect.contains (pos)) 
		return retType;
	else
		return Gui::GuiEvent::MISSED;
}

void Button::setPosition (const sf::Vector2f& position) {
	image.setPosition (position);
}


void Button::setLocked (const bool& lck) {
	locked = lck;
}
