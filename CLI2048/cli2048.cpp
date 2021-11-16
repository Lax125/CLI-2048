#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>
#include <Windows.h>
#include <conio.h>
#include "direction.h"
#include "game.h"
#include "consoleutils.h"

direction getDir() {
	while (true) {
		int key = _getch();
		if (key == 3 || key == 27) throw 1;
		if (key == 224) {
			switch (_getch()) {
			case 72:
				return direction::UP;
			case 80:
				return direction::DOWN;
			case 75:
				return direction::LEFT;
			case 77:
				return direction::RIGHT;
			}
		}
	}
}

direction getRandDir(std::default_random_engine& rng) {
	std::uniform_int_distribution<int> directionDist(0, 3);
	return (direction)directionDist(rng);
}

bool exiting = false;

BOOL WINAPI consoleHandler(DWORD signal) {
	if (signal == CTRL_C_EVENT) exiting = true;
	return TRUE;
}

int main(int argc, char* argv[]) {
	size_t width = 4, height = 4;
	bool randomMoves = false;
	std::default_random_engine rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	unsigned long delayInterval = 100;
	int argi = 1;
	while (argi < argc) {
		if (strcmp(argv[argi], "-s") == 0 || strcmp(argv[argi], "--size") == 0) {
			argi++;
			if (argc - argi < 2) {
				std::cerr << "Too few arguments for option -s/--size (needed 2, got "
					<< argc - argi << ')' << std::endl;
				return EXIT_FAILURE;
			}
			size_t x = (size_t)strtoul(argv[argi], nullptr, 10);
			width = std::clamp(x, (size_t)1, (size_t)100);
			argi++;
			size_t y = (size_t)strtoul(argv[argi], nullptr, 10);
			height = std::clamp(y, (size_t)1, (size_t)100);
		}
		else if (strcmp(argv[argi], "-d") == 0 || strcmp(argv[argi], "--delay") == 0) {
			argi++;
			if (argc - argi < 1) {
				std::cerr << "Too few arguments for option -d/--delay (needed 1, got "
					<< argc - argi << ')' << std::endl;
				return EXIT_FAILURE;
			}
			unsigned long m = strtoul(argv[argi], nullptr, 10);
			delayInterval = std::clamp(m, 0UL, 1000UL);
		}
		else if (strcmp(argv[argi], "-r") == 0 || strcmp(argv[argi], "--random") == 0) {
			randomMoves = true;
		}
		else if (strcmp(argv[argi], "-h") == 0 || strcmp(argv[argi], "--help") == 0) {
			std::cout << " OPTION                     | EFFECT                " << std::endl;
			std::cout << "----------------------------+-----------------------" << std::endl;
			std::cout << " -h/--help                  | Show this table       " << std::endl;
			std::cout << " -s/--size <width> <height> | Specify board size    " << std::endl;
			std::cout << " -r/--random                | Autoplay randomly     " << std::endl;
			std::cout << " -d/--delay <milliseconds>  | Specify autoplay speed" << std::endl;
		}
		argi++;
	}
	Game game(width, height);
	SetConsoleCtrlHandler(consoleHandler, TRUE);
	enableVirtualTerminalProcessing();
	hideConsoleCursor();
	disableSync();
	clearConsole();
	while (true) {
		game.display();
		if (game.over()) {
			std::cout << "\tGame Over" << std::endl;
			break;
		}
		try {
			if (randomMoves) {
				Sleep(delayInterval);
				if (exiting) {
					std::cout << "\tStopping..." << std::endl;
					break;
				}
				while (!game.swipe(getRandDir(rng)));
			}
			else while (!game.swipe(getDir()));
		}
		catch (int) {
			std::cout << "\tGoodbye!" << std::endl;
			break;
		}
	}
	showConsoleCursor();
	return EXIT_SUCCESS;
}