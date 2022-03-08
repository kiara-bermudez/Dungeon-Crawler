#include <iostream>
#include <fstream>
#include <string>
#include "logic.h"
#include <sstream>

using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::istringstream;

/**
 * TODO: Student implement this function
 * Load representation of the dungeon level from file into the 2D map.
 * Calls createMap to allocate the 2D array.
 * @param   fileName    File name of dungeon level.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference to set starting position.
 * @return  pointer to 2D dynamic array representation of dungeon map with player's location., or nullptr if loading fails for any reason
 * @updates  maxRow, maxCol, player
 */
char** loadLevel(const string& fileName, int& maxRow, int& maxCol, Player& player) {

    string dimensions = "";       // Line with the map dimensions
    string playerStart = "";      // Line with the player starting row and column
    int startRow = -1;            // Player starting row
    int startCol = -1;            // Player starting column
    string line = "";             // Line with row of map 
    int row = 0;                  // Row of map number
    char space = ' ';             // Symbol at certain space on map
    bool door = false;            // Is true if level has door
    bool exit = false;            // IS true if level has exit
    char checkCol = 'x';          // Variable to check if there are too many columns

    ///// ------------- File Stream and Open File ------------- /////

	ifstream fin(fileName);                     // Create file stream

	if (!fin.is_open()) {                       // Check whether you can open file, if not return nullptr
		return nullptr;
	}


    ///// ------------- Dimensions ------------- /////
    
    getline(fin, dimensions);                   // Get first line (should be row and column size)

    if (dimensions == "") {                     // Check whether line is empty
        return nullptr;
    }

    istringstream sin(dimensions);              // Create string stream for dimensions

    sin >> maxRow;                              // Get max row

    if (sin.fail()) {                           // Check maxRow is non int
        return nullptr;
    }                         

    if (maxRow <= 0) {                          // Check if maxRow is negative or 0
        return nullptr;
    }

    sin >> maxCol;                              // Get max col

    if (sin.fail()) {                           // Check if max col is non int
        return nullptr;
    }

    if (maxCol <= 0) {                          // Check if maxCow is negative or 0
        return nullptr;
    }

    

    if (!(maxRow < INT32_MAX) || !(maxCol < INT32_MAX)) {   // Check if max row or max col is greater than 2147483647
        return nullptr;
    }
    if (maxRow > INT32_MAX / maxCol) {          // Check if number of tiles is greater than 2147483647
        return nullptr;
    }


    ///// ------------- Player Start Position ------------- /////
    
    getline(fin, playerStart);                  // Get second line (should be player starting row and column)

    if (playerStart == "") {                    // Check whether playerstart line is empty
        return nullptr;
    }

    istringstream sin2(playerStart);            // Create string stream for player starting position

    sin2 >> startRow;                           // Get Player Start Row

    if (sin2.fail()) {                          // Check whether start Row is a non int
        return nullptr;
    }

    if (startRow < 0 || startRow >= maxRow) {   // Check if startRow is negative or greater than maxRow
        return nullptr;
    } 

    player.row = startRow;                      // Assign player.row with start row

    sin2 >> startCol;                           // Get Player Start Col

    if (sin2.fail()) {                          // Check whether start Col is a non int
        return nullptr;
    }

    if (startCol < 0 || startCol >= maxCol) {   // Check if startCol is negative, non num, or greater than maxCol
        return nullptr;
    }    

    player.col = startCol;                      // Assign player.col with start col


    ///// ------------- Create Map ------------- /////
    
    char** level = createMap(maxRow, maxCol);   // Create map for current level (makes map full of -)

    if (level == nullptr) {                     // Check if level is nullptr
        return nullptr;
    }
        

    ///// ------------- Creating level ------------- /////
    
    while (!fin.eof()) {                        // Loop through until end of file

        getline(fin, line);                     // Get current row (line)

        if (line == "") {                       // Check if line is empty
            deleteMap(level, maxRow);
            return nullptr;
        }

        if (row == maxRow) {                    // Check whether there are more rows than there are supposed to be
            deleteMap(level, maxRow);
            return nullptr;
        }

        istringstream sin3(line);               // Create stringstream for line

        for(int col = 0; col < maxCol; ++col) { // Loop through each column in line

            space = 'k';                        // Assign space with char (used to check whether char changes later)

            sin3 >> space;                      // Get current tile (space)

            if (sin3.fail()) {                  // Check whether string stream fails to get space
                deleteMap(level, maxRow);
                return nullptr;
            }
            
            if (space == 'k') {                 // Check whether there is not enough columns
                deleteMap(level, maxRow);       // If space has not changed from 'k', then that means sin3 was unable to get space
                return nullptr;                 
            }

            if (col + 1 == maxCol) {            // Check if you are on last column of map

                sin3 >> checkCol;               // Get tile in next column
                
                if (checkCol != 'x') {          // If checkCol has changed from default, then that means there
                    deleteMap(level, maxRow);   // is an extra column
                    return nullptr; 
                }
                
            }

            // Update map with correct tiles
            if (space == TILE_OPEN) {           // Open tile

                level[row][col] = TILE_OPEN;

            } else if (space == TILE_TREASURE) {// Treasure tile

                level[row][col] = TILE_TREASURE;

            } else if (space == TILE_AMULET) {  // Amulet tile

                level[row][col] = TILE_AMULET;

            } else if (space == TILE_MONSTER) { // Monster tile

                level[row][col] = TILE_MONSTER;

            } else if (space == TILE_PILLAR) {  // Pillar tile

                level[row][col] = TILE_PILLAR;

            } else if (space == TILE_DOOR) {    // Door tile

                level[row][col] = TILE_DOOR;

                if (level[startRow][startCol] != TILE_DOOR) {   // Check if player start tile is the tile with the door
                    door = true;                
                }

            } else if (space == TILE_EXIT) {    // Exit tile

                level[row][col] = TILE_EXIT;

                if (level[startRow][startCol] != TILE_EXIT) { // Check if player start tile is the tile with the exit
                    exit = true;
                }

            } else if (space == TILE_PLAYER) {  // If player tile on space, replace with open tile

                level[row][col] = TILE_OPEN;

            } else {                            // If there is a tile that does not match tile constant status

                deleteMap(level, maxRow);
                return nullptr;

            }

        }

        row += 1;                               // Increment row by 1 

    }

    if (row < maxRow) {                        // Check whether there are not enough rows
        deleteMap(level, maxRow);
        return nullptr;
    }

    level[startRow][startCol] = TILE_PLAYER;    // Assign player start place with player tile

    if (door == false && exit == false) {       // Check whether level has door or exit
        deleteMap(level, maxRow);
        return nullptr;
    } 

    return level;
}

