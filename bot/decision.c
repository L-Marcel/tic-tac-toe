#include <stdio.h>
#include <stdbool.h>
#include "core.c"

bool matchGrid(int grid[SPACES], int toCompare[SPACES], int round) {
  int equalsOccurrences = 0;
  for(int i = 0; i < SPACES; i++) {
    if(grid[i] == toCompare[i]) {
      equalsOccurrences++;
    };
  };

  if(equalsOccurrences == round) {
    return true;
  };

  return false;
};

float getChance(PossibleResult results[MAX_RESULTS], int board[SPACES], int round, int choice) {
  int occurrences = 0;
  int notLoseOccurrences = 0;

  int boardAfterChoice[SPACES];
  memcpy(boardAfterChoice, board, sizeof(int) * SPACES);
  boardAfterChoice[choice] = BOT_ID;

  for(int i = 0; i < MAX_RESULTS; i++) {
    if(matchGrid(boardAfterChoice, results[i].grid, round)) {
      occurrences++;

      if(!results[i].botLose) {
        notLoseOccurrences++;
      };
    };
  };

  return notLoseOccurrences / occurrences;
};

int getBotDecision(int board[SPACES], int difficult, int round) {
  int grids[POSSIBILITIES][SPACES] = {0};
  setPossibilities(grids, POSSIBILITIES, 0);

  PossibleResult results[MAX_RESULTS];
  setWinPossibilities(grids, results);

  int bestChoice = -1;
  float chance = 0;

  for(int i = 0; i < SPACES; i++) {
    if(round < 8 && board[i] == -1 && (((chance < (0.25 * (difficult + 1)))) || (bestChoice == -1))) {
      float newChance = getChance(results, board, round + 1, i);

      //will win (task, in work)
      if(newChance >= chance) {
        chance = newChance;
        bestChoice = i;
      };
    } else if(round >= 8 && board[i] == -1) {
      return i;
    };
  };

  return bestChoice;
};