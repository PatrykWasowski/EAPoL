#ifndef BUTTON_MANAGER
#define BUTTON_MANAGER

#include <SFML/Graphics.hpp>
#include <list>
#include "Button.h"

class ButtonManager {
public:
	ButtonManager ();
	~ButtonManager ();
	void associateWindow (sf::RenderWindow& wind);
	void createButtons ();
	void draw ();
	Gui::GuiEvent checkButtonsClicked (sf::Event event);

private:
	std::list <Button*> buttonList;
	sf::RenderWindow* window;
};

#endif