#ifndef BUTTON
#define BUTTON

#include <SFML/Graphics.hpp>
#include <iostream>
#include "../GuiEvent.h"
class Button {
public:
	
	Button ();
	Button (const std::string& filename, sf::Vector2f position, Gui::GuiEvent ButtonType);
	virtual ~Button ();
	
	void draw (sf::RenderWindow*);
	
	Gui::GuiEvent checkClicked (const float& posX, const float& posY);
	Gui::GuiEvent checkClicked (const sf::Vector2f& pos);

	//getters and setters

	void setPosition (const sf::Vector2f& position);

protected:
	sf::Sprite image;
	sf::Texture texture;
	sf::RectangleShape rect;
	Gui::GuiEvent retType;
};
#endif
