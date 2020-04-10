#ifndef NPLAYER_H
#define NPLAYER_H

#include <Player.h>
#include <ncursesw/curses.h>

// An implimentation of a player 
class NPlayer : public Player {
    protected:
        WINDOW * playerWindow_; // window where player can interact with hand
    public:
        NPlayer(std::string name) : Player(name) {}
        void setPlayerWindow(WINDOW * win);
		WINDOW* playerWindow() { return playerWindow_; }
     	void displayHand(std::vector<Card> hand);
};

#endif /* NPLAYER_H */