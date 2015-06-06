#include "Console.h"


Console::Console () {
}


Console::~Console () {
}

Console::Console (const sf::Vector2f& position, const int& messageBuffer, const int& msgToDisplay):
	up ("./Resources/up.png", sf::Vector2f (position.x + 690, position.y + 5), Gui::GuiEvent::SCROLL_UP),
	down ("./Resources/down.png", sf::Vector2f (position.x + 690, position.y + msgToDisplay * 16 - 25), Gui::GuiEvent::SCROLL_DOWN),
	firstToDisplay (0),
	messageCounter (0)
{
	int buff = messageBuffer;
	// create space for messages
	if (messageBuffer < msgToDisplay)
		buff = msgToDisplay;
	for (int i = 0; i < buff; ++i)
		messages.push_back (new std::string(""));
	// crete texts
	sf::Text simpleText;
	simpleText.setColor (sf::Color (200, 200, 200, 255));
	font.loadFromFile ("./Resources/angleterre-book/angltrr.ttf");
	simpleText.setFont (font);
	simpleText.setCharacterSize (12);
	for (int i = 0; i < msgToDisplay; ++i) {
		simpleText.setPosition (position.x + 5, position.y + 5 + (i * 16));
		displayText.push_back (new sf::Text (simpleText));
	}
	background.setFillColor (sf::Color (30, 30, 30, 255));
	background.setPosition (position);
	background.setSize (sf::Vector2f (680.f, (float)10 + msgToDisplay * 16));
}


void Console::associateWindow (sf::RenderWindow& wind) {
	window = &wind; 
}

void Console::addMessage (const std::string& msg) {
	mutex.lock ();
	//if (messageCounter < displayText.size ())

	// move messages 
	for (int i = messages.size () - 1; i >= 1; --i) {
		(*messages [i]) = (*messages [i - 1]);
	}
	(*messages [0]) = msg;
	if (messageCounter < 20)
		messageCounter++;
	mutex.unlock ();

	assignMessagesToDisplay ();
}

Gui::GuiEvent Console::checkButtonsClicked (const sf::Event& event) {
	sf::Vector2f position;
	position.x = (float) event.mouseButton.x;
	position.y = (float) event.mouseButton.y;
	if (up.checkClicked (position) == Gui::GuiEvent::SCROLL_UP) {
		return Gui::GuiEvent::SCROLL_UP;
	}
	if (down.checkClicked (position) == Gui::GuiEvent::SCROLL_DOWN) {
		return Gui::GuiEvent::SCROLL_DOWN;
	}
	return Gui::GuiEvent::MISSED;
}

void Console::draw () {
	window->draw (background);
	up.draw (window);
	down.draw (window);

	for (unsigned i = 0; i < displayText.size (); i++) {
		window->draw (*displayText [i]);
	}

	// if more messages than display size
	/*if (messageCounter >= displayText.size ()) {
		for (int i = 0; i < displayText.size (); i++) {
			displayText [i]->setString ()
		}

	}
	else
*/
}

void Console::reactOnButton (const Gui::GuiEvent& event) {
	if (event == Gui::GuiEvent::SCROLL_DOWN) {
		mutex.lock ();
		if ((firstToDisplay == 0) || (messageCounter <= (int)displayText.size ())) {
			mutex.unlock ();
			return;

		}
		--firstToDisplay;
		mutex.unlock ();
		assignMessagesToDisplay ();
		
	}
	else {
		mutex.lock ();
		if (firstToDisplay == (messageCounter - (int)displayText.size()) || messageCounter <= (int)displayText.size ()) {
			mutex.unlock ();
			return;

		}
			++firstToDisplay;
		mutex.unlock ();
		assignMessagesToDisplay ();
	}
}



// only when button pressed or new message
void Console::assignMessagesToDisplay () {
	// todo

	int counter = firstToDisplay;
	mutex.lock ();
	for (int i = displayText.size () - 1; i >= 0; --i) 
		displayText [i]->setString (*(messages[counter++]));
	mutex.unlock ();
}
