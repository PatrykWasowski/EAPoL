#ifndef TEXT_FIELD_MANAGER
#define TEXT_FIELD_MANAGER

#include <list>
#include "TextField.h"
#include <vector>
#include <iostream>
#include <fstream>
class TextFieldManager {
public:
	TextFieldManager ();
	~TextFieldManager ();
	
	void associateWindow (sf::RenderWindow& wind);
	void createFields ();

	void draw ();

	Gui::GuiEvent checkActivation (const sf::Event& ev);
	Gui::GuiEvent manageTextInput (const sf::Event& ev);
	bool checkIfAnyActive ();
	
	/*
	order of data:
	1) login
	2) password
	3) mac address
	*/
	std::vector<std::string> getData ();

	void setMacAddress (std::string& mac);
	void addCharToMac (char& c);

private:
	std::list <TextField*> fieldsList;
	TextField* activeField = nullptr;
	sf::RenderWindow* window;
	sf::Font font;

	
};
#endif
