#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include <string>
#include "NPlayer.h"

class HumanPlayer : public NPlayer {
	public:
		HumanPlayer(std::string name);	
		std::vector<Card> discardCards();
		Card playCard(int count, int max);
};

#endif /* HUMANPLAYER_H */
