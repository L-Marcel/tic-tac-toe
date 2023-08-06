#include <stdio.h>
#include <stdbool.h>
#include "core.c"

typedef struct Choice {
  int value;
  float chance;
  bool playerWillWin;
  bool botWillWin;
} Choice;

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
  float occurrences = 0;
  float notLoseOccurrences = 0;

  int boardAfterChoice[SPACES];
  memcpy(boardAfterChoice, board, sizeof(int) * SPACES);
  boardAfterChoice[choice] = BOT_ID;

  for(int i = 0; i < MAX_RESULTS; i++) {
    if(
      //Exclui casos impossíveis
      matchGrid(boardAfterChoice, results[i].grid, round) && 
      //Pega somente as combinações onde o mesmo jogador foi o primeiro a jogar
      playerIsTheFirst(results[i].grid, BOT_ID, PLAYER_ID) == playerIsTheFirst(boardAfterChoice, BOT_ID, PLAYER_ID)
    ) {
      occurrences++;

      //#debug
      //printRow(results[i].grid, results[i].botLose);
      if(!results[i].botLose) {
        notLoseOccurrences++;
      };
    };
  };

  //#debug
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

  Choice bestChoice = { -1, 0.0, true, false };
  bool botWillWin = false;
  bool playerWillWin = false;
  bool easyModeIsEnabled = difficult <= 1;

  for(int i = 0; i < SPACES; i++) {
    if(round < 8 && board[i] == -1) {
      float newChance = getChance(results, board, round + 1, i);
      botWillWin = checkIfBotIsTheWinner(board, i) && difficult >= 1;


      //O centro é especial
      if(i == 4 && newChance <= 0.9 && difficult >= 1) {
        newChance += 0.1;
        //#debug
        //printf("Bonus: %d%%", 10);
      };

      //Quando o meio é ocupado, o canto se torna 
      //um ponto importante
      if(
        board[4] == PLAYER_ID && 
        (i == 0 || i == 2 || i == 6 || i == 8) && 
        newChance <= 0.8 &&
        difficult >= 2
      ) {
        newChance += 0.2;
        //#debug
        //printf("Bonus: %d%%", 20);
      };

      for(int p = 0; p < SPACES; p++) {
        if(p != i && board[p] == -1) {
          playerWillWin = playerWillWin || (checkIfPayerIsTheWinner(board, p, i) && difficult >= 2);
        }
      };

      Choice currentChoice = { i, newChance, playerWillWin, botWillWin };
      
      //#debug
      //printf("Candidate: (%f%%) %d Bot: %d / Player: %d\n", currentChoice.chance * 100.0, i + 1, botWillWin, playerWillWin);
      if(
        bestChoice.value == -1 ||
        (
          ( 
            !bestChoice.playerWillWin ||
            (
              !currentChoice.playerWillWin && 
              !bestChoice.botWillWin &&
              bestChoice.playerWillWin
            )
          ) && (
            currentChoice.botWillWin || 
            (
              (currentChoice.chance >= bestChoice.chance) && 
              !currentChoice.playerWillWin &&
              !currentChoice.botWillWin
            )
          ) && (
            (currentChoice.chance <= (0.25 + (0.1875 * (difficult + 1))))
          )
        )
      ) {
        //#debug
        //printf("New best chance: (Before -> %.2f%%) %.2f%% %d\n", bestChoice.chance * 100.0, currentChoice.chance * 100, i + 1);
        bestChoice = currentChoice;

        if(botWillWin || easyModeIsEnabled) {
          //#debug
          //printf("Bot will win: %d", bestChoice.value + 1);
          return bestChoice.value;
        };
      };
    } else if(round >= 8 && board[i] == -1) {
      //#debug
      //printf("Final round: %d", i + 1);
      return i;
    };

    playerWillWin = false;
  };

  //#debug
  //printf("Final: %d\n", bestChoice.value + 1);
  return bestChoice.value;
};