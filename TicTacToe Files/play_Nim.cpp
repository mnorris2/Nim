// playNim.cpp
// This set of functions are used to actually play the game.
// Play starts with the function: playNim() which is defined below

#include "Nim.h"
#include <winsock2.h>
#include <iostream>
#include <string>

void main(int argc, char* argv[])
{
	int  choice;
	char choice_str[1024];
	char newline;
	char name[1024];

	WORD wVersionRequired = 0x0202;
	WSADATA wsaData;
	int iResult = WSAStartup(wVersionRequired, &wsaData);
	if (iResult != 0)
	{
		std::cout << "Unable to initialize Windows Socket library." << std::endl;
		exit(0);
	}

	std::cout << "Name?" << std::endl;
	std::cin >> name;

	do {
		std::cout << std::endl << std::endl << std::endl;
		std::cout << "Do you want to:" << std::endl;
		std::cout << "    (1) Host a game" << std::endl;
		std::cout << "    (2) Connect to someone else's game?" << std::endl;
		std::cout << "    (3) Quit" << std::endl;
		std::cout << "Enter 1, 2 or 3: ";
		std::cin >> choice_str; std::cin.get(newline);
		choice = atoi(choice_str);

		if (choice == 1)
			server_main(argc, argv, name);
		else if (choice == 2)
			client_main(argc, argv, name);
		else if (choice != 3)
			std::cout << std::endl << "Please enter a digit between 1 and 3." << std::endl;

	} while (choice != 3);
}

void initializeBoard(int rocks[], int piles, char board[][MAXROCKS])
{
	for (int i = 0; i < MAXPILES; i++)
	{
		for (int j = 0; j < MAXROCKS; j++)
		{
			board[i][j] = ' ';
		}
	}

	for (int i = 0; i < piles; i++)
	{
		for (int j = 0; j < rocks[i]; j++)
		{
			board[i][j] = '*';
		}
	}
}

void updateBoard(char board[][MAXROCKS], string move, int player, int rocksperpile[])
{
	int pilenumber;
	int rocks;
	int moveNumber = stoi(move);

	pilenumber = moveNumber / 100;
	pilenumber--;

	rocks = moveNumber % 100;

	for (int i = rocksperpile[pilenumber] - 1; i >= rocksperpile[pilenumber] - rocks; i--)
	{
		board[pilenumber][i] = ' ';
	}

	rocksperpile[pilenumber] -= rocks;
}

void displayBoard(char board[][MAXROCKS], int rocksperpiles[], int piles)
{
	for (int i = 0; i < piles; i++)
	{
		cout << "Pile " << i << ": ";

		for (int j = 0; j < rocksperpiles[i]; j++)
		{
			cout << board[i][j];
		}

		cout << std::endl;
	}
}

int check4Win(char board[][MAXROCKS], bool myMove)
{
	int winner;

	if (myMove)
	{
		winner = 1;
	}
	else
	{
		winner = 0;
	}

	for (int i = 0; i < MAXPILES; i++)
	{
		for (int j = 0; j < MAXROCKS; j++)
		{
			if (board[i][j] == '*')
			{
				winner = -1;
			}
		}
	}

	return winner;
}

string getMove(int piles, int rocksperpiles[],char board[][MAXROCKS], int player)
{
	int rocks;
	int pile;
	string move;

	do
	{
		cout << "What pile do you want to take from?" << endl;
		cin >> pile;

	} while (pile < 0 || pile >= piles);

	do
	{
		cout << "How many rocks do you want to take from pile " << pile << "?" << endl;
		cin >> rocks;

	} while (rocks < 0 || rocks > rocksperpiles[pile]);

	pile++;

	move += to_string(pile);

	if (rocks < 10)
	{
		move += "0";
	}

	move += to_string(rocks);

	return move;
}

