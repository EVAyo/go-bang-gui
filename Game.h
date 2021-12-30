#pragma once
#ifndef GAME_H
#define GAME_H
#include <string>
class Game
{
public:
    Game();
    void initGrid(int);				//initialize the board
    void printGrid();				//show current grid state

    int ** getGrid();               //get grid data
    void setGrid(int **);           //set grid data

    void setWinPos(int,int,int);
    int * getWinPos();

    bool putChess(int,int);			//place the piece
    bool checkPoint(int,int);       //check can place chess on the point
    int ** getLastGrid();           //get last grid data
    void setLastGrid(int **);       //set last grid data

    void setCurUser(int);
    int getCurUser();

    void setIsOver(bool);
    bool getIsOver();

    void setGameMode(int);
    int getGameMode();

    void setWhoFirst(int);
    int getWhoFirst();

    void checkOver();				//check whether the game is over
    int * checkLine(int,int);	//check whether there is a line
private:
    int size;           //grid size
    int **grid;         //grid array
    int **lastGrid;     //x,y,current user
    int winPos[4];      //the location of the first point
    bool isOver;        //game over?
    int gameMode;       //0:PVP 1:PVE 3:online
    int curUser;        //current user 1:black -1:white
    int firstPlay;      //who first 1:black -1:white
    const int winFlag = 5;	//condition for winning the game
};

#endif // !GAME
