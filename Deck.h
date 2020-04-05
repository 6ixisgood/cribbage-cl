#ifndef DECK_H
#define DECK_H

#include "Card.h"
#include <string>
#include <stack>
#include <vector>
class Deck {
	private:
		std::vector<Card> cards_ { Card(1, "A", "H"), Card(2, "2", "H"), Card(3, "3", "H"), Card(4, "4", "H"), Card(5, "5", "H"), 
							Card(6, "6", "H"), Card(7, "7", "H"), Card(8, "8", "H"), Card(9, "9", "H"), Card(10, "10", "H"),
							Card(10, "J", "H"), Card(10, "Q", "H"), Card(10, "K", "H"), 
							Card(1, "A", "S"), Card(2, "2", "S"), Card(3, "3", "S"), Card(4, "4", "S"), Card(5, "5", "S"), 
							Card(6, "6", "S"), Card(7, "7", "S"), Card(8, "8", "S"), Card(9, "9", "S"), Card(10, "10", "S"),
							Card(10, "J", "S"), Card(10, "Q", "S"), Card(10, "K", "S"),
							Card(1, "A", "C"), Card(2, "2", "C"), Card(3, "3", "C"), Card(4, "4", "C"), Card(5, "5", "C"), 
							Card(6, "6", "C"), Card(7, "7", "C"), Card(8, "8", "C"), Card(9, "9", "C"), Card(10, "10", "C"),
							Card(10, "J", "C"), Card(10, "Q", "C"), Card(10, "K", "C"),
							Card(1, "A", "D"), Card(2, "2", "D"), Card(3, "3", "D"), Card(4, "4", "D"), Card(5, "5", "D"), 
							Card(6, "6", "D"), Card(7, "7", "D"), Card(8, "8", "D"), Card(9, "9", "D"), Card(10, "10", "D"),
							Card(10, "J", "D"), Card(10, "Q", "D"), Card(10, "K", "D") };
		std::stack<Card> deck_;

	public:
		Card dealOne();
		void reshuffle();
		bool isEmpty();

		
};

#endif /* DECK_H */
