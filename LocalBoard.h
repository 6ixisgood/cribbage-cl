#include "Board.h"


class LocalBoard : public Board {
    private:

    public:
        LocalBoard(Player * p1, Player * p2);
        void dealRound();
        void player2InitialDiscard();
        Card player2PlayCard(int count);
        void cutStarterCard();
        void displayPlayer2Hand();


};

