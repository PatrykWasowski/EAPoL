#ifndef CHOICEPANEL
#define CHOICEPANEL
#include <SFML/graphics.hpp>
#include <vector>
#include <string>
#include <mutex>
#include "Option.h"
#include "Button.h"
#include "../GuiEvent.h"
class ChoicePanel {

public:
	
	ChoicePanel (const sf::Vector2f& position, const int& msgToDisplay);
	ChoicePanel ();
	~ChoicePanel ();

	void associateWindow (sf::RenderWindow& wind);
	void addOption (const std::string& msg);

	Gui::GuiEvent checkButtonsClicked (const sf::Event& event);
	Gui::GuiEvent checkOptionsClicked (const sf::Event& event);
	void reactOnButton (const Gui::GuiEvent& event);
	int getOption ();


	void draw ();
	void clear ();


private:
	// basic stuff
	sf::RenderWindow* window;
	sf::RectangleShape background;
	sf::RectangleShape choice;
	Button up;
	Button down;
	sf::Font font;

	// data
	std::vector<Option*> options;
	Option* activeOption = nullptr;
	std::vector<sf::Vector2f*> textPositions;

	int firstToDisplay;
	int opts;
		
	std::mutex mutex;
	void assignMessagesToDisplay ();
};
#endif

