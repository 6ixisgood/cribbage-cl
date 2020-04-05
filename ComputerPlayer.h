#ifndef COMPUTERPLAYER_H
#define COMPUTERPLAYER_H

#include <string>
#include "Player.h"

class ComputerPlayer : public Player {
	public:
		ComputerPlayer(std::string name);
		std::tuple<Card, Card> discardToCrib();
		Card playCard(int score);
		
};

#endif /* COMPUTERPLAYER_H */
