#include "ChoicePanel.h"


ChoicePanel::ChoicePanel () {
}


ChoicePanel::~ChoicePanel () {
}

ChoicePanel::ChoicePanel (const sf::Vector2f& position, const int& msgToDisplay) :
up ("./Resources/up.png", sf::Vector2f (position.x + 465, position.y), Gui::GuiEvent::SCROLL_UP),
down ("./Resources/down.png", sf::Vector2f (position.x + 465, position.y + msgToDisplay * 16 - 30), Gui::GuiEvent::SCROLL_DOWN),
firstToDisplay (0),
opts (-1)
{
	font.loadFromFile ("./Resources/angleterre-book/angltrr.ttf");

	for (int i = 0; i < msgToDisplay; ++i) {
		textPositions.push_back (new sf::Vector2f (position.x, position.y + i * 16));
	}
	int a = textPositions.size ();

	background.setFillColor (sf::Color (45, 45, 45, 255));
	background.setPosition (position);
	background.setSize (sf::Vector2f (460.f, (float) msgToDisplay * 16));
}


void ChoicePanel::associateWindow (sf::RenderWindow& wind) {
	window = &wind; 
}

void ChoicePanel::addOption (const std::string& msg) {
	mutex.lock ();
	++opts;
	if (opts < (int)textPositions.size()) {
		options.push_back (new Option (font));
		options [opts]->setPostion (*textPositions [opts]);
		options [opts]->setText (msg);
	
		//if first option
		/*if (opts == 0) {
			activeOption = options [0];
			options [0]->checkClicked (background.getPosition ().x + 1, background.getPosition ().y + 1);
		}	*/	
	}
	else {
		options.push_back (new Option (font));
		options.back ()->setText (msg);
	}
	mutex.unlock ();
}

Gui::GuiEvent ChoicePanel::checkButtonsClicked (const sf::Event& event) {
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


Gui::GuiEvent ChoicePanel::checkOptionsClicked (const sf::Event& event) {
	if (opts < 0)
		return Gui::GuiEvent::MISSED;
	sf::Vector2f position;
	position.x = (float) event.mouseButton.x;
	position.y = (float) event.mouseButton.y;
	if (!(background.getGlobalBounds ().contains (position)))
		return Gui::GuiEvent::MISSED;
	int temp;
	Gui::GuiEvent ev = Gui::GuiEvent::MISSED;
	
	(opts < (int)textPositions.size ()) ? temp = opts+1 : temp = textPositions.size ();
	for (int i = firstToDisplay; i < firstToDisplay + temp; ++i) {
			if ((options [i]->checkClicked (position) != Gui::GuiEvent::MISSED) && (activeOption != options[i])) {
			ev = options [i]->checkClicked (position);
			if (activeOption != nullptr) 
				activeOption->resetActive ();
			activeOption = options [i];
			lastActive = i;
		}
	}
	return ev;
}



void ChoicePanel::draw () {
	window->draw (background);
	up.draw (window);
	down.draw (window);

	if (options.size ()>0){
		int temp;
		(opts < (int)textPositions.size ()) ? temp = opts : temp = (int)textPositions.size ()-1;
		for (int i = firstToDisplay; i <= firstToDisplay + temp; ++i) {
			options [i]->draw (window);
		}
	}
}

void ChoicePanel::reactOnButton (const Gui::GuiEvent& event) {
	if (event == Gui::GuiEvent::SCROLL_DOWN) {
		mutex.lock ();
		if ((firstToDisplay == 0) || (opts < (int)textPositions.size ())) {
			mutex.unlock ();
			return;
		}
		--firstToDisplay;
		mutex.unlock ();
		assignMessagesToDisplay ();
	}
	else {
		mutex.lock ();
		if ((firstToDisplay == ((int)options.size () - (int) textPositions.size ())) || (opts < (int) textPositions.size ())) {
			mutex.unlock ();
			return;
		}
		++firstToDisplay;
		mutex.unlock ();
		assignMessagesToDisplay ();
	}
}

int ChoicePanel::getOption () {
	for (int i = 0; i < (int)options.size(); i++) {
		if (activeOption == options [i])
			return i+1;
	}

	return 0;

}

void ChoicePanel::assignMessagesToDisplay () {
	mutex.lock ();
	for (int i = 0; i < (int)textPositions.size(); ++i) {
		options [firstToDisplay+i]->setPostion (*textPositions[i]);
	}
	mutex.unlock ();
}

void ChoicePanel::clear () {
	options.clear ();
	firstToDisplay = 0;
	opts = -1;
}


bool ChoicePanel::selectLastActive () {
	if (lastActive >= 0 && lastActive < (int) options.size ()) {
		activeOption = options [lastActive];
		activeOption->setActive ();
		return true;
	}
	return false;
}

void ChoicePanel::setLastActive (const int& lastAct) {
	lastActive = lastAct;
}

int ChoicePanel::getLastActive () {
	return lastActive;
}