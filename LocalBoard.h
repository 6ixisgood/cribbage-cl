#include "Board.h"


class LocalBoard : public Board {
    private:

    public:
        LocalBoard(Player * p1, Player * p2);
        void dealRound();
        /**
         * Just deal out one card locally
         */
        void cutStarterCard();
};

