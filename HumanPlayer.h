#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include <string>
#include "Player.h"

class HumanPlayer : public Player {
	public:
		HumanPlayer(std::string name);	
		std::vector<Card> discardCards();
		Card playCard(int count, int max);
};

#endif /* HUMANPLAYER_H */
