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

bool playerIsTheFirst(int board[SPACES], int player, int enemy) {
  int playerPlays = 0;
  int enemyPlays = 0;

  for(int i = 0; i < SPACES; i++) {
    if(board[i] == player) {
      playerPlays++;
    } else if(board[i] == enemy) {
      enemyPlays++;
    };
  };

  if(playerPlays > enemyPlays) {
    return true;
  };

  return false;
};

float getChance(PossibleResult results[MAX_RESULTS], int board[SPACES], int round, int choice) {
  float occurrences = 0;
  float notLoseOccurrences = 0;

  int boardAfterChoice[SPACES];
  memcpy(boardAfterChoice, board, sizeof(int) * SPACES);
  boardAfterChoice[choice] = BOT_ID;

  for(int i = 0; i < MAX_RESULTS; i++) {
    if(
      matchGrid(boardAfterChoice, results[i].grid, round) && 
      playerIsTheFirst(results[i].grid, BOT_ID, PLAYER_ID) == playerIsTheFirst(boardAfterChoice, BOT_ID, PLAYER_ID)
    ) {
      occurrences++;

      if(!results[i].botLose) {
        notLoseOccurrences++;
      };
    };
  };

  //printf("\nGet chance: %f %f %.2f\n", notLoseOccurrences, occurrences, notLoseOccurrences / occurrences);
  if(notLoseOccurrences == 0 || occurrences == 0) {
    return 0;
  };
  
  return notLoseOccurrences / occurrences;
};

bool checkIfBotIsTheWinner(int board[SPACES], int choice) {
  int boardAfterChoice[SPACES];
  memcpy(boardAfterChoice, board, sizeof(int) * SPACES);
  boardAfterChoice[choice] = BOT_ID;

  bool botIsTheWinner = playerWin(boardAfterChoice, BOT_ID) && !playerWin(boardAfterChoice, PLAYER_ID);
  return botIsTheWinner;
};

int getBotDecision(int board[SPACES], int difficult, int round) {
  int grids[POSSIBILITIES][SPACES] = {0};
  setPossibilities(grids, POSSIBILITIES, 0);

  PossibleResult results[MAX_RESULTS];
  setWinPossibilities(grids, results);

  int bestChoice = -1;
  float chance = 0;
  bool botWillWin = false;
  bool easyModeIsEnabled = difficult <= 1;

  for(int i = 0; i < SPACES; i++) {
    if(round < 8 && board[i] == -1) {
      float newChance = getChance(results, board, round + 1, i);
      botWillWin = checkIfBotIsTheWinner(board, i) && difficult >= 2;

      //printf("Candidate: %f %d %d %d\n", newChance, i, newChance >= chance, (((newChance <= 0.2 + (0.2 * (difficult + 1)))) || (bestChoice == -1)));
      if((botWillWin || ((newChance >= chance) && !botWillWin)) && (((newChance <= 0.4 + (0.15 * (difficult + 1)))) || (bestChoice == -1))) {
        //printf("New best chance: %.2f%% %.2f%% %d %.2f%%\n", chance * 100.0, newChance * 100, i, 0.25 * (difficult + 1) * 100);
        chance = newChance;
        bestChoice = i;

        if(botWillWin || easyModeIsEnabled) {
          return bestChoice;
        };
      };
    } else if(round >= 8 && board[i] == -1) {
      return i;
    };
  };

  return bestChoice;
};