/**
 * TODO: Student implement this function
 * Translate the character direction input by the user into row or column change.
 * That is, updates the nextRow or nextCol according to the player's movement direction.
 * @param   input       Character input by the user which translates to a direction.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @updates  nextRow, nextCol
 */
void getDirection(char input, int& nextRow, int& nextCol) {
    
    // Update rows and columns depending on keyboard input

    if (input == MOVE_UP) {                     // Move up

        nextRow -= 1;

    } else if (input == MOVE_LEFT) {            // Move left

        nextCol -= 1;

    } else if (input == MOVE_DOWN) {            // Move down

        nextRow += 1;

    } else if (input == MOVE_RIGHT) {           // Move right

        nextCol += 1;

    } else {                                    // Anything not up, down, left, or right does not move

        nextRow = nextRow;
        nextCol = nextCol;

    }

}

/**
 * TODO: [suggested] Student implement this function
 * Allocate the 2D map array.
 * Initialize each cell to TILE_OPEN.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @return  2D map array for the dungeon level, holds char type.
 */
char** createMap(int maxRow, int maxCol) {

    char** map = new char*[maxRow] {};     // create map 2D array

    for (int y = 0; y < maxRow; ++y) {     // create first dimension

        map[y] = new char[maxCol] {};       

        for (int x= 0; x < maxCol; ++x) {  // create second dimensions

            map[y][x] = TILE_OPEN;         // Make all tiles open

        }
    }

    return map;
}

/**
 * TODO: Student implement this function
 * Deallocates the 2D map array.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @return None
 * @update map, maxRow
 */
void deleteMap(char**& map, int& maxRow) {

    if (map != nullptr) {           // Check whether map is already nullptr

        // Delete and deallocate
        for(int y = 0; y < maxRow; ++y) {

            delete[] map[y];        // delete each column value in row

        }

        delete[] map;               // delete rows
    }
    
    map = nullptr;                  // Make map nullptr             
    maxRow = 0;                     // Make max row 0

}

