#include "Button.h"


Button::Button (const std::string& filename, sf::Vector2f position, Gui::GuiEvent buttonType) {
	if(!texture.loadFromFile (filename))
		exit(1);
	image.setTexture (texture);
	image.setPosition (position);
	//image.setTextureRect (sf::IntRect (0, 0, 50, 50));
	retType = buttonType;
}

Button::Button () {
}


Button::~Button () {
}


void Button::draw (sf::RenderWindow* window) {
	window->draw (image);

}

Gui::GuiEvent Button::checkClicked (const float& posX, const float& posY) {
	sf::FloatRect rect = image.getGlobalBounds ();
	if (rect.contains (posX, posY))
		return retType;
	else
		return Gui::GuiEvent::MISSED;

}

Gui::GuiEvent Button::checkClicked (const sf::Vector2f& pos) {
	sf::FloatRect rect = image.getGlobalBounds ();
	if (rect.contains (pos)) 
		return retType;
	else
		return Gui::GuiEvent::MISSED;

}

void Button::setPosition (const sf::Vector2f& position) {
	image.setPosition (position);
}
