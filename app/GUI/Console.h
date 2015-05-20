#ifndef CONSOLE
#define CONSOLE
#include <SFML/graphics.hpp>
#include <vector>
#include <string>
#include <mutex>
#include "Button.h"
#include "../GuiEvent.h"
class Console {

public:
	
	Console (const sf::Vector2f& position, const int& messageBuffer, const int& msgToDisplay);
	Console ();
	~Console ();

	void associateWindow (sf::RenderWindow& wind);
	void addMessage (const std::string& msg);

	Gui::GuiEvent checkButtonsClicked (const sf::Event& event);

	void reactOnButton (const Gui::GuiEvent& event);

	void draw ();

	void clear ();


private:
	// basic stuff
	sf::RenderWindow* window;
	sf::RectangleShape background;
	sf::Font font;
	Button up;
	Button down;
	// data
	std::vector <std::string*> messages;
	
	//display
	std::vector <sf::Text*> displayText;
	
	//counters
	int firstToDisplay;
	int messageCounter;

	//Critical section:
	std::mutex mutex;

	void assignMessagesToDisplay ();
};
#endif

