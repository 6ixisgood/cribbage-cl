#ifndef CARD_H
#define CARD_H

#include <string> 
#include <map>
class Card {
	private: 
		int value_;
		std::string name_;
		std::string suit_;
		std::map<std::string, int> sortValue_ = {{"A", 1}, {"2", 2}, {"3", 3}, {"4", 4},
											{"5", 5}, {"6", 6}, {"7", 7}, {"8", 8},
											{"9", 9}, {"10", 10}, {"J", 11}, 
											{"Q", 12}, {"K", 13}};
	public:
		Card();
		Card(int value, std::string name, std::string suit);
		Card(const Card& origin);
		int value() const { return value_; }
		void setValue(int value);
		std::string name() { return name_; }
		void setName(std::string name);
		std::string suit() { return suit_; }
		void setSuit(std::string suit);
		int sortValue() const;
		std::string toString() { return name_ + suit_; }
		bool operator==(const Card& rhs) const;
		Card& operator=(const Card& rhs);
};

#endif /* CARD_H */
