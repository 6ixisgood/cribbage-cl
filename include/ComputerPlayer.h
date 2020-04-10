#ifndef COMPUTERPLAYER_H
#define COMPUTERPLAYER_H

#include <string>
#include "NPlayer.h"

class ComputerPlayer : public NPlayer {
	public:
		ComputerPlayer(std::string name);
		std::vector<Card> discardCards();
		Card playCard(int score, int max);		
};

#endif /* COMPUTERPLAYER_H */
