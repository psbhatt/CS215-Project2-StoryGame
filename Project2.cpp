//----------------------------------------------------------------------------
//                  CS 215 - Project 2 - MiniMud
//----------------------------------------------------------------------------
// Author: Praneeth Bhatt
// Date: 3/21/2021
// Description:  Mini game 
// Assistance: I received help from no one.
//-----------------------------------------------------------
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include "gameutils.h"
using namespace std;

void printLogo() {
	cout << "+----------------------------------------+" << endl;
	cout << "|               MINI MUD                 |" << endl;
	cout << "|            Praneeth Bhatt              |" << endl;
	cout << "+----------------------------------------+" << endl;

}

string getInputFileName() {
	string fileName;
	cout << "Enter name of game to play: ";
	cin >> fileName;
	fileName += ".txt";
	return fileName;
}

void initItems(item itemList[], int& numItems) {
	for (int i = 0; i < numItems; i++) {
		itemList[i].shortDesc = "";
		itemList[i].longDesc = "";
	}
	numItems = 0;
}

void initRooms(room roomList[], int& numRooms) {
	for (int i = 0; i < numRooms; i++) {
		roomList[i].shortDesc = "";
		roomList[i].longDesc = "";
		roomList[i].north = roomList[i].south = roomList[i].east = roomList[i].west = NO_EXIT;
		roomList[i].numItems = MAX_ITEMS;
		initItems(roomList[i].items, roomList[i].numItems);

	}

}

void initWinData(winDataStruct& win) {
	win.winMessage = "";
	win.winRoom = NO_EXIT;
	win.numWinItems = MAX_ITEMS;
	initItems(win.winItem, win.numWinItems);
}

item removeItem(int index, item itemList[], int& numItems) {
	if ((index < 0) || (index > numItems - 1)) {
		string message = "removeItem: invalid index = " + to_string(index);
		gameAbort(message);
	}
	item removedItem = itemList[index];
	for (int i = index; i < numItems - 1; i++) {
		itemList[i] = itemList[i + 1];
	}
	numItems--;
	return removedItem;
}

void addItem(item newItem, item itemList[], int& numItems) {
	if (numItems + 1 > MAX_ITEMS) {
		gameAbort("addItem: maximum number of items exceeded!");
	}

	itemList[numItems] = newItem;
	numItems++;
}

void addItem(string sDesc, string lDesc, item itemList[], int& numItems) {
	item toAdd;
	toAdd.shortDesc = sDesc;
	toAdd.longDesc = lDesc;
	addItem(toAdd, itemList, numItems);
}

string getCmd(string& remain) {
	string command;
	cout << "===> ";
	if (cin.peek() == '\n') cin.ignore();
	getline(cin, command);
	command = split(command, remain);

	return command;
}

//----------------------------------------------------------------------------
//							makeExitList
//----------------------------------------------------------------------------
string makeExitList(room thisRoom) {
	string exitList = "You can go:";
	if (thisRoom.north != NO_EXIT) exitList += " north,";
	if (thisRoom.south != NO_EXIT) exitList += " south,";
	if (thisRoom.east != NO_EXIT) exitList += " east,";
	if (thisRoom.west != NO_EXIT) exitList += " west.";
	exitList[exitList.length() - 1] = '.'; // change last comma to a period
	return exitList;
} // makeExitList()

void doLook(room current, bool show) {
	if (show)
		cout << current.shortDesc << endl;
	else
		cout << current.longDesc << endl;

	if (current.numItems > 0) {
		cout << "You notice the following: ";
		for (int i = 0; i < current.numItems - 1; i++) {
			cout << current.items[i].shortDesc << ", ";
		}
		cout << current.items[current.numItems - 1].shortDesc << "." << endl;
	}

	cout << makeExitList(current) << endl;

}

void doLook(room current) {
	doLook(current, false);
}


int findItem(string itemName, item itemList[], int numItems) {
	int value = NOT_FOUND;
	for (int i = 0; i < numItems; i++) {
		if (itemList[i].shortDesc == itemName) {
			value = i;
		}
	}
	return value;
}

void doExamine(string givenName, item roomItems[], item invItems[], int numRoomItems, int numInvItems) {
	int result = findItem(givenName, roomItems, numRoomItems);
	if (!(result == NOT_FOUND))
		cout << roomItems[result].longDesc << endl;
	else {
		result = findItem(givenName, invItems, numInvItems);
		if (!(result == NOT_FOUND))
			cout << invItems[result].longDesc << endl;
		else
			cout << "You see no " << givenName << endl;
	}

}

