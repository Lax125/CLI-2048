#pragma once
#include <random>
#include <iomanip>
#include "direction.h"

class Board {
private:
	const size_t width, height;
	const size_t size;
	size_t latestTile = 0;
	unsigned int* tiles;
	bool* newlyCombined;
	unsigned int nEmptyTiles = size;
	std::default_random_engine rng;

public:
	Board(size_t, size_t);
	~Board();
	bool placeRandomTile();
	std::pair<int, bool> push(direction);
	bool dead() const;
	friend std::ostream& operator<<(std::ostream&, const Board&);
};