#ifndef CONTROLLER
#define CONTROLLER

#include "GUI\ButtonManager.h"
#include "GUI\TextFieldManager.h"
#include "GUI\Console.h"

class Controller {
public:
	Controller ();
	~Controller ();
	void initManagers(sf::RenderWindow& wind);
	void draw ();
	void manageEvents (const sf::Event& event);
	Console& getConsole ();
	void sendMessagge (const std::string& msg);

private:
	ButtonManager btnManager;
	TextFieldManager txtFldManager;
	Console console;
	void debugEvent (std::string from, Gui::GuiEvent event);
	void performAction (const Gui::GuiEvent& event);
	int debug = 0;
};

#endif
