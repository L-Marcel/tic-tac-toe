#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <locale.h>
#include <windows.h>
#include "bot/decision.c"

char *getPlayerSymbol(int playerId, int index) {
  if(playerId == BOT_ID) {
    return "X";
  } else if(playerId == PLAYER_ID) {
    return "O";
  } else {
    char *id = (char *)malloc(2 * sizeof(char));;
    id[0] = '0' + index;
    id[1] = '\0';

    return id;
  };
};

void printBoard(int grid[SPACES]) {
  system("cls");
  printf("\n %s | %s | %s\n", getPlayerSymbol(grid[0], 1), getPlayerSymbol(grid[1], 2), getPlayerSymbol(grid[2], 3));
  printf("-----------\n");
  printf(" %s | %s | %s\n", getPlayerSymbol(grid[3], 4), getPlayerSymbol(grid[4], 5), getPlayerSymbol(grid[5], 6));
  printf("-----------\n");
  printf(" %s | %s | %s\n", getPlayerSymbol(grid[6], 7), getPlayerSymbol(grid[7], 8), getPlayerSymbol(grid[8], 9));
};

void printBoardWithputClear(int grid[SPACES]) {
  printf("\n %s | %s | %s\n", getPlayerSymbol(grid[0], 1), getPlayerSymbol(grid[1], 2), getPlayerSymbol(grid[2], 3));
  printf("-----------\n");
  printf(" %s | %s | %s\n", getPlayerSymbol(grid[3], 4), getPlayerSymbol(grid[4], 5), getPlayerSymbol(grid[5], 6));
  printf("-----------\n");
  printf(" %s | %s | %s\n", getPlayerSymbol(grid[6], 7), getPlayerSymbol(grid[7], 8), getPlayerSymbol(grid[8], 9));
};


int main() {
  UINT CP_DEFAULT = GetConsoleOutputCP();

  setlocale(LC_ALL, "Portuguese_Brasil");
  SetConsoleOutputCP(65001);

  while(true) {
    int difficult;
    int roundOf = PLAYER_ID;

    system("cls");
    while(true) {
      printf("\nSelecione o nível de dificuldade:\n[0] - Fácil\n[1] - Médio\n[2] - Difícil\n[3] - Impossível\n>> ");
      scanf("%d", &difficult);
      fflush(stdin);

      if(difficult >= 0 && difficult <= 3) {
        break;
      } else {
        system("cls");
        printf("\nSelecione uma opção válida!");
      };
    };

    int board[SPACES] = {-1,-1,-1,-1,-1,-1,-1,-1,-1};

    system("cls");
    while(true) {
      printf("\nSelecione quem começa primeiro:\n[0] Bot\n[1] Voce\n>> ");
      scanf("%d", &roundOf);
      fflush(stdin);

      if(roundOf >= 0 && roundOf <= 1) {
        break;
      } else {
        system("cls");
        printf("\nSelecione uma opção válida!\n");
      };
    };
    
    int round = 0;
    int selectedByBot = -1;
    while(true) {
      bool playerIsTheWinner = playerWin(board, PLAYER_ID);
      bool botIsTheWinner = playerWin(board, BOT_ID);

      if(playerIsTheWinner) {
        printBoard(board);
        printf("\nParabéns, voce venceu o bot!");
        break;
      } else if(botIsTheWinner) {
        printBoard(board);
        printf("\nVoce perdeu...");
        break;
      } else if(round == 9) {
        printBoard(board);
        printf("\nTemos um empate!");
        break;
      } else if(roundOf == PLAYER_ID) {
        roundOf = BOT_ID;
        
        int selectedPosition;
        if(selectedByBot == -1) {
          printBoard(board);
        } else {
          system("cls");
          printf("\nO bot selecionou a casa de número %d!\n", selectedByBot + 1);
          printBoardWithputClear(board);
          selectedByBot = -1;
        };
        
        printf("\nAviso: 'O' te representa, 'X' representa o bot!");
        while(true) {
          printf("\nEscolha o número uma casa válida >> ");
          scanf("%d", &selectedPosition);
          fflush(stdin);

          bool positionIsValid = 
            selectedPosition <= 9 &&
            selectedPosition >= 1 && 
            board[selectedPosition - 1] == -1;

          if(!positionIsValid) {
            printBoard(board);
            printf("\nAviso: o número %d não está disponível!", selectedPosition);
          } else {
            board[selectedPosition - 1] = PLAYER_ID;
            break;
          };
        };
      } else {
        roundOf = PLAYER_ID;
        selectedByBot = getBotDecision(board, difficult, round);
        board[selectedByBot] = BOT_ID;
      };

      round++;
    };

    int again = 0;
    while(true) {
      printf("\nDeseja jogar novamente?\n[0] Não\n[1] Sim\n>> ");
      scanf("%d", &again);
      fflush(stdin);

      if(again >= 0 && again <= 1) {
        break;
      } else {
        system("cls");
        printf("\nSelecione uma opção válida!");
      };
    };

    if(again == 0) {
      break;
    };
  };

  SetConsoleOutputCP(CP_DEFAULT);
  return 0;
};