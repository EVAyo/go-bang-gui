#include <iostream>
#include "Game.h"

using namespace std;

Game::Game() {
    size = 15;
    grid = new int*[size];
    for (int i = 0; i < size; i++) {
        grid[i] = new int[size];
    }
    initGrid();
    setWhoFirst(1);                 //black first
}

//initialize the board
void Game::initGrid()
{
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++)
        {
            grid[i][j] = 0;
            lastGrid[i][j] = 0;
        }
    }
    setIsOver(false);
    setCurUser(getWhoFirst());      //current black
    string who = getWhoFirst()== 1 ? "黑棋" : "白棋";
    setGameMsg("点击开始游戏，" + who + "先行！");
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


//get grid data
int** Game::getGrid()
{
    return grid;
}

//get last grid data
int ** Game::getLastGrid()
{
    return lastGrid;
}
//set last grid data
void Game::setLastGrid(int **last)
{
    lastGrid = last;
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

//set game message
void Game::setGameMsg(std::string msg)
{
    gameMsg = msg;
}
//get game message
std::string Game::getGameMsg()
{
    return gameMsg;
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

//show current grid state
void Game::printGrid()
{
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
}

//place the piece
bool Game::putChess(int i, int j)
{
    if (!isOver && grid[i][j] == 0)	{
        grid[i][j] = getCurUser();
//        setCurUser(getCurUser()*-1);   //change the current user after putting the piece
    }else {
        cout << "can not place piece here !\n";
        setGameMsg("这里不能落棋!");
        return false;
    }
    return true;
}


//check whether the game is over
void Game::checkOver()
{
    int* ckResult;					//the check result of every grid
    int r = -1, c = -1;
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
                r = i;				//a line has been found
                c = j;
                string res = (getCurUser() == 1 ? "黑棋" : "白棋");
                setGameMsg("游戏结束!" + res + "胜利!");
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
//    int result[5] = { 0,0,0,0,0 };	//results in 4 directions
    int *result = new int[5]{};
    for (int d = 1; d <= 4; d++)
    {
        int num = 0;
        switch (d)
        {
        case 1:	/*right*/
            if (c + winFlag >= size)
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
            if (c + winFlag >= size || r + winFlag >= size)
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
            if (r + winFlag >= size)
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
            if (c < winFlag - 1 || r + winFlag >= size)
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
            result[d] = 1;
            result[0] = 1;
        }
    }
    return result;
}
