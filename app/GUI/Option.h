#ifndef OPTION
#define OPTION
#include <SFML/Graphics.hpp>
#include "../GuiEvent.h"
#include <iostream>
class Option {

public:
	Option (const sf::Font& font);
	~Option ();
	void setPostion (const sf::Vector2f& position);

	void draw (sf::RenderWindow* window);

	Gui::GuiEvent checkClicked (const float& posX, const float& posY);
	Gui::GuiEvent checkClicked (const sf::Vector2f& pos);

	std::string getText ();
	void setText (const std::string& text);

	void resetActive ();
	

private:
	std::string data;
	sf::Text text;
	sf::RectangleShape background;
	bool active = false;
	std::string parseText ();


};
#endif