/**
 * TODO: Student implement this function
 * Resize the 2D map by doubling both dimensions.
 * Copy the current map contents to the right, diagonal down, and below.
 * Do not duplicate the player, and remember to avoid memory leaks!
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height), to be doubled.
 * @param   maxCol      Number of columns in the dungeon table (aka width), to be doubled.
 * @return  pointer to a dynamically-allocated 2D array (map) that has twice as many columns and rows in size.
 * @update maxRow, maxCol
 */
char** resizeMap(char** map, int& maxRow, int& maxCol) {

	int oldMaxRow = maxRow;                         // Max Row before resize
	int oldMaxCol = maxCol;                         // Max Column before resize

    // Check for Incorrect Behavior
    if (map == nullptr) {                           // Check if map is nullptr

        return nullptr;

    } else if (maxRow <= 0 || maxCol <= 0) {        // Check if maxRow and maxCol is negative or 0

        return nullptr;

    } else if (maxRow*2 * maxCol*2 >= 2147483647) { // Check if number of tiles is greater than 2147483647

        return nullptr;

    }

    char** newMap = createMap(maxRow * 2, maxCol * 2); // Create new map with double maxRow and maxCol

    // Copy subarray A (part of map with player) into resized array

    for (int y = 0; y < maxRow; ++y) {              // create first dimension

        for (int x= 0; x < maxCol; ++x) {           // create second dimensions

            newMap[y][x] = map[y][x];               // Copy tiles from map to new resized map

        }
    }    

    // Copy subarray C (bottom left) into resized array

	int y = 0;
	int x = 0;
    for (int newy = maxRow; newy < maxRow * 2; ++newy) {     // create first dimension

		x = 0;

        for (int newx= 0; newx < maxCol; ++newx) {           // create second dimensions

			if (map[y][x] == TILE_PLAYER) {					 // Make player tile into open tile

				newMap[newy][newx] = '-';

			} else {

				newMap[newy][newx] = map[y][x];               // Copy tiles from map to new resized map

			}
			++x;                                             // Increment x by 1
        }
		++y;                                                 // Increment y by 1
    }     


    // Copy subarray B (top right) into resized array

	y = 0;
    for (int newy = 0; newy < maxRow; ++newy) {              // create first dimension

		x = 0;

        for (int newx= maxCol; newx < maxCol * 2; ++newx) {  // create second dimensions

			if (map[y][x] == TILE_PLAYER) {					 // Make player tile into open tile

				newMap[newy][newx] = '-';

			} else {

				newMap[newy][newx] = map[y][x];              // Copy tiles from map to new resized map

			}
			++x;                                             // Increment x by 1
        }
		++y;                                                 // Increment y by 1
    }  

    // Copy subarray D (bottom right) into resized array

	y = 0;
    for (int newy = maxRow; newy < maxRow  * 2; ++newy) {    // create first dimension

		x = 0;

        for (int newx= maxCol; newx < maxCol * 2; ++newx) {  // create second dimensions

			if (map[y][x] == TILE_PLAYER) {					 // Make player tile into open tile

				newMap[newy][newx] = '-';

			} else {

				newMap[newy][newx] = map[y][x];              // Copy tiles from map to new resized map

			}
			++x;                                             // Increment x by 1
        }
		++y;                                                 // Increment y by 1
    }   

    deleteMap(map, maxRow);         		                // Deallocate old map
    maxRow = oldMaxRow * 2;                                 // Double max Row
    maxCol = oldMaxCol * 2;                                 // Double max column

    return newMap;
}

/**
 * TODO: Student implement this function
 * Checks if the player can move in the specified direction and performs the move if so.
 * Cannot move out of bounds or onto TILE_PILLAR or TILE_MONSTER.
 * Cannot move onto TILE_EXIT without at least one treasure. 
 * If TILE_TREASURE, increment treasure by 1.
 * Remember to update the map tile that the player moves onto and return the appropriate status.
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object to by reference to see current location.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @return  Player's movement status after updating player's position.
 * @update map contents, player
 */
