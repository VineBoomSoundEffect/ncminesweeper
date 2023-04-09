#include <ncursesw/ncurses.h>
#include <ctime>
#include <cstdlib>
using namespace std;
struct Cell{
	bool mine = 0,
	opened = 0,
	flagged = 0;
}board[100][100];
struct Cursor{
	int x, y;
}cursor;
int GetMines(Cell board[100][100], Cursor cell){
	int s = 0;
	for(int i=cell.x-1;i<=cell.x+1;i++)
		for(int j=cell.y-1;j<=cell.y+1;j++){
			if(board[i][j].mine == 1){
				s++;
			}
		}
	return s;
}
int GetFlags(Cell board[100][100], Cursor cell){
	int s = 0;
	for(int i=cell.x-1;i<=cell.x+1;i++)
		for(int j=cell.y-1;j<=cell.y+1;j++){
			if(board[i][j].flagged == 1){
				s++;
			}
		}
	return s;
}
void AutoClear(Cell board[100][100], int w, int h, Cursor cell, int &s, int &lost){
	for(int i=cell.x-1;i<=cell.x+1;i++)
		for(int j=cell.y-1;j<=cell.y+1;j++)
			if(board[i][j].opened == 0 && i>=0 && i<w && j>=0 && j<h && board[i][j].mine == 0 && GetFlags(board, cursor) == 0){
				board[i][j].opened = 1;
				s++;
				if(GetMines(board, {i, j})==0)
					AutoClear(board, w, h, {i, j}, s, lost);
			}
			else if(board[i][j].opened == 0 && i>=0 && i<w && j>=0 && j<h && board[i][j].flagged == 0 && GetFlags(board, cursor) != 0){
				board[i][j].opened = 1;
				s++;
				if(board[i][j].mine == 1){
					lost = 1;
					board[i][j].flagged = 1;
				}
				if(GetMines(board, {i, j})==0)
					AutoClear(board, w, h, {i, j}, s, lost);
			}
}
void GenerateBoard(Cell board[100][100], int w, int h, int nrmines){
	srand(time(NULL));
	int randx, randy;
	for(int i=0;i<nrmines;i++)
		while(1){
			randx = rand()%w;
			randy = rand()%h;
			if(board[randx][randy].mine == 0){
				board[randx][randy].mine = 1;
				break;
			}
		}
}
void GetInput(int ch, Cursor &cursor, Cell board[100][100], int w, int h, int &nrflags, int &s, int &lost){
	ch = getch();
	switch(ch){
		case 'w': if(cursor.y != 0  ){cursor.y--; move(cursor.y, cursor.x*2);} break;
		case 's': if(cursor.y != h-1){cursor.y++; move(cursor.y, cursor.x*2);} break;
		case 'a': if(cursor.x != 0  ){cursor.x--; move(cursor.y, cursor.x*2);} break;
		case 'd': if(cursor.x != w-1){cursor.x++; move(cursor.y, cursor.x*2);} break;

		case 'j':{
			if(board[cursor.x][cursor.y].flagged == 0){
				if(board[cursor.x][cursor.y].mine == 0){
					if(board[cursor.x][cursor.y].opened == 0) s++;
					board[cursor.x][cursor.y].opened = 1;
					if(GetMines(board, cursor) == 0)
						AutoClear(board, w, h, cursor, s, lost);
					if(board[cursor.x][cursor.y].opened == 1 && GetMines(board, cursor) == GetFlags(board, cursor))
						AutoClear(board, w, h, cursor, s, lost);
				}
				if(board[cursor.x][cursor.y].mine == 1){
					lost = 1;
					board[cursor.x][cursor.y].opened = 1;
					board[cursor.x][cursor.y].flagged = 1;
				}
			}
			break;
		}
		case 'k':{
			if(board[cursor.x][cursor.y].opened == 0)
				if(board[cursor.x][cursor.y].flagged == 0){
					board[cursor.x][cursor.y].flagged = 1;
					nrflags++;
				}
				else{
					board[cursor.x][cursor.y].flagged = 0;
					nrflags--;
				}
			break;
		}
	}
}
void OutputBoard(Cursor &cursor, Cell board[100][100], int w, int h){
	for(int i=0;i<h;i++){
		for(int j=0;j<w;j++){
			if(board[j][i].opened == 0){
				attron(COLOR_PAIR(1));
				mvprintw(i, j*2, "[]");
				attroff(COLOR_PAIR(1));
				if(board[j][i].flagged == 1){
					attron(COLOR_PAIR(2));
					mvprintw(i, j*2, "!!");
					attroff(COLOR_PAIR(2));
				}
			}
			else
				if(board[j][i].mine == 0)
					if(GetMines(board, {j, i}) == 0)
						mvprintw(i, j*2, "  ");
					else
						mvprintw(i, j*2, "%i ", GetMines(board, {j, i}));
				else{
					attron(COLOR_PAIR(3));
					mvprintw(i, j*2, "# ");
					attroff(COLOR_PAIR(3));
				}
		}
	}
	move(cursor.y, cursor.x*2);
	refresh();
}
int main(){
	int w = 30, h = 24;
	int nrmines = 100;
	int nrflags = 0;
	int s = 0;
	int lost = 0;
	int ch;
	initscr();
	noecho();
	keypad(stdscr, TRUE);
	start_color();
	init_pair(1, COLOR_BLUE, COLOR_BLACK);
	init_pair(2, COLOR_YELLOW, COLOR_BLACK);
	init_pair(3, COLOR_BLACK, COLOR_RED);
	GenerateBoard(board, w, h, nrmines);
	mvprintw(0, (w+2)*2, "MINESWEEPER - TEODOR POMOHACI");
	mvprintw(1, (w+2)*2, "W,A,S,D - MOVE AROUND");
	mvprintw(2, (w+2)*2, "J - OPEN CELL");
	mvprintw(3, (w+2)*2, "K - PLACE FLAG");
	OutputBoard(cursor, board, w, h);
	while(1){
		GetInput(ch, cursor, board, w, h, nrflags, s, lost);
		OutputBoard(cursor, board, w, h);
		if(lost == 1){
			mvprintw(5, (w+2)*2, "YOU LOST!");
			mvprintw(6, (w+2)*2, "BUT YOU MAY CONTINUE");
			lost = 2;
		}
		if(s == w*h-nrmines && lost == 0){
			mvprintw(5, (w+2)*2, "YOU WON!");
			lost = 2;
		}
		mvprintw(4, (w+2)*2, "                     ");
		mvprintw(4, (w+2)*2, "%i", nrmines-nrflags);
		move(cursor.y, cursor.x*2);
	}
	getch();
	endwin();
	return 0;
}