#ifndef COMPUTERPLAYER_H
#define COMPUTERPLAYER_H

#include <string>
#include "Player.h"

class ComputerPlayer : public Player {
	public:
		ComputerPlayer(std::string name);
		std::vector<Card> discardCards();
		Card playCard(int score, int max);		
};

#endif /* COMPUTERPLAYER_H */
