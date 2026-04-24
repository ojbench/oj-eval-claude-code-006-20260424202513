#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <utility>

extern int rows;         // The count of rows of the game map.
extern int columns;      // The count of columns of the game map.
extern int total_mines;  // The count of mines of the game map.

#include <vector>

extern int rows;         // The count of rows of the game map.
extern int columns;      // The count of columns of the game map.
extern int total_mines;  // The count of mines of the game map.

int map_info[35][35]; // -1: ?, 0-8: number, 9: @
bool processed[35][35];

// You MUST NOT use any other external variables except for rows, columns and total_mines.

/**
 * @brief The definition of function Execute(int, int, bool)
 *
 * @details This function is designed to take a step when player the client's (or player's) role, and the implementation
 * of it has been finished by TA. (I hope my comments in code would be easy to understand T_T) If you do not understand
 * the contents, please ask TA for help immediately!!!
 *
 * @param r The row coordinate (0-based) of the block to be visited.
 * @param c The column coordinate (0-based) of the block to be visited.
 * @param type The type of operation to a certain block.
 * If type == 0, we'll execute VisitBlock(row, column).
 * If type == 1, we'll execute MarkMine(row, column).
 * If type == 2, we'll execute AutoExplore(row, column).
 * You should not call this function with other type values.
 */
void Execute(int r, int c, int type);

/**
 * @brief The definition of function InitGame()
 *
 * @details This function is designed to initialize the game. It should be called at the beginning of the game, which
 * will read the scale of the game map and the first step taken by the server (see README).
 */
void InitGame() {
  // TODO (student): Initialize all your global variables!
  for (int i = 0; i < 35; ++i) {
    for (int j = 0; j < 35; ++j) {
      map_info[i][j] = -1;
      processed[i][j] = false;
    }
  }
  int first_row, first_column;
  std::cin >> first_row >> first_column;
  Execute(first_row, first_column, 0);
}

/**
 * @brief The definition of function ReadMap()
 *
 * @details This function is designed to read the game map from stdin when playing the client's (or player's) role.
 * Since the client (or player) can only get the limited information of the game map, so if there is a 3 * 3 map as
 * above and only the block (2, 0) has been visited, the stdin would be
 *     ???
 *     12?
 *     01?
 */
void ReadMap() {
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      char c;
      std::cin >> c;
      if (c == '?') map_info[i][j] = -1;
      else if (c == '@') map_info[i][j] = 9;
      else if (c >= '0' && c <= '8') map_info[i][j] = c - '0';
      else if (c == 'X') map_info[i][j] = 9; // Should not happen in ReadMap for advanced
    }
  }
}

/**
 * @brief The definition of function Decide()
 *
 * @details This function is designed to decide the next step when playing the client's (or player's) role. Open up your
 * mind and make your decision here! Caution: you can only execute once in this function.
 */
void Decide() {
  // 1. Basic reasoning: check all visited non-mine grids
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      if (map_info[i][j] >= 0 && map_info[i][j] <= 8) {
        int unknown_count = 0;
        int marked_count = 0;
        std::vector<std::pair<int, int>> unknown_neighbors;
        for (int di = -1; di <= 1; ++di) {
          for (int dj = -1; dj <= 1; ++dj) {
            if (di == 0 && dj == 0) continue;
            int ni = i + di, nj = j + dj;
            if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
              if (map_info[ni][nj] == -1) {
                unknown_count++;
                unknown_neighbors.push_back({ni, nj});
              } else if (map_info[ni][nj] == 9) {
                marked_count++;
              }
            }
          }
        }

        // Rule 1: All unknown neighbors are mines
        if (unknown_count > 0 && map_info[i][j] == unknown_count + marked_count) {
          Execute(unknown_neighbors[0].first, unknown_neighbors[0].second, 1);
          return;
        }

        // Rule 2: All unknown neighbors are safe
        if (unknown_count > 0 && map_info[i][j] == marked_count) {
          Execute(i, j, 2); // AutoExplore will visit all safe neighbors
          return;
        }
      }
    }
  }

  // 2. Strong reasoning (Subset / Intersection logic could go here)
  // For now, let's try a simple random click if no logic works
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      if (map_info[i][j] == -1) {
        Execute(i, j, 0);
        return;
      }
    }
  }
}

#endif