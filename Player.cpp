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

void Player::setPlayHand(vector<Card> cards) {
	playHand_ = cards;
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


 


