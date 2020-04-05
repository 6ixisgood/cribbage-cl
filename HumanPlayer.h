#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include <string>
#include "Player.h"

class HumanPlayer : public Player {
	public:
		HumanPlayer(std::string name);	
		std::tuple<Card, Card> discardToCrib();
		Card playCard(int count);
};

#endif /* HUMANPLAYER_H */
