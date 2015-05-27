#ifndef BUTTON_MANAGER
#define BUTTON_MANAGER

#include <SFML/Graphics.hpp>
#include <vector>
#include "Button.h"

class ButtonManager {
public:
	ButtonManager ();
	~ButtonManager ();
	void associateWindow (sf::RenderWindow& wind);
	void createButtons ();
	void draw ();
	Gui::GuiEvent checkButtonsClicked (sf::Event event);
	void changeBtnActivity (const int& position, const bool& activity);

private:
	std::vector <Button*> buttonList;
	sf::RenderWindow* window;
};

#endif