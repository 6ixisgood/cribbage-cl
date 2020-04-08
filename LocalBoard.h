#include "Board.h"


class LocalBoard : public Board {
    private:

    public:
        LocalBoard(Player * p1, Player * p2);
        void initGame();
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

        int checkWin();

};

