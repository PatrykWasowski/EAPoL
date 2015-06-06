#ifndef TEXT_FIELD
#define TEXT_FIELD

#include <SFML\Graphics.hpp>
#include <string>
#include <iostream>
#include "../GuiEvent.h"
#define ACTIVE sf::Color(30,30,30,255)
#define INACTIVE sf::Color(45,45,45,255)


namespace TextType {
	enum type {
		MACADDR,
		LOGIN,
		PASSWORD
	};
}

class TextField {
public:
	TextField (const sf::Vector2f& position, const sf::Vector2f size, sf::Font& font, TextType::type t);
	TextField ();
	~TextField ();

	void draw (sf::RenderWindow* window);

	void setIsActive (const bool& active);
	bool getIsActive ();
	Gui::GuiEvent checkActivation (const sf::Vector2f& click);


	void addCharToText (char& c);
	void eraseChar ();
	std::string getText ();
	
	
	void addMacChar (char& c);
	void setMac (const std::string& mac);
	void resetMac ();
private:
	sf::RectangleShape field;
	sf::Text text;
	bool isActive = false;
	std::string str;
	TextType::type type = TextType::type::LOGIN;
	sf::Text label;

};

#endif