int doPlayerMove(char** map, int maxRow, int maxCol, Player& player, int nextRow, int nextCol) {

    int status = 0;                                         // Status of Move

    // Check for Incorrect Behaviors

    if (nextRow < 0 || nextRow >= maxRow) {                 // Check if nextRow is out of bounds

        nextRow = player.row;                               // Make next row current player row

        return status;

    } else if (nextCol < 0 || nextCol >= maxCol) {          // Check if nextCol is out of bounds

        nextCol = player.col;                               // Make next col current player col

        return status;
    }

    // Set status

    char space = map[nextRow][nextCol];                             // Current space/tile

    if (space == TILE_OPEN) {                                       // Open tile has status 1

        status = 1;

    } else if (space == TILE_TREASURE) {                            // Treasure tile has status 2

        player.treasure += 1;                                       // Increment player treasure by 1

        status = 2;

    } else if (space == TILE_AMULET) {                              // Amulet tile has status 3

        status = 3;

    } else if (space == TILE_MONSTER || space == TILE_PILLAR) {     // Monster or Pillar tile has status 0

        status = 0;

        nextRow = player.row;                                       // Don't move, stay in same place

        nextCol = player.col;

        return 0;

    } else if (space == TILE_DOOR) {                                // Door tile has status 4

        status = 4;

    } else if (space == TILE_EXIT && player.treasure > 0) {         // Exit tile with a player with a treasure has status 5

        status = 5;

    } else if (space == TILE_EXIT) {                                // Exit tile with player without treasure has status 0

        status = 0;

        nextRow = player.row;                                       // Don't move, stay in same place

        nextCol = player.col;

        return 0;

    }

    map[nextRow][nextCol] = TILE_PLAYER;                            // Update map with new position of player
    map[player.row][player.col] = TILE_OPEN;                        // Update map by removing old position of player
    player.row = nextRow;                                           // Update player row 
    player.col = nextCol;                                           // Update player col


    return status;
}

/**
 * TODO: Student implement this function
 * Update monster locations:
 * We check up, down, left, right from the current player position.
 * If we see an obstacle, there is no line of sight in that direction, and the monster does not move.
 * If we see a monster before an obstacle, the monster moves one tile toward the player.
 * We should update the map as the monster moves.
 * At the end, we check if a monster has moved onto the player's tile.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference for current location.
 * @return  Boolean value indicating player status: true if monster reaches the player, false if not.
 * @update map contents
 */
bool doMonsterAttack(char** map, int maxRow, int maxCol, const Player& player) {

    int row = player.row;       // Row you are checking
    int col = player.col;       // Column you are checking
    int oldRow = row;           // Row before checking current row
    int oldCol = col;           // Column before checking current column


    // Check up
    while (map[row][col] != TILE_PILLAR && row != 0) {          // Loop until pillar or edge of map

        row--;                                                  // Make row you are checking one up

        if (map[row][col] == TILE_MONSTER) {                    // Check whether tile is monster

            map[oldRow][oldCol] = TILE_MONSTER;                 // Make tile before current tile you are checking the monster

            map[row][col] = TILE_OPEN;                          // Make current tile open
        }
        oldRow = row;                                       
    }


    // Check down
    row = player.row;
    oldRow = row;
    while (map[row][col] != TILE_PILLAR && row != maxRow - 1) {  // Loop until pillar or edge of map

        row++;                                                   // Make row you are checking one down

        if (map[row][col] == TILE_MONSTER) {                     // Check whether tile is monster

            map[oldRow][oldCol] = TILE_MONSTER;                  // Make tile before current tile you are checking the monster

            map[row][col] = TILE_OPEN;                           // Make current tile open

        }
        oldRow = row;
    }


    // Check left
    col = player.col;
    oldCol = col;
    row = player.row;
    oldRow = row;
    while (map[row][col] != TILE_PILLAR && col != 0) {           // Loop until pillar or edge of map

        col--;                                                   // Make column you are checking one left

        if (map[row][col] == TILE_MONSTER) {                     // Check whether tile is monster

            map[oldRow][oldCol] = TILE_MONSTER;                  // Make tile before current tile you are checking the monster

            map[row][col] = TILE_OPEN;                           // Make current tile open

        }   
        oldCol = col;
    } 

    // Check right   
    col = player.col;
    oldCol = col;
    while (map[row][col] != TILE_PILLAR && col != maxCol - 1) {  // Loop until pillar or edge of map

        col++;                                                   // Make column you are checking one right

        if (map[row][col] == TILE_MONSTER) {                     // Check whether tile is monster

            map[oldRow][oldCol] = TILE_MONSTER;                  // Make tile before current tile you are checking the monster

            map[row][col] = TILE_OPEN;                           // Make current tile open

        }
        oldCol = col;
    } 

    
    if (map[player.row][player.col] == TILE_MONSTER) {           // Check whether monster reached player

        return true;

    }

    return false;
}
