#include "TextFieldManager.h"


TextFieldManager::TextFieldManager () {
	font.loadFromFile ("./Resources/data_control/data-latin.ttf");
}


TextFieldManager::~TextFieldManager () {
}


void TextFieldManager::associateWindow (sf::RenderWindow& wind) {
	window = &wind;
}

void TextFieldManager::createFields () {
	fieldsList.push_back (new TextField (sf::Vector2f (35.f, 30.f), sf::Vector2f (160.f, 30.f), font, TextType::type::LOGIN));
	fieldsList.push_back (new TextField (sf::Vector2f (35.f, 80.f), sf::Vector2f (160.f, 30.f), font, TextType::type::PASSWORD));
	fieldsList.push_back (new TextField (sf::Vector2f (35.f, 130.f), sf::Vector2f (160.f, 30.f), font, TextType::type::MACADDR));
}

void TextFieldManager::draw () {
	for (auto &i : fieldsList) {
		i->draw (window);
	}
}

Gui::GuiEvent TextFieldManager::checkActivation (const sf::Event& ev) {
	if (ev.type == sf::Event::MouseButtonPressed) {
		sf::Vector2f click ((float) ev.mouseButton.x, (float) ev.mouseButton.y);
		bool clicked = false;
		for (auto &i : fieldsList) {
			if (i->checkActivation (click) == Gui::GuiEvent::ACTIVATED) {
				i->setIsActive (true);
				activeField = i;
				clicked = true;
			}
			else
				i->setIsActive (false);
		}
		if (!clicked) {
			activeField = nullptr;
			return Gui::GuiEvent::MISSED;
		}
	}
	return Gui::GuiEvent::ACTIVATED;
}

bool TextFieldManager::checkIfAnyActive () {
	if (activeField == nullptr)
		return false;
	return true;
}

Gui::GuiEvent TextFieldManager::manageTextInput (const sf::Event& ev) {
	if ((activeField != nullptr) && ev.type == sf::Event::TextEntered) {
		sf::Uint32 code = ev.text.unicode;
		if (code >= 32 && code <= 126) {
			char c = static_cast<char>(code);
			activeField->addCharToText (c);
			return Gui::GuiEvent::CHAR_TYPED;
		}
		if (code == 8) {
			activeField->eraseChar ();
			return Gui::GuiEvent::MISSED;
		}
		//tabulation
		if (code == 9){
			for (std::list<TextField*>::const_iterator iter = fieldsList.begin (); iter != fieldsList.end (); ++iter) {

				if ((*iter)->getIsActive ()) {
					(*iter)->setIsActive (false);
					++iter;
					if (iter == fieldsList.end ()) iter = fieldsList.begin ();
					(*iter)->setIsActive (true);
					activeField = (*iter);
					break;
				}

			}
			return Gui::GuiEvent::MISSED;
		}

	}
	return Gui::GuiEvent::MISSED;
}

std::vector<std::string> TextFieldManager::getData () {
	std::vector<std::string> data;
	for (auto i : fieldsList)
		data.push_back (i->getText ());
	return data;
}