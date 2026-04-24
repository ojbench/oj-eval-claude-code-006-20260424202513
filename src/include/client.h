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

  // 2. Strong reasoning: Intersection/Subset logic
  // Compare neighbors of two numbered cells that share unknown neighbors.
  struct Cell { int r, c, val, unknown, marked; std::vector<std::pair<int, int>> neighbors; };
  std::vector<Cell> cells;
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      if (map_info[i][j] >= 0 && map_info[i][j] <= 8) {
        Cell cell = {i, j, map_info[i][j], 0, 0, {}};
        for (int di = -1; di <= 1; ++di) {
          for (int dj = -1; dj <= 1; ++dj) {
            if (di == 0 && dj == 0) continue;
            int ni = i + di, nj = j + dj;
            if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
              if (map_info[ni][nj] == -1) {
                cell.unknown++;
                cell.neighbors.push_back({ni, nj});
              } else if (map_info[ni][nj] == 9) {
                cell.marked++;
              }
            }
          }
        }
        if (cell.unknown > 0) cells.push_back(cell);
      }
    }
  }

  for (size_t a = 0; a < cells.size(); ++a) {
    for (size_t b = 0; b < cells.size(); ++b) {
      if (a == b) continue;
      // If neighbors of A are a subset of neighbors of B
      // The remaining mines in B must be at least (valA - markedA)
      // Actually simpler: if A's unknown neighbors are a subset of B's unknown neighbors
      bool is_subset = true;
      for (auto &nA : cells[a].neighbors) {
        bool found = false;
        for (auto &nB : cells[b].neighbors) {
          if (nA == nB) { found = true; break; }
        }
        if (!found) { is_subset = false; break; }
      }

      if (is_subset) {
        int mines_in_A = cells[a].val - cells[a].marked;
        int mines_in_B = cells[b].val - cells[b].marked;
        int extra_mines = mines_in_B - mines_in_A;
        int extra_unknowns = cells[b].unknown - cells[a].unknown;

        if (extra_unknowns > 0) {
          if (extra_mines == 0) {
            // All extra unknown neighbors in B are safe
            for (auto &nB : cells[b].neighbors) {
              bool inA = false;
              for (auto &nA : cells[a].neighbors) {
                if (nA == nB) { inA = true; break; }
              }
              if (!inA) {
                Execute(nB.first, nB.second, 0);
                return;
              }
            }
          } else if (extra_mines == extra_unknowns) {
            // All extra unknown neighbors in B are mines
            for (auto &nB : cells[b].neighbors) {
              bool inA = false;
              for (auto &nA : cells[a].neighbors) {
                if (nA == nB) { inA = true; break; }
              }
              if (!inA) {
                Execute(nB.first, nB.second, 1);
                return;
              }
            }
          }
        }
      }
    }
  }

  // 3. Last resort: select unknown grid with minimum probability of being a mine
  double min_prob = 1.1;
  int best_r = -1, best_c = -1;

  std::vector<std::pair<int, int>> unknown_grids;
  for (int i = 0; i < rows; ++i) {
    for (int j = 0; j < columns; ++j) {
      if (map_info[i][j] == -1) {
        unknown_grids.push_back({i, j});

        double prob = 1.0; // Default if no numbered neighbors
        bool has_numbered_neighbor = false;

        for (int di = -1; di <= 1; ++di) {
          for (int dj = -1; dj <= 1; ++dj) {
            if (di == 0 && dj == 0) continue;
            int ni = i + di, nj = j + dj;
            if (ni >= 0 && ni < rows && nj >= 0 && nj < columns) {
              if (map_info[ni][nj] >= 0 && map_info[ni][nj] <= 8) {
                int u = 0, m = 0;
                for (int ddi = -1; ddi <= 1; ++ddi) {
                  for (int ddj = -1; ddj <= 1; ++ddj) {
                    if (ddi == 0 && ddj == 0) continue;
                    int nni = ni + ddi, nnj = nj + ddj;
                    if (nni >= 0 && nni < rows && nnj >= 0 && nnj < columns) {
                      if (map_info[nni][nnj] == -1) u++;
                      else if (map_info[nni][nnj] == 9) m++;
                    }
                  }
                }
                if (u > 0) {
                  double p = (double)(map_info[ni][nj] - m) / u;
                  if (p < prob) prob = p;
                  has_numbered_neighbor = true;
                }
              }
            }
          }
        }

        if (!has_numbered_neighbor) prob = 0.5; // Slightly prefer grids near known ones

        if (prob < min_prob) {
          min_prob = prob;
          best_r = i;
          best_c = j;
        }
      }
    }
  }

  if (best_r != -1) {
    Execute(best_r, best_c, 0);
  }
}

#endif