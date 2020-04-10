// player 1 info window
#define PLAYER1_INFO_WINDOW_X 2
#define PLAYER1_INFO_WINDOW_Y 0
#define PLAYER1_INFO_WINDOW_HEIGHT 1
#define PLAYER1_INFO_WINDOW_WIDTH 35
// player 1 card window
#define PLAYER1_WINDOW_X 2
#define PLAYER1_WINDOW_Y 1
#define PLAYER1_WINDOW_WIDTH PLAYER1_INFO_WINDOW_WIDTH
#define PLAYER1_WINDOW_HEIGHT 7
// the play window
#define PLAY_WINDOW_X 2
#define PLAY_WINDOW_Y PLAYER1_WINDOW_Y+PLAYER1_WINDOW_HEIGHT
#define PLAY_WINDOW_WIDTH PLAYER1_WINDOW_WIDTH
#define PLAY_WINDOW_HEIGHT PLAYER1_WINDOW_HEIGHT
#define PLAY_WINDOW_PLAYER2_X 1
#define PLAY_WINDOW_PLAYER1_X 10
// player 2 info window
#define PLAYER2_INFO_WINDOW_X PLAYER1_INFO_WINDOW_X
#define PLAYER2_INFO_WINDOW_Y PLAY_WINDOW_Y+PLAY_WINDOW_HEIGHT
#define PLAYER2_INFO_WINDOW_HEIGHT PLAYER1_INFO_WINDOW_HEIGHT
#define PLAYER2_INFO_WINDOW_WIDTH PLAYER1_INFO_WINDOW_WIDTH
// player 2 card window
#define PLAYER2_WINDOW_X PLAYER1_WINDOW_X
#define PLAYER2_WINDOW_Y PLAYER2_INFO_WINDOW_Y+PLAYER2_INFO_WINDOW_HEIGHT
#define PLAYER2_WINDOW_WIDTH PLAYER1_INFO_WINDOW_WIDTH
#define PLAYER2_WINDOW_HEIGHT PLAYER1_WINDOW_HEIGHT
// starter card window
#define STARTER_WINDOW_X PLAYER1_WINDOW_WIDTH+PLAYER1_WINDOW_X+2
#define STARTER_WINDOW_Y PLAYER1_WINDOW_Y
#define STARTER_WINDOW_WIDTH 10
#define STARTER_WINDOW_HEIGHT PLAYER1_WINDOW_HEIGHT
// log window
#define LOG_WINDOW_X STARTER_WINDOW_X
#define LOG_WINDOW_Y PLAY_WINDOW_Y
#define LOG_WINDOW_WIDTH 35
#define LOG_WINDOW_HEIGHT 13
// score area window
#define SCORE_WINDOW_X STARTER_WINDOW_X+STARTER_WINDOW_WIDTH
#define SCORE_WINDOW_Y STARTER_WINDOW_Y
#define SCORE_WINDOW_WIDTH 15
#define SCORE_WINDOW_HEIGHT STARTER_WINDOW_HEIGHT

#include <ncursesw/curses.h>
#include "NPlayer.h"
#include "Board.h"

class LocalBoard : public Board {
    protected:
        WINDOW * player1Window_; // window for the player 1's current cards
		WINDOW * player2Window_; // window for the player 2's current cards
		WINDOW * playWindow_;	// window for the "play"
		WINDOW * scoreWindow_;	// window keeping track of the score
		WINDOW * starterWindow_;	// window for the starter card
		WINDOW * player1InfoWindow_;	// window to update player 1 what to do
		WINDOW * player2InfoWindow_;	// window to show what player 2 is doing
		WINDOW * helpWindow_;	// window with what each key does
		WINDOW * logWindow_;	// window displays logs of what is going on in the game 

        NPlayer * nplayer1_;
        NPlayer * nplayer2_;

    public:
        LocalBoard(NPlayer * p1, NPlayer * p2);
        void dealRound();
        void startGameRound();
        void initialDiscard();
        void cutStarterCard(); // just deal out one card locally
        int playRound(unsigned short int round);
        void endPlay();
        void countHandScores();
        /**
         *  Stuff to update windows  
         */
        void displayHelpWindow();
        void updatePlayer1InfoWindow(std::string s);
		void updatePlayer2InfoWindow(std::string s);
		void updateScoreWindow();
		void updateLog(std::string s);
        void displayHand(WINDOW * win, std::vector<Card> cards);

        int checkWin();

};

