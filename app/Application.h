#ifndef APPLICATION
#define APPLICATION
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include "Controller.h"
class Application {
public:
	Application ();
	~Application ();

	void initialize ();
	void run ();

private:
	sf::RenderWindow window;
	Controller controller;
};
#endif 