void play_Nim(SOCKET s, bool isClient, char* serverName)
{
	// This function plays the game and returns the value

	const int ABORT = 2;
	const int CLIENT = 1;
	const int SERVER = 0;
	const int NOWINNER = -1;
	bool noWinner = true;
	int winner;
	int opponent;
	string move;
	bool myMove;
	int rocks;
	int piles = 0;
	int rocksperpiles[MAXPILES];
	char board[MAXPILES][MAXROCKS];
	int player;
	std::string message;

	//if isClient, client bit else server bit (pick rocks)
	if (isClient) {
		std::cout << "You go first" << std::endl;
		opponent = SERVER;
		player = CLIENT;
		myMove = true;
		char * setup = "";

		int waittime = wait(s, 20, 0);


		if (waittime != 0)
		{
			int len = recv(s, setup, MAX_RECV_BUF, 0);
			cout << setup << endl;

			if (setup[0] >= 3 && setup[0] <= 9)
			{
				piles = setup[0];
				cout << "Got Piles" << endl;
			}
			else
			{
				cout << "Incorrect Board!  Good Bye! :)" << endl;
				exit(0);
			}

			for (int i = 1; i <= strlen(setup); i += 2)
			{
				if (((setup[i] * 10) + setup[i + 1]) >= 1 && ((setup[i] * 10) + setup[i + 1] <= 20))
				{
					rocksperpiles[i - 1] = (setup[i] * 10) + setup[i + 1];
					cout << rocksperpiles[i - 1];
				}
				else
				{
					cout << "Incorrect Board!  Good Bye! :)" << endl;
					exit(0);
				}
			}
			cout << "Got rocks per piles" << endl;

			initializeBoard(rocksperpiles, piles, board);
		}
	}
	else {
		std::cout << "You go second" << std::endl;
		opponent = CLIENT;
		player = SERVER;
		myMove = false;

		do
		{
			std::cout << "Pick numbers of piles (Piles: 3-9)" << std::endl;
			cin >> piles;
		} while (piles < 3 || piles > 9);

		for (int i = 0; i < piles; i++)
		{
			do
			{
				std::cout << "How many rocks for pile #" << i << "?" << std::endl;
				cin >> rocks;
			} while (rocks < 1 || rocks > 20);

			rocksperpiles[i] = rocks;
		}

		initializeBoard(rocksperpiles, piles, board);

		message = to_string(piles);

		//send TCP of rocks going mn1n2n3 ex. m041507
		for (int i = 0; i < piles; i++)
		{
			if (rocksperpiles[i] < 10)
			{
				message += "0";
				message += to_string(rocksperpiles[i]);
			}
			else
			{
				message += to_string(rocksperpiles[i]);
			}
		}

		message += "\0";

		cout << message << endl;

		send(s, (char*)message.c_str(), message.length(), 0);
		cout << "Sending: " << message << std::endl;
	}

	while (noWinner) {
		if (myMove) {

			char decision;

			if (piles == 0)
			{
				exit(0);
			}

			do
			{
				displayBoard(board,rocksperpiles,piles);

				//Give user option to trash talk opponent or forfeit game
				cout << "Do you want to: " << endl;
				cout << "1. Make a move (m)" << endl;
				cout << "2. Make a snarky comment (c)" << endl;
				cout << "3. Forfeit the Game(f)" << endl << endl;

				cin >> decision;

				if (toupper(decision) == 'C')
				{
					string comment;
					message = 'C';

					cin.ignore();
					do
					{
						cout << "Type your wonderful comment.(Max: 80 characters)" << endl;
						getline(cin, comment);
					} while (comment.length() >= 80);

					message += comment;
					message += "\0";

					send(s, (char*)message.c_str(), message.length(), 0);
					cout << "Sending: " << message << endl;
				}
				else if (toupper(decision) == 'F')
					cout << "Sending: " << message << endl;
				{
					send(s, "F\0", 3, 0);

					noWinner = false;
					winner = opponent;
				}

			} while (toupper(decision) != 'M' || toupper(decision) == 'C');

			if (toupper(decision) == 'M')
			{
				// Get my move & display board
				move = getMove(piles, rocksperpiles, board, player);

				std::cout << "Board after your move:" << std::endl;
				updateBoard(board, move, player, rocksperpiles);
				displayBoard(board, rocksperpiles, piles);

				//TCP send it to opponent
				send(s, (char*)move.c_str(), move.length() + 1, 0);
				std::cout << "Sending: " << move << std::endl;
			}

		}
		else {
			//make move, send to other person
			std::cout << "Waiting for your opponent's move..." << std::endl << std::endl;
			//Get opponent's move & display board
			int status = wait(s, 50, 0);
			if (status != 0) {
				char buffer[MAX_RECV_BUF] = "";
				recv(s, buffer, MAX_RECV_BUF, 0);
				std::cout << "Received: " << buffer << std::endl;

				if (isdigit(buffer[0]))
				{
					updateBoard(board, buffer, opponent, rocksperpiles);
					displayBoard(board, rocksperpiles, piles);
				}
				else if (toupper(buffer[0]) == 'C')
				{
					for (int i = 0; i < (strlen(buffer) + 1); i++)
					{
						cout << buffer[i];
					}

					cout << endl;
				}
				else if (toupper(buffer[0]) == 'F')
				{
					noWinner = false;
					winner = player;
				}

			}
			else {
				winner = ABORT;
			}
		}

		myMove = !myMove;

		if (winner == ABORT) {
			std::cout << "No response from opponent.  Aborting the game..." << std::endl;
		}
		else {
			//After move, check for wins or losses
			winner = check4Win(board, myMove);
			if (winner != -1)
			{
				noWinner = false;
			}
		}


		//If win or loss, notify user and stop game
		if (winner == player)
			std::cout << "You WIN!" << std::endl;
		else if (winner == opponent)
			std::cout << "I'm sorry.  You lost" << std::endl;
	}
}
	//If opposing player move is invalid or connection is lost, terminate game