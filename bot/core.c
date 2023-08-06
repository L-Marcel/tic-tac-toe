#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define POSSIBILITIES 512
#define SPACES 9
#define BOT_ID 0
#define PLAYER_ID 1
#define MAX_RESULTS 140

typedef struct PossibleResult {
  int grid[9];
  bool botLose;
} PossibleResult;

void setPossibilities(int grids[POSSIBILITIES][SPACES], int size, int round) {
  int group = 1;
  int player = BOT_ID;

  for(int l = 0; l < POSSIBILITIES; l++) {
    if(l < ((size / 2) * group) ) {
      grids[l][round] = player;
    } else {
      group++;

      if(player == BOT_ID) {
        player = PLAYER_ID;
      } else {
        player = BOT_ID;
      };

      grids[l][round] = player;
    };
  };

  if((size % 2) == 0 && size > 2) {
    setPossibilities(grids, size / 2, round + 1);
  };
};


bool playerWin(int grid[SPACES], int player) {
  bool winHorizontal = false;
  bool winVertical = false;
  bool winDiagonal = false;

  //horizontal
  int row = 1;
  int strikes = 0;
  for(int i = 0; i < SPACES; i++) {
    if(i < row * 3 && grid[i] == player) {
      strikes++;

      if(strikes == 3) {
        winHorizontal = true;
        break;
      };
    } else if(i < row * 3) {
      i = row * 3;
      row++;
      strikes = 0;

      if(grid[i] == player) {
        strikes++;
      };
    };
  };

  //vertical
  for(int c = 0; c < SPACES / 3; c++) {
    if(
      grid[c] == player && 
      grid[c + 3] == player && 
      grid[c + 6] == player
    ) {
      winVertical = true;
      break;
    };
  };

  //diagonal
  if(
    (grid[0] == player && 
    grid[4] == player && 
    grid[8] == player) ||
    (grid[2] == player && 
    grid[4] == player && 
    grid[6] == player)
  ) {
    winDiagonal = true;
  };

  bool win = winDiagonal || winHorizontal || winVertical;
  
  return win;
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


void printRow(int grid[SPACES], bool willLose) {
  for (int c = 0; c < SPACES; c++) {
    printf("%2d ", grid[c]);
  }

  printf(" - Lose: %d / Bot win: %d / Player win: %d / Player is the first: %d\n", willLose, 
    playerWin(grid, BOT_ID), 
    playerWin(grid, PLAYER_ID),  
    playerIsTheFirst(grid, PLAYER_ID, BOT_ID)
  );
}

bool isValid(int grid[SPACES]) {
  bool possibilitiesExists = false;
  int botOccurrences = 0;

  for(int c = 0; c < SPACES; c++) {
    if(grid[c] == BOT_ID) {
      botOccurrences++;
    };
  };

  if(
    (grid[0] == BOT_ID && botOccurrences == 5) ||
    (grid[0] != BOT_ID && botOccurrences == 4)
  ) {
    possibilitiesExists = true;
  };

  return possibilitiesExists;
};

void setWinPossibilities(int grids[POSSIBILITIES][SPACES], PossibleResult results[MAX_RESULTS]) {
  int index = 0;
  for (int l = 0; l < POSSIBILITIES; l++) {
    bool possibilityIsValid = isValid(grids[l]);
    bool botWillLose = 
      ( 
        //Pega na combinação do tabuleiro, os casos onde ambos ganham, 
        //Onde o player começar primeiro, pois assim a vitória é dele
        playerWin(grids[l], BOT_ID) && 
        playerWin(grids[l], PLAYER_ID) && 
        playerIsTheFirst(grids[l], PLAYER_ID, BOT_ID)
      ) || ( 
        //Na combinação do tabuleiro,
        //Pega somente os casos onde o player ganha
        !playerWin(grids[l], BOT_ID) && 
        playerWin(grids[l], PLAYER_ID)
      );
    
    if(possibilityIsValid) {
      PossibleResult result;
      result.botLose = botWillLose;
      
      memcpy(result.grid, grids[l], sizeof(int) * SPACES);

      results[index] = result;
      index++;
    };
  };
};
