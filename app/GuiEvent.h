#ifndef GUIEVENT
#define GUIEVENT
namespace Gui {
	enum GuiEvent {
		MISSED,
		ACTIVATED,
		CONNECT,
		DISCONNECT,
		CHAR_TYPED,
		CLOSE,
		MINIMIZE,
		SCROLL_DOWN,
		SCROLL_UP
	};
}
#endif