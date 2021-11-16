#pragma once
#include "board.h"
#include "direction.h"

class Game {
private:
	Board board;
	int score = 0;
public:
	Game(size_t width, size_t height);
	int currentScore() const;
	bool swipe(direction);
	bool over() const;
	void display() const;
};
