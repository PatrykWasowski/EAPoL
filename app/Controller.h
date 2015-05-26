#ifndef CONTROLLER
#define CONTROLLER

#include "GUI\ButtonManager.h"
#include "GUI\TextFieldManager.h"
#include "GUI\Console.h"
#include "GUI\ChoicePanel.h"
#include <mutex>
#include <condition_variable>


class Controller {
public:
	Controller ();
	~Controller ();
	void initManagers(sf::RenderWindow& wind);
	void draw ();
	void manageEvents (const sf::Event& event);
	Console& getConsole ();
	void sendMessagge (const std::string& msg);
	void setCriticalSection (std::mutex*m, bool* r, std::condition_variable* c);
	void addOption (const std::string& opt);
	std::string getOption ();
	std::vector<std::string> getData ();

	bool running ();
	bool connection ();



private:
	ButtonManager btnManager;
	TextFieldManager txtFldManager;
	Console console;
	ChoicePanel choicePanel;
	void debugEvent (std::string from, Gui::GuiEvent event);
	void performAction (const Gui::GuiEvent& event);
	int debug = 0;
	
	std::mutex runMutex;
	std::mutex* mtx;
	std::condition_variable* cv;

	bool* ready;
	bool r = true;
	bool c = false;
	void connect ();

};

#endif