void doTake(string givenName, item roomItems[], item invItems[], int& numRoomItems, int& numInvItems) {
	item hold;
	int result = findItem(givenName, roomItems, numRoomItems);
	if (!(result == NOT_FOUND) && (numInvItems < MAX_ITEMS)) {
		hold = removeItem(result, roomItems, numRoomItems);
		addItem(hold, invItems, numInvItems);
		cout << "You take the " << givenName << "." << endl;
	}
	else if (!(result == NOT_FOUND) && (numInvItems >= MAX_ITEMS)) {
		cout << "Your hands are full, you can't take that." << endl;
	}
	else {
		result = findItem(givenName, invItems, numInvItems);
		if (result == NOT_FOUND)
			cout << "You see no " << givenName << endl;
		else
			cout << "You already have the " << givenName << endl;

	}
}

void doDrop(string givenName, item roomItems[], item invItems[], int& numRoomItems, int& numInvItems) {
	item hold;
	int result = findItem(givenName, invItems, numInvItems);
	if (!(result == NOT_FOUND) && (numRoomItems < MAX_ITEMS)) {
		hold = removeItem(result, invItems, numInvItems);
		addItem(hold, roomItems, numRoomItems);
		cout << "You drop the " << givenName << "." << endl;
	}
	else if (!(result == NOT_FOUND) && (numRoomItems >= MAX_ITEMS)) {
		cout << "The room is full of junk, you can't drop that." << endl;
	}
	else {
		result = findItem(givenName, roomItems, numRoomItems);
		if (result != NOT_FOUND)
			cout << "You don't have the " << givenName << endl;
		else
			cout << "You have no" << givenName << endl;

	}

}

bool gameWon(room rooms[], int numRoomItems, winDataStruct& win) {

	for (int i = 0; i < win.numWinItems; i++) {
		if (findItem(win.winItem[i].shortDesc, rooms[win.winRoom].items, rooms[win.winRoom].numItems) == NOT_FOUND)
			return false;
	}
	return true;
}




//----------------------------------------------------------------------------
//							loadData
//----------------------------------------------------------------------------
void loadData(room rooms[], int& numRooms, winDataStruct& winData) {
	// variable declarations
	ifstream f;				// input file handle
	string tag;				// first word extracted from each line
	string filename;		// name of input file
	string data;			// data read from file using cleanGetline()
	int roomNdx = -1;		// index of the "current room" being read/populated
							// this should always be rumRooms-1 

	// initialize the rooms and winData
	initRooms(rooms, numRooms);
	initWinData(winData);

	// get input file name and open the input file; abort on failure
	filename = getInputFileName();
	f.open(filename);
	if (f.fail()) {
		gameAbort("Unable to open " + filename);
	}

	// read line-by-line and store data in the game data structures
	f >> tag;
	while (tag != "END:") {
		if (tag == "ROOM:") {
			numRooms++;		// starting a new room
			roomNdx++;		// increment every time numRooms is incremented
			cleanGetline(f, rooms[roomNdx].shortDesc); // short desc on remainder of ROOM: line
			cleanGetline(f, rooms[roomNdx].longDesc);  // long desc on next line by itself (no tag)
		}
		else if (tag == "ITEM:") {
			string shortDesc, longDesc;
			cleanGetline(f, shortDesc);
			cleanGetline(f, longDesc);
			addItem(shortDesc, longDesc, rooms[roomNdx].items, rooms[roomNdx].numItems);

		}
		else if (tag == "WIN_ITEM:") {
			cleanGetline(f, data);
			addItem(data, "", winData.winItem, winData.numWinItems);


		}
		else if (tag == "NORTH:")
			f >> rooms[roomNdx].north;
		else if (tag == "SOUTH:")
			f >> rooms[roomNdx].south;
		else if (tag == "EAST:")
			f >> rooms[roomNdx].east;
		else if (tag == "WEST:")
			f >> rooms[roomNdx].west;
		else if (tag == "WIN_ROOM:")
			f >> winData.winRoom;

		else if (tag == "REMARK:")
			cleanGetline(f, data);		// data is junk, throw it away
		else if (tag == "WIN_TEXT:")
			cleanGetline(f, winData.winMessage);
		else if (tag == "END:")
			f.close();
		else
			gameAbort("Unknown tag in " + filename + ": " + tag);


		// check for read failure and read the next tag
		if (f.fail())
			gameAbort("Failure while reading input file " + filename);
		f >> tag;
	}
} // loadData()




