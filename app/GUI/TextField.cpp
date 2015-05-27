#include "TextField.h"


TextField::TextField (const sf::Vector2f& position, const sf::Vector2f size, sf::Font& font, TextType::type t) {

	field.setFillColor (INACTIVE);
	field.setOutlineColor (sf::Color (70, 70, 70, 255));
	field.setPosition (position);
	field.setSize (size);
	text.setPosition (position.x + 10, position.y + 5);

	text.setColor (sf::Color (200, 200, 200, 255));
	text.setCharacterSize (15);
	str = "";
	text.setString (str);
	isActive = false;
	text.setFont (font);
	type = t;

	label.setPosition (position.x, position.y - 16);
	label.setColor (sf::Color (140, 140, 140, 255));
	label.setCharacterSize (10);
	label.setFont (font);
	switch (t) {
		case TextType::MACADDR:
			label.setString ("Mac Address");
			break;
		case TextType::LOGIN:
			label.setString ("Login:");
			break;
		case TextType::PASSWORD:
			label.setString ("Password");
			break;
		default:
			break;
	}

}

TextField::TextField () {

}


TextField::~TextField () {
}

void TextField::draw (sf::RenderWindow* window) {
	window->draw (field);
	window->draw (text);
	window->draw (label);
}


void TextField::setIsActive (const bool& active) {
	if (active == isActive)
		return ;
	isActive = active;
	if (active) {
		field.setFillColor (ACTIVE);
	}
	else {
		field.setFillColor (INACTIVE);
	}
}

bool TextField::getIsActive () {
	return isActive;
}

Gui::GuiEvent TextField::checkActivation (const sf::Vector2f& click) {
	
	if (field.getGlobalBounds ().contains (click))
		return Gui::GuiEvent::ACTIVATED;
	return Gui::GuiEvent::MISSED;
}

void TextField::addCharToText (char& c){
	if (isActive) {
		switch (type) {
			case TextType::MACADDR:
				if (str.length () < 12) {

					if (c >= 65 && c <= 70)
						c += 32;
					
					if ((c >= 48 && c <= 57) || (c >= 97 && c <= 102)) 
					{
						str += c;
						if ((str.length () % 2) == 0 && str.length() != 12) {
							text.setString (text.getString()+ c + ":");
						}
						else 
							text.setString (text.getString () + c);
					}
				}
				break;
			case TextType::LOGIN:
				str += c;
				if (str.length () <= 16)
					text.setString (str);
				else
					text.setString (str.substr (str.length () - 16, str.length ()));
				break;
			case TextType::PASSWORD:
				str += c;
				if (str.length() <= 18)
					text.setString (text.getString () + "*");
				break;
			default:
				str += c;
				text.setString (str);
				break;
		}
	}
}


void TextField::eraseChar () {
	if (isActive) {
		str = str.substr (0, str.size () - 1);
		if (type == TextType::type::PASSWORD) {
			if (str.length() < 18) {
				std::string temp = text.getString ();
				temp = temp.substr (0, temp.size () - 1);
				text.setString (temp);
			}
		}
		else if (type == TextType::type::MACADDR) {
			std::string temp = text.getString ();
			if ((str.length() % 2) == 1 && (str.length() != 11 ))
				temp = temp.substr (0, temp.size () - 2);
			else
				temp = temp.substr (0, temp.size () - 1);
			text.setString (temp);
		}
		else {
			if (str.length () <= 16)
				text.setString (str);
			else
				text.setString (str.substr (str.length () - 16, str.length ()));
		}
	}
}


std::string TextField::getText () {
	return str;
}

