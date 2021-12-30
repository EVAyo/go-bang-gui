#include <iostream>
#include "Game.h"

using namespace std;

Game::Game() {
    size = 15;
    grid = new int*[size];
    lastGrid = new int*[size];
    for (int i = 0; i < size; i++) {
        grid[i] = new int[size];
        lastGrid[i] = new int[size];
    }
    initGrid(0);
    setWhoFirst(1);                 //black first
}

//initialize the board
void Game::initGrid(int GameMode)
{
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++)
        {
            grid[i][j] = 0;
            lastGrid[i][j] = 0;
        }
    }
    gameMode = GameMode;                   //PVP
    setWinPos(-1,-1,-1);            //r,c,dirc
    setIsOver(false);
    setCurUser(getWhoFirst());      //current black
}

//set Who First play
void Game::setWhoFirst(int who)
{
    firstPlay = who;
}
//get Who First play
int Game::getWhoFirst()
{
    return firstPlay;
}

//set Win Pos
void Game::setWinPos(int r,int c,int dirc)
{
    winPos[0] = r;
    winPos[1] = c;
    winPos[2] = dirc;
    if(dirc != -1)
    {
        winPos[3] = grid[r][c];
    }
}
//get Win Pos
int * Game::getWinPos()
{
    return winPos;
}

//get grid data
int** Game::getGrid()
{
    return grid;
}
//set grid data
void Game::setGrid(int **newGird)
{
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++)
        {
            grid[i][j] = newGird[i][j];
        }
    }
}


//get last grid data
int ** Game::getLastGrid()
{
    return lastGrid;
}
//set last grid data
void Game::setLastGrid(int **last)
{
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++)
        {
            lastGrid[i][j] = last[i][j];
        }
    }
}


// get current user
int Game::getCurUser()
{
    return curUser;
}
//set current user
void Game::setCurUser(int user)
{
    curUser = user;
}

//set game isOver
void Game::setIsOver(bool res)
{
    isOver = res;
}
//get game isOver
bool Game::getIsOver()
{
    return isOver;
}

//set GameMode
void Game::setGameMode(int mode)
{
    gameMode = mode;
}
//get GameMode
int Game::getGameMode()
{
    return gameMode;
}


//show current grid state
void Game::printGrid()
{
    cout << "================\n";
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++)
        {
            if (lastGrid[i][j] == 1)
            {
                cout << " * ";
            }
            else if (lastGrid[i][j] == -1)
            {
                cout << " x ";
            }
            else
            {
                //cout << ' ' << grid[i][j] << ' ';
                cout << " o ";
            }
        }
        cout << '\n';
    }
    cout << "================\n";
}

//place the piece
bool Game::putChess(int i, int j)
{
    if(!getIsOver()&&checkPoint(i,j))
    {
        setLastGrid(grid);
        grid[i][j] = getCurUser();
        return true;
    }
    return false;
}
//can place the piece?
bool Game::checkPoint(int i, int j)
{
    return grid[i][j] == 0;
}

//check whether the game is over
void Game::checkOver()
{
    int* ckResult;					//the check result of every grid
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (grid[i][j] == 0)
            {
                continue;			//skip the 0 grid
            }
            ckResult = checkLine(i,j);
            if (ckResult[0] == 1)
            {
                setWinPos(i,j,ckResult[1]);
                setIsOver(true);
                break;
            }
            delete ckResult;
        }
    }
    if(!getIsOver())
    {
        setCurUser(getCurUser()*-1);   //change the current user after putting the piece
    }
}
//check whether there is a line
/*
r:row c:column dirc:direction
dirc:
1:rigth
2:right-down
3:down
4:left-down
*/
int * Game::checkLine(int r, int c)
{
    int *result = new int[2]{};//results,dirc
    for (int d = 1; d <= 4; d++)
    {
        int num = 0;
        switch (d)
        {
        case 1:	/*right*/
            if (c + winFlag > size)
            {
                break;				//Insufficient number of squares on the right
            }
            for (int i = 1; i < winFlag; i++) {
                if (grid[r][c + i] == grid[r][c]) {
                    num++;
                }
                else
                {
                    break;			//there ara different grid
                }
            }
            break;
        case 2:	/*right-down*/
            if (c + winFlag > size || r + winFlag > size)
            {
                break;				//Insufficient number of squares on the right-down
            }
            for (int i = 1; i < winFlag; i++) {
                if (grid[r + i][c + i] == grid[r][c]) {
                    num++;
                }
                else
                {
                    break;			//there ara different grid
                }
            }
            break;
        case 3:	/*down*/
            if (r + winFlag > size)
            {
                break;				//Insufficient number of squares on the down
            }
            for (int i = 1; i < winFlag; i++) {
                if (grid[r + i][c] == grid[r][c]) {
                    num++;
                }
                else
                {
                    break;			//there ara different grid
                }
            }
            break;
        case 4:	/*left-down*/
            if (c < winFlag - 1 || r + winFlag > size)
            {
                break;				//Insufficient number of squares on the left-down
            }
            for (int i = 1; i < winFlag; i++) {
                if (grid[r + i][c - i] == grid[r][c]) {
                    num++;
                }
                else
                {
                    break;			//there ara different grid
                }
            }
            break;
        default:
            break;
        }
        if (num == winFlag - 1)		//there are enough identical grid
        {
            result[1] = d;
            result[0] = 1;
        }
    }
    return result;
}
