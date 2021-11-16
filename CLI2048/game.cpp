#include <iostream>
#include "board.h"
#include "game.h"
#include "consoleutils.h"

Game::Game(size_t width, size_t height) : board(width, height) {}

int Game::currentScore() const {
	return score;
}

bool Game::swipe(direction d) {
	const auto pushResult = board.push(d);
	score += pushResult.first;
	if (pushResult.second) {
		board.placeRandomTile();
		return true;
	}
	else return false;
}

bool Game::over() const {
	return board.dead();
}

void Game::display() const {
	consoleCursorToHome();
	std::cout << "\n\n";
	std::cout << "\tScore: " << score << "\n\n";
	std::cout << board << std::flush;
}

