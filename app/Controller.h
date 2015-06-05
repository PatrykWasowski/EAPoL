#ifndef CONTROLLER
#define CONTROLLER

#include "GUI\ButtonManager.h"
#include "GUI\TextFieldManager.h"
#include "GUI\Console.h"
#include "GUI\ChoicePanel.h"
#include "../CriticalSectionPack.h"
#include <SFML/Window.hpp>
#include <Windows.h>



class Controller {
public:
	Controller ();
	~Controller ();
	void initManagers(sf::RenderWindow& wind);
	void draw ();
	void manageEvents (const sf::Event& event);
	Console& getConsole ();
	void sendMessagge (const std::string& msg);
	void setCriticalSection (CriticalSectionPack*, CriticalSectionPack*);
	void addOption (const std::string& opt);
	int getOption ();
	std::vector<std::string> getData ();

	void setButtonLocked (const int& id, const bool& lock);

	bool running ();
	bool getConnection ();
	bool getConnecting ();
	void clearOptions ();
	void selectLastPort ();
	void setLastPort (const int& lastPort);
	int getLastPort ();
	void setConnection (bool);
	void setConnecting (bool);


private:
	sf::RenderWindow* window;
	ButtonManager btnManager;
	TextFieldManager txtFldManager;
	Console console;
	ChoicePanel choicePanel;
	void debugEvent (std::string from, Gui::GuiEvent event);
	void performAction (const Gui::GuiEvent& event);

	std::mutex runMutex;
	
	CriticalSectionPack* connectPack;

	CriticalSectionPack* disconnectPack;
	std::mutex mtx;
	bool* ready;
	bool r = true;
	bool c = false;
	bool connecting = false;
	void connect ();
	void disconnect ();
};

#endif
