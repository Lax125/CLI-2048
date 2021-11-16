#pragma once
#include <iomanip>
#include <random>
#include <chrono>
#include "board.h"

Board::Board(size_t _width, size_t _height) : width(_width), height(_height),
	size(_width * _height), rng(std::chrono::system_clock::now().time_since_epoch().count()) {
	if (width == 0 || height == 0) {
		throw "Board has zero width or height";
	}
	tiles = new unsigned int[size];
	newlyCombined = new bool[size];
	std::fill(tiles, tiles + size, 0);
	std::fill(newlyCombined, newlyCombined + size, false);
	placeRandomTile();
}

Board::~Board() {
	delete[] tiles;
	delete[] newlyCombined;
}

bool Board::placeRandomTile() {
	if (nEmptyTiles == 0) return false;
	std::uniform_int_distribution<unsigned int> tileDist(1, nEmptyTiles);
	std::uniform_int_distribution<int> d10(1, 10);
	unsigned int n = tileDist(rng);
	for (size_t i = 0; i < size; i++) {
		if (tiles[i] == 0) {
			if (--n == 0) {
				if (d10(rng) == 10) tiles[i] = 2;
				else                tiles[i] = 1;
				nEmptyTiles--;
				latestTile = i;
				return true;
			}
		}
	}
	return false;
}

std::pair<int, bool> Board::push(direction d) {
	size_t tile0;
	int minorDelta, majorDelta;
	size_t minorIterations, majorIterations;
	bool* combinedTiles = new bool[size];
	std::fill(combinedTiles, combinedTiles + size, false);

	switch (d) {
	case direction::UP:
		tile0 = 0;
		minorDelta = width;
		majorDelta = 1;
		minorIterations = height - 1;
		majorIterations = width;
		break;
	case direction::DOWN:
		tile0 = width * (height - 1);
		minorDelta = -(int)width;
		majorDelta = 1;
		minorIterations = height - 1;
		majorIterations = width;
		break;
	case direction::LEFT:
		tile0 = 0;
		minorDelta = 1;
		majorDelta = width;
		minorIterations = width - 1;
		majorIterations = height;
		break;
	default:
		tile0 = width - 1;
		minorDelta = -1;
		majorDelta = width;
		minorIterations = width - 1;
		majorIterations = height;
		break;
	}

	bool changed = false;
	int scoreDelta = 0;
	size_t majorMarker = tile0;
	for (size_t i = 0; i < majorIterations; i++) {
		size_t gravityMarker = majorMarker;
		size_t searchMarker = majorMarker;
		for (size_t j = 0; j < minorIterations; j++) {
			searchMarker += minorDelta;
			auto& gravityTile = tiles[gravityMarker];
			auto& searchTile = tiles[searchMarker];
			if (searchTile == 0) continue;
			if (gravityTile == 0) {
				std::swap(searchTile, gravityTile);
				changed = true;
			}
			else if (gravityTile == searchTile) {
				gravityTile++;
				scoreDelta += 1 << gravityTile;
				searchTile = 0;
				nEmptyTiles++;
				combinedTiles[gravityMarker] = true;
				gravityMarker += minorDelta;
				changed = true;
			}
			else if (gravityMarker + minorDelta != searchMarker) {
				std::swap(tiles[gravityMarker + minorDelta], searchTile);
				gravityMarker += minorDelta;
				changed = true;
			}
			else {
				gravityMarker += minorDelta;
			}
		}
		majorMarker += majorDelta;
	}
	if (changed) std::copy(combinedTiles, combinedTiles + size, newlyCombined);
	delete[] combinedTiles;
	return { scoreDelta, changed };
}

bool Board::dead() const {
	if (nEmptyTiles > 0) return false;
	for (size_t i = 0; i < height; i++) {
		for (size_t j = 0; j < width - 1; j++) {
			if (tiles[i * width + j] == tiles[i * width + j + 1]) return false;
		}
	}
	for (size_t i = 0; i < width; i++) {
		for (size_t j = 0; j < height - 1; j++) {
			if (tiles[j * width + i] == tiles[(j + 1) * width + i]) return false;
		}
	}
	return true;
}

std::ostream& operator<<(std::ostream& os, const Board& board) {
	const char* const colors[] = {
		"\033[38;5;244m\033[48;5;244m",
		"\033[38;5;15m\033[48;5;44m",
		"\033[38;5;15m\033[48;5;75m",
		"\033[38;5;15m\033[48;5;105m",
		"\033[38;5;15m\033[48;5;135m",
		"\033[38;5;15m\033[48;5;165m",
		"\033[38;5;15m\033[48;5;170m",
		"\033[38;5;15m\033[48;5;175m",
		"\033[38;5;15m\033[48;5;180m",
		"\033[38;5;15m\033[48;5;149m",
		"\033[38;5;15m\033[48;5;40m",
		"\033[38;5;15m\033[48;5;196m",
	};
	const char* const numbers[] = {
		"       ",
		"   2   ",
		"   4   ",
		"   8   ",
		"   16  ",
		"   32  ",
		"   64  ",
		"  128  ",
		"  256  ",
		"  512  ",
		"  1024 ",
		"  2048 "
	};
	const char* const latestNumbers[] = {
		"       ",
		"   \033[4m2\033[24m   ",
		"   \033[4m4\033[24m   ",
		"   \033[4m8\033[24m   ",
		"   \033[4m16\033[24m  ",
		"   \033[4m32\033[24m  ",
		"   \033[4m64\033[24m  ",
		"  \033[4m128\033[24m  ",
		"  \033[4m256\033[24m  ",
		"  \033[4m512\033[24m  ",
		"  \033[4m1024\033[24m ",
		"  \033[4m2048\033[24m "
	};
	const char overColor[] = "\033[38;5;15m\033[48;5;236m";
	const char resetStyle[] = "\033[0m";
	os << "\t\033(0l";
	for (size_t i = 0; i < board.width * 7; i++) os << 'q';
	os << "k\033(B\n";
	for (size_t i = 0; i < board.height; i++) {
		os << "\t\033(0x\033(B";
		for (size_t j = 0; j < board.width; j++) {
			const size_t tileI = board.width * i + j;
			const unsigned int tile = board.tiles[tileI];
			if (tile < 12) os << colors[tile];
			else os << overColor;
			if (board.newlyCombined[tileI]) os << "\033(0l\033(B     \033(0k\033(B";
			else os << "       ";
		}
		os << resetStyle << "\033(0x\033(B\n";
		os << "\t\033(0x\033(B";
		for (size_t j = 0; j < board.width; j++) {
			const size_t tileI = board.width * i + j;
			const unsigned int tile = board.tiles[tileI];
			if (tile < 12) {
				os << colors[tile];
				if (tileI == board.latestTile) os << latestNumbers[tile];
				else os << numbers[tile];
			}
			else           os << overColor << "  2^" << tile << ' ';
		}
		os << resetStyle << "\033(0x\033(B\n";
		os << "\t\033(0x\033(B";
		for (size_t j = 0; j < board.width; j++) {
			const size_t tileI = board.width * i + j;
			const unsigned int tile = board.tiles[board.width * i + j];
			if (tile < 12) os << colors[tile];
			else os << overColor;
			if (board.newlyCombined[tileI]) os << "\033(0m\033(B     \033(0j\033(B";
			else os << "       ";
		}
		os << resetStyle << "\033(0x\033(B\n";
	}
	os << "\t\033(0m";
	for (size_t i = 0; i < board.width * 7; i++) os << 'q';
	os << "j\033(B\n";
	return os;
}
