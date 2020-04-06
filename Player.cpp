#include <iostream>
#include <algorithm>
#include "Player.h"

using namespace std;

Player::Player(string name) : name_(name) {
	position_ = 0;
}

void Player::setPosition(int pos) { 
	position_ = pos; 
}


int Player::advancePosition(int spaces) {
	position_ += spaces;
	return position_;
}

void Player::addToHand(Card c) {
	hand_.push_back(c);
}

void Player::removeFromHand(Card c) {
	std::vector<Card>::iterator i = std::find(hand_.begin(), hand_.end(), c);
	if (i != hand_.end()) {
		hand_.erase(i);
	}

}

void Player::printHand() {
	std::cout << "Your Hand: ";
	int j=1;
	for (std::vector<Card>::iterator i = hand_.begin(); i != hand_.end(); ++i, j++) {
		std::cout << "[" << j << "]" << i->toString() << " ";
	}
	std::cout << "\n";
}

void Player::setPlayHand(vector<Card> cards) {
	playHand_ = cards;
}

void Player::printPlayHand() {
	std::cout << "Your Play Hand: ";
	int j=1;
	for (std::vector<Card>::iterator i = playHand_.begin(); i != playHand_.end(); ++i, j++) {
		std::cout << "[" << j << "]" << i->toString() << " ";
	}
	std::cout << "\n";
}

bool Player::isPlayHandEmpty() {
	return playHand_.empty();
}

void Player::removeFromPlayHand(Card c) {
	std::vector<Card>::iterator i = std::find(playHand_.begin(), playHand_.end(), c);
	if (i != playHand_.end()) {
		playHand_.erase(i);
	}

}

void Player::emptyHand() {
	this->hand_.clear();
}

void Player::emptyPlayHand() {
	this->playHand_.clear();
}

Card Player::getCardInHandAt(int index) {
	return hand_[index];
}

Card Player::getCardInPlayHandAt(int index) {
	return playHand_[index];
}

 