//----------------------------------------------------------------------------
//							doInventory
//----------------------------------------------------------------------------
// Given: the player's inventory (partial array of items)
// This is the game's response to the "inv"/"i" (inventory) command.
// Prints a message telling the player what he/she is holding.
//----------------------------------------------------------------------------
void doInventory(item item[], int numItems) {
	if (numItems == 0)
		cout << "You don't have anything.\n";
	else {
		cout << "You are holding: ";
		for (int i = 0; i < (numItems - 1); i++)		// for all but last item
			cout << item[i].shortDesc << ", ";			// print short desc with comma
		cout << item[numItems - 1].shortDesc << ".\n";  // for last item, use period and newline
	}
} // doInventory()

//----------------------------------------------------------------------------
//							goNorth
//----------------------------------------------------------------------------
// Given:    partial array of rooms
// Modifies: index of current room
// This is the game's response to the player entering the "north"/"n" command.
// When the north exit exists, changes the current room index and performs
// a "look" command with a short room description
void goNorth(int& currentRoom, room rooms[]) {
	if (rooms[currentRoom].north == NO_EXIT)
		cout << "You can't go north.\n";
	else {
		currentRoom = rooms[currentRoom].north;
		doLook(rooms[currentRoom], true); // true = show short room desc.
	}
} // goNorth()

void goSouth(int& currentRoom, room rooms[]) {
	if (rooms[currentRoom].south == NO_EXIT)
		cout << "You can't go south.\n";
	else {
		currentRoom = rooms[currentRoom].south;
		doLook(rooms[currentRoom], true); // true = show short room desc.
	}
}

void goEast(int& currentRoom, room rooms[]) {
	if (rooms[currentRoom].east == NO_EXIT)
		cout << "You can't go east.\n";
	else {
		currentRoom = rooms[currentRoom].east;
		doLook(rooms[currentRoom], true); // true = show short room desc.
	}
}

void goWest(int& currentRoom, room rooms[]) {
	if (rooms[currentRoom].west == NO_EXIT)
		cout << "You can't go west.\n";
	else {
		currentRoom = rooms[currentRoom].west;
		doLook(rooms[currentRoom], true); // true = show short room desc.
	}
}

void startUp(room roomList[], int& numRooms, item inventory[], int& invSize, winDataStruct& win) {
	numRooms = MAX_ROOMS;
	invSize = MAX_ITEMS;
	printLogo();

	loadData(roomList, numRooms, win);

	initItems(inventory, invSize);

	cout << "You fall asleep an dream of a place far away..." << endl;
	doLook(roomList[START_ROOM]);

}

 
//----------------------------------------------------------------------------
//							main
//----------------------------------------------------------------------------
int main() {
	// declare data structures
	int numRooms = MAX_ROOMS;
	int numInv = MAX_ITEMS;
	room rooms[MAX_ROOMS];
	item inventory[MAX_ITEMS];
	int currRoom = START_ROOM;
	winDataStruct winData;
	

	// declare other variables
	string cmd;					// command entered by user (first word of input)
	string remainder;			// remainder of command entered by user
	bool   gameWasWon = false;	// check with gameWon() as needed

	// start up: load game data from file and initialize player inventory
	startUp(rooms, numRooms, inventory, numInv, winData);
	// get first command input
	cmd = getCmd(remainder);

	// repeat until "exit" entered or the game is won
	while (cmd != "exit" && !gameWasWon) {
		if (cmd == "help")
			printHelp();
		// add cases for other commands and invocations

		else if (cmd == "look" || cmd == "l")
			doLook(rooms[currRoom]);

		else if (cmd == "inv" || cmd == "i")
			doInventory(inventory, numInv);

		else if (cmd == "exa" || cmd == "x")
			doExamine(remainder, rooms[currRoom].items, inventory, rooms[currRoom].numItems, numInv);

		else if (cmd == "take" || cmd == "t")
			doTake(remainder, rooms[currRoom].items, inventory, rooms[currRoom].numItems, numInv);

		else if (cmd == "drop" || cmd == "d")
			doDrop(remainder, rooms[currRoom].items, inventory, rooms[currRoom].numItems, numInv);

		else if (cmd == "north" || cmd == "n")
			goNorth(currRoom, rooms);

		else if (cmd == "south" || cmd == "s")
			goSouth(currRoom, rooms);

		else if (cmd == "east" || cmd == "e")
			goEast(currRoom, rooms);

		else if (cmd == "west" || cmd == "w")
			goWest(currRoom, rooms);

		else if (cmd == "help")
			printHelp();

		else if (cmd == "exit") {
			system("pause");
			return 0;
		}
		else // the user entered an unrecognized command
			cout << "What??\n";

		// check if game was won and print win message or get next user input
		gameWasWon = gameWon( rooms, rooms[currRoom].numItems, winData);
		if (gameWasWon)
			cout << winData.winMessage << endl;
		else
			cmd = getCmd(remainder);
	}

	system("pause");
	return 0;
}