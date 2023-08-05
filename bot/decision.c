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
      !(playerWin(results[i].grid, BOT_ID) && playerWin(results[i].grid, PLAYER_ID)) &&
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

bool checkIfPayerIsTheWinner(int board[SPACES], int choice, int botChoice) {
  int boardAfterChoice[SPACES];
  memcpy(boardAfterChoice, board, sizeof(int) * SPACES);
  boardAfterChoice[botChoice] = BOT_ID;
  boardAfterChoice[choice] = PLAYER_ID;

  bool playerIsTheWinner = playerWin(boardAfterChoice, PLAYER_ID) && !playerWin(boardAfterChoice, BOT_ID);
  return playerIsTheWinner;
};

int getBotDecision(int board[SPACES], int difficult, int round) {
  int grids[POSSIBILITIES][SPACES] = {0};
  setPossibilities(grids, POSSIBILITIES, 0);

  PossibleResult results[MAX_RESULTS];
  setWinPossibilities(grids, results);

  int bestChoice = -1;
  float chance = 0;
  bool botWillWin = false;
  bool playerWillWin = false;
  bool easyModeIsEnabled = difficult <= 1;

  for(int i = 0; i < SPACES; i++) {
    if(round < 8 && board[i] == -1) {
      float newChance = getChance(results, board, round + 1, i);
      botWillWin = checkIfBotIsTheWinner(board, i) && difficult >= 2;

      for(int p = 0; p < SPACES; p++) {
        if(p != i && board[p] == -1) {
          playerWillWin = playerWillWin || (checkIfPayerIsTheWinner(board, p, i) && difficult >= 1);
        }
      };

      //printf("Candidate: (%f%%) %d %d Bot: %d / Player: %d\n", newChance, i, newChance >= chance, botWillWin, playerWillWin);
      if(!playerWillWin && (botWillWin || ((newChance >= chance) && !botWillWin)) && (((newChance <= (0.25 + (0.1875 * (difficult + 1))))) || (bestChoice == -1))) {
        //printf("New best chance: %.2f%% %.2f%% %d %.2f%%\n", chance * 100.0, newChance * 100, i, 0.25 * (difficult + 1) * 100);
        chance = newChance;
        bestChoice = i;

        if(botWillWin || easyModeIsEnabled) {
          //printf("Bot will win: %d", bestChoice);
          return bestChoice;
        };
      };
    } else if(round >= 8 && board[i] == -1) {
      //printf("Final round: %d", i);
      return i;
    };

    playerWillWin = false;
  };

  //printf("Final: %d\n", bestChoice);
  return bestChoice;
};