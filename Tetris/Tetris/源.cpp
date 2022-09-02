#include"iostream"
#include"stdlib.h"
#include"easyx.h"
#include"string.h"
#include"graphics.h"
#include"conio.h"
#include"time.h"
#include"fstream"
#include"windows.h"
#include"tchar.h"
#undef UNICODE
#undef _UNICODE

using namespace std;

struct Map_pos//划分地图区域
{
	int x = 0, y = 0, map_val = 0, map_col = 0;
};
Map_pos mpos[15][25];//地图上的网格及其坐标//实际 10*20   多余5格缓存
struct no
{
	char id[20];
	char score[10];
	int score_int=0;
	char time[100] = { '\n' };
};

struct Users
{
	char name[20] = { '\n' };
	char password[20] = { '\n' };
	int priv = 0;
};

class Block
{
	int block_val = 0, block_type = 0, gameSpeed = 400;//block的值和block的类型
	int score = 0;
	
public:
	void init_game();//初始化地图 初始化界面
	void drawBlock(int head_x, int head_y, int block_type);//画方块组合
	void cleanBlock(int head_x, int head_y, int block_type);//消除方块组合
	int down(int head_x, int head_y, int block_type);//下移
	int isLRBorder(int head_x, int block_type);//判断左右边界
	bool isBottom(int head_x, int head_y, int block_type);	//判断是否能继续向下移动  
	bool MoveLR(int head_x, int head_y, int block_type);//判断能否左右移动
	int next_block();//下一个方块
	int TopBorder(int head_x, int head_y, int block_type);//方块上边界

	bool gameover();//游戏结束
	void rank();//游戏结束输入ID 并计入计分板 并输出前十分数 相对应的 ID Socre Time

	void fullLine();//行满消除加分
	void run();//跑起来咯!!!!~~~~
	void menu();

	friend class User;
	friend class Admin;
};

class User
{
	char name[20] = {0};
	char password[20] = { 0 };
	int priv = 0;//权限等级
public:
	bool login();
	bool reg();//注册
	Block b;
};
class Admin :public User
{
	int priv = 1;
public:
	void menu();

};

void Block::init_game()//初始化地图 画好游戏背景以及基础内容显示
{
	initgraph(720, 960);
	setlinecolor(RGB(20, 20, 20));
	for (int width = 48; width < 720; width += 48)
		line(width, 0, width, 960);
	for (int height = 48; height < 960; height += 48)
		line(0, height, 720, height);

	setlinecolor(RGB(254, 254, 254));
	setlinestyle(PS_SOLID | PS_ENDCAP_ROUND | PS_JOIN_ROUND, 5);//宽度5 接头圆角
	line(480, 0, 480, 960);     //4边框
	line(719, 1, 1, 1);
	line(0, 959, 1, 1);
	line(0, 959, 719, 959);
	line(719, 0, 719, 959);
	settextcolor(RGB(254, 254, 254));
	settextstyle(50, 0, _T("楷体"));
	RECT Tetris = { 480,0,719,219 };
	drawtext(_T("Tetris"), &Tetris, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//区域居中打印
	RECT Score = { 480,640,719,959 };
	drawtext(_T("Score:"), &Score, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	line(480, 740, 719, 740);
	RECT Score_VAL = { 480,800,719,959 };
	drawtext(_T("000000"), &Score_VAL, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	settextstyle(25, 0, _T("楷体"));
	RECT NEXT_BLOCK = { 480,100,719,319 };
	drawtext(_T("Next Block"), &NEXT_BLOCK, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//区域居中打印
	outtextxy(520, 600, "Author:方明轩");

}
void Block::drawBlock(int head_x, int head_y, int block_type)//head_x head_y 4x4矩阵左上角坐标 block_type 方块组合类型
{
	int block[4][4];// 1田  2345L   6789!L 1011Z 1213!Z 14151617山 1819|
	int col = 0;//  1BLUE 2GREEN 3RED 4WHITE 5YELLOW 6CYAN  7BROWN 
	if (block_type == 1)//田
	{
		setfillcolor(BLUE);
		block[2][2] = block[2][3] = block[3][2] = block[3][3] = 1;
		col = 1;
	}
	else if (block_type == 2)//L
	{
		setfillcolor(GREEN);
		block[2][1] = block[2][2] = block[2][3] = block[3][3] = 1;
		col = 2;
	}
	else if (block_type == 3)//L
	{
		setfillcolor(GREEN);
		block[1][2] = block[1][3] = block[2][2] = block[3][2] = 1;
		col = 2;
	}
	else if (block_type == 4)//L
	{
		setfillcolor(GREEN);
		block[2][1] = block[3][2] = block[3][3] = block[3][1] = 1;
		col = 2;
	}
	else if (block_type == 5)//L
	{
		setfillcolor(GREEN);
		block[3][2] = block[1][3] = block[2][3] = block[3][3] = 1;
		col = 2;
	}
	else if (block_type == 6)//!L
	{
		setfillcolor(RED);
		block[3][1] = block[3][2] = block[3][3] = block[2][3] = 1;
		col = 3;
	}
	else if (block_type == 7)//!L
	{
		setfillcolor(RED);
		block[1][2] = block[1][3] = block[2][3] = block[3][3] = 1;
		col = 3;
	}
	else if (block_type == 8)//!L
	{
		setfillcolor(RED);
		block[2][1] = block[2][2] = block[2][3] = block[3][1] = 1;
		col = 3;
	}
	else if (block_type == 9)//!L
	{
		setfillcolor(RED);
		block[1][2] = block[3][3] = block[2][2] = block[3][2] = 1;
		col = 3;
	}
	else if (block_type == 10)//Z
	{
		setfillcolor(WHITE);
		block[3][1] = block[3][2] = block[2][2] = block[2][3] = 1;
		col = 4;
	}
	else if (block_type == 11)//Z
	{
		setfillcolor(WHITE);
		block[1][2] = block[2][2] = block[2][3] = block[3][3] = 1;
		col = 4;
	}
	else if (block_type == 12)//!Z
	{
		setfillcolor(YELLOW);
		block[2][1] = block[2][2] = block[3][2] = block[3][3] = 1;
		col = 5;
	}
	else if (block_type == 13)//!Z
	{
		setfillcolor(YELLOW);
		block[1][3] = block[2][2] = block[2][3] = block[3][2] = 1;
		col = 5;
	}
	else if (block_type == 14)//山
	{
		setfillcolor(CYAN);
		block[1][3] = block[2][2] = block[2][3] = block[3][3] = 1;
		col = 6;
	}
	else if (block_type == 15)//山
	{
		setfillcolor(CYAN);
		block[2][1] = block[2][2] = block[2][3] = block[3][2] = 1;
		col = 6;
	}
	else if (block_type == 16)//山
	{
		setfillcolor(CYAN);
		block[1][2] = block[2][2] = block[3][2] = block[2][3] = 1;
		col = 6;
	}
	else if (block_type == 17)//山
	{
		setfillcolor(CYAN);
		block[2][2] = block[3][1] = block[3][2] = block[3][3] = 1;
		col = 6;
	}
	else if (block_type == 18)// -
	{
		setfillcolor(BROWN);
		block[3][0] = block[3][1] = block[3][2] = block[3][3] = 1;
		col = 7;
	}
	else if (block_type == 19)// -
	{
		setfillcolor(BROWN);
		block[0][3] = block[1][3] = block[2][3] = block[3][3] = 1;
		col = 7;
	}
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (block[i][j] == 1) {
				floodfill(mpos[head_x + i][head_y + j].x, mpos[head_x + i][head_y + j].y, BLACK, 1);
				mpos[head_x + i][head_y + j].map_val = 1;
				mpos[head_x + i][head_y + j].map_col = col;
			}
		}
	}
}
void Block::cleanBlock(int head_x, int head_y, int block_type)//涂黑消除
{
	int block[4][4] = { 0 };
	int LBorder = 0;
	setfillcolor(BLACK);
	if (block_type == 1)//田
	{
		block[2][2] = block[2][3] = block[3][2] = block[3][3] = 1;
		LBorder = 2;
	}
	else if (block_type == 2)//L
	{
		block[2][1] = block[2][2] = block[2][3] = block[3][3] = 1;
		LBorder = 2;

	}
	else if (block_type == 3)//L
	{
		LBorder = 1;
		block[1][2] = block[1][3] = block[2][2] = block[3][2] = 1;
	}
	else if (block_type == 4)//L
	{
		block[2][1] = block[3][2] = block[3][3] = block[3][1] = 1;
		LBorder = 2;
	}
	else if (block_type == 5)//L
	{
		block[3][2] = block[1][3] = block[2][3] = block[3][3] = 1;
		LBorder = 1;
	}
	else if (block_type == 6)//!L
	{
		block[3][1] = block[3][2] = block[3][3] = block[2][3] = 1;
		LBorder = 2;
	}
	else if (block_type == 7)//!L
	{
		block[1][2] = block[1][3] = block[2][3] = block[3][3] = 1;
		LBorder = 1;
	}
	else if (block_type == 8)//!L
	{
		block[2][1] = block[2][2] = block[2][3] = block[3][1] = 1;
		LBorder = 2;
	}
	else if (block_type == 9)//!L
	{
		block[1][2] = block[3][3] = block[2][2] = block[3][2] = 1;
		LBorder = 1;
	}
	else if (block_type == 10)//Z
	{
		block[3][1] = block[3][2] = block[2][2] = block[2][3] = 1;
		LBorder = 2;
	}
	else if (block_type == 11)//Z
	{
		block[1][2] = block[2][2] = block[2][3] = block[3][3] = 1;
		LBorder = 1;
	}
	else if (block_type == 12)//!Z
	{
		block[2][1] = block[2][2] = block[3][2] = block[3][3] = 1;
		LBorder = 2;
	}
	else if (block_type == 13)//!Z
	{
		block[1][3] = block[2][2] = block[2][3] = block[3][2] = 1;
		LBorder = 1;
	}
	else if (block_type == 14)//山
	{
		block[1][3] = block[2][2] = block[2][3] = block[3][3] = 1;
		LBorder = 1;
	}
	else if (block_type == 15)//山
	{
		block[2][1] = block[2][2] = block[2][3] = block[3][2] = 1;
		LBorder = 2;
	}
	else if (block_type == 16)//山
	{
		block[1][2] = block[2][2] = block[3][2] = block[2][3] = 1;
		LBorder = 1;
	}
	else if (block_type == 17)//山
	{
		block[2][2] = block[3][1] = block[3][2] = block[3][3] = 1;
		LBorder = 2;
	}
	else if (block_type == 18)// -
	{
		block[3][0] = block[3][1] = block[3][2] = block[3][3] = 1;
		LBorder = 3;
	}
	else if (block_type == 19)// -
	{
		block[0][3] = block[1][3] = block[2][3] = block[3][3] = 1;
		LBorder = 0;
	}
	else if (block_type == 20)
	{
		block[0][0] = block[0][1] = block[0][2] = block[0][3] =
			block[1][0] = block[1][1] = block[1][2] = block[1][3] =
			block[2][0] = block[2][1] = block[2][2] = block[2][3] =
			block[3][0] = block[3][1] = block[3][2] = block[3][3] = 1;
	}

	for (int i = LBorder; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)// TopBorder(head_x, head_y, block_type) - 1
		{
			if (block[i][j] == 1)
			{
				floodfill(mpos[head_x + i][head_y + j].x, mpos[head_x + i][head_y + j].y, BLUE, 1);
				floodfill(mpos[head_x + i][head_y + j].x, mpos[head_x + i][head_y + j].y, WHITE, 1);
				floodfill(mpos[head_x + i][head_y + j].x, mpos[head_x + i][head_y + j].y, YELLOW, 1);
				floodfill(mpos[head_x + i][head_y + j].x, mpos[head_x + i][head_y + j].y, BROWN, 1);
				floodfill(mpos[head_x + i][head_y + j].x, mpos[head_x + i][head_y + j].y, CYAN, 1);
				floodfill(mpos[head_x + i][head_y + j].x, mpos[head_x + i][head_y + j].y, RED, 1);
				floodfill(mpos[head_x + i][head_y + j].x, mpos[head_x + i][head_y + j].y, GREEN, 1);
				mpos[head_x + i][head_y + j].map_val = 0;
				mpos[head_x + i][head_y + j].map_col = 0;
			}

		}
	}
}
int Block::down(int head_x, int head_y, int block_type)
{

	if (isBottom(head_x, head_y, block_type))
		return head_y;
	else {
		head_y += 1;
		return head_y;
	}
}
int Block::next_block()//2020.12.09 00:19留坑  next_block逻辑不正常 
{
	srand((unsigned)time(NULL));
	int nextBlockType = rand() % 19 + 1;
	/*cleanBlock(10, 5, 20);
	drawBlock(10, 5, nextBlockType);*/
	return nextBlockType;
}
int Block::isLRBorder(int head_x, int block_type)
{
	//判断是否为左右边界 并且回移修正防止出界]       

	int block[4][4];// 1田  2345L   6789!L 1011Z 1213!Z 14151617山 1819|
	int Lborder = 0;
	if (block_type == 1) Lborder = 2;//田	
	else if (block_type == 2)  Lborder = 2;//L
	else if (block_type == 3)  Lborder = 1;//L
	else if (block_type == 4)  Lborder = 2;//L
	else if (block_type == 5)  Lborder = 1;//L
	else if (block_type == 6)  Lborder = 2;//!L
	else if (block_type == 7)  Lborder = 1;//!L
	else if (block_type == 8)  Lborder = 2;//!L
	else if (block_type == 9)  Lborder = 1;//!L
	else if (block_type == 10) Lborder = 2;//Z
	else if (block_type == 11) Lborder = 1;//Z
	else if (block_type == 12) Lborder = 2;//!Z
	else if (block_type == 13) Lborder = 1;//!Z
	else if (block_type == 14) Lborder = 1;//山
	else if (block_type == 15) Lborder = 2;//山
	else if (block_type == 16) Lborder = 1;//山
	else if (block_type == 17) Lborder = 2;//山
	else if (block_type == 18) Lborder = 3;// -
	else if (block_type == 19) Lborder = 0;// -
	if (head_x > 6)//左边界直接判断 矩阵是否出界
	{
		head_x = 6;
	}
	else if (Lborder + head_x < 0)//右边界需要判断矩阵内的值是否到达边界
	{
		head_x = -Lborder;//右移Lborder个单位来跳出边界
	}
	return head_x;
}
bool Block::MoveLR(int head_x, int head_y, int block_type)
{
	int block[4][4];// 1田  2345L   6789!L 1011Z 1213!Z 14151617山 1819|
	int Lborder = 0;
	if (block_type == 1) Lborder = 2;//田	
	else if (block_type == 2)  Lborder = 2;//L
	else if (block_type == 3)  Lborder = 1;//L
	else if (block_type == 4)  Lborder = 2;//L
	else if (block_type == 5)  Lborder = 1;//L
	else if (block_type == 6)  Lborder = 2;//!L
	else if (block_type == 7)  Lborder = 1;//!L
	else if (block_type == 8)  Lborder = 2;//!L
	else if (block_type == 9)  Lborder = 1;//!L
	else if (block_type == 10) Lborder = 2;//Z
	else if (block_type == 11) Lborder = 1;//Z
	else if (block_type == 12) Lborder = 2;//!Z
	else if (block_type == 13) Lborder = 1;//!Z
	else if (block_type == 14) Lborder = 1;//山
	else if (block_type == 15) Lborder = 2;//山
	else if (block_type == 16) Lborder = 1;//山
	else if (block_type == 17) Lborder = 2;//山
	else if (block_type == 18) Lborder = 3;// -
	else if (block_type == 19) Lborder = 0;// -


	for (int j = 0; j < 4; j++)
	{
		if (mpos[head_x + Lborder - 1][head_y + j].map_val == 1 && mpos[head_x + Lborder][head_y + j].map_val == 1)
			return false;
		if (mpos[head_x + 3][head_y + j].map_val == 1 && mpos[head_x + 4][head_y + j].map_val == 1)
			return false;
	}



	return true;
}
int Block::TopBorder(int head_x, int head_y, int block_type)
{
	int block[4][4];// 1田  2345L   6789!L 1011Z 1213!Z 14151617山 1819|
	int Topborder = 0;
	if (block_type == 1) Topborder = 2;//田	
	else if (block_type == 2)  Topborder = 1;//L
	else if (block_type == 3)  Topborder = 2;//L
	else if (block_type == 4)  Topborder = 1;//L
	else if (block_type == 5)  Topborder = 2;//L
	else if (block_type == 6)  Topborder = 1;//!L
	else if (block_type == 7)  Topborder = 2;//!L
	else if (block_type == 8)  Topborder = 1;//!L
	else if (block_type == 9)  Topborder = 2;//!L
	else if (block_type == 10) Topborder = 2;//Z
	else if (block_type == 11) Topborder = 1;//Z
	else if (block_type == 12) Topborder = 2;//!Z
	else if (block_type == 13) Topborder = 1;//!Z
	else if (block_type == 14) Topborder = 2;//山
	else if (block_type == 15) Topborder = 1;//山
	else if (block_type == 16) Topborder = 2;//山
	else if (block_type == 17) Topborder = 1;//山
	else if (block_type == 18) Topborder = 0;// -
	else if (block_type == 19) Topborder = 3;// -

	return Topborder + 1;
}
bool Block::isBottom(int head_x, int head_y, int block_type)
{
	//判断是否能继续向下移动  
	// mpos[head_x + i][head_y + j]   坐标

	int block[4][4];
	int LBorder = 0;
	if (block_type == 1)//田
	{
		block[2][2] = block[2][3] = block[3][2] = block[3][3] = 1;
		LBorder = 2;
	}
	else if (block_type == 2)//L
	{
		block[2][1] = block[2][2] = block[2][3] = block[3][3] = 1;
		LBorder = 2;

	}
	else if (block_type == 3)//L
	{
		LBorder = 1;
		block[1][2] = block[1][3] = block[2][2] = block[3][2] = 1;
	}
	else if (block_type == 4)//L
	{
		block[2][1] = block[3][2] = block[3][3] = block[3][1] = 1;
		LBorder = 2;
	}
	else if (block_type == 5)//L
	{
		block[3][2] = block[1][3] = block[2][3] = block[3][3] = 1;
		LBorder = 1;
	}
	else if (block_type == 6)//!L
	{
		block[3][1] = block[3][2] = block[3][3] = block[2][3] = 1;
		LBorder = 2;
	}
	else if (block_type == 7)//!L
	{
		block[1][2] = block[1][3] = block[2][3] = block[3][3] = 1;
		LBorder = 1;
	}
	else if (block_type == 8)//!L
	{
		block[2][1] = block[2][2] = block[2][3] = block[3][1] = 1;
		LBorder = 2;
	}
	else if (block_type == 9)//!L
	{
		block[1][2] = block[3][3] = block[2][2] = block[3][2] = 1;
		LBorder = 1;
	}
	else if (block_type == 10)//Z
	{
		block[3][1] = block[3][2] = block[2][2] = block[2][3] = 1;
		LBorder = 2;
	}
	else if (block_type == 11)//Z
	{
		block[1][2] = block[2][2] = block[2][3] = block[3][3] = 1;
		LBorder = 1;
	}
	else if (block_type == 12)//!Z
	{
		block[2][1] = block[2][2] = block[3][2] = block[3][3] = 1;
		LBorder = 2;
	}
	else if (block_type == 13)//!Z
	{
		block[1][3] = block[2][2] = block[2][3] = block[3][2] = 1;
		LBorder = 1;
	}
	else if (block_type == 14)//山
	{
		block[1][3] = block[2][2] = block[2][3] = block[3][3] = 1;
		LBorder = 1;
	}
	else if (block_type == 15)//山
	{
		block[2][1] = block[2][2] = block[2][3] = block[3][2] = 1;
		LBorder = 2;
	}
	else if (block_type == 16)//山
	{
		block[1][2] = block[2][2] = block[3][2] = block[2][3] = 1;
		LBorder = 1;
	}
	else if (block_type == 17)//山
	{
		block[2][2] = block[3][1] = block[3][2] = block[3][3] = 1;
		LBorder = 2;
	}
	else if (block_type == 18)// -
	{
		block[3][0] = block[3][1] = block[3][2] = block[3][3] = 1;
		LBorder = 3;
	}
	else if (block_type == 19)// -
	{
		block[0][3] = block[1][3] = block[2][3] = block[3][3] = 1;
		LBorder = 0;
	}

	int temp_flag[4] = { 0 };

	for (int i = LBorder; i < 4; i++) {
		if (mpos[head_x + i][head_y + 3].map_val == 1 && mpos[head_x + i][head_y + 4].map_val == 1 && block_type == 1)//田
			temp_flag[i] = 1;

		if (mpos[head_x + i][head_y + 3].map_val == 1 && mpos[head_x + i][head_y + 4].map_val == 1 && block_type == 2)//L1
			temp_flag[i] = 1;
		if (block[3][2] == 1 && mpos[head_x + 3][head_y + 3].map_val == 1 && block_type == 3)//L2
			temp_flag[i] = 1;
		if (block[2][2] == 1 && mpos[head_x + 2][head_y + 3].map_val == 1 && block_type == 3)//L2
			temp_flag[i] = 1;
		if (mpos[head_x + 2][head_y + 2].map_val == 1 && block_type == 4)//L3
			temp_flag[2] = 1;
		if (block[i][3] == 1 && mpos[head_x + i][head_y + 4].map_val == 1 && block_type == 5)//L4
			temp_flag[i] = 1;

		if (mpos[head_x + i][head_y + 2].map_val == 1 && mpos[head_x + i][head_y + 4].map_val == 1 && block_type == 6)//!L1
			temp_flag[i] = 1;
		if (mpos[head_x + 1][head_y + 4].map_val == 1 && block_type == 7)//!L2
			temp_flag[i] = 1;
		if (mpos[head_x + 2][head_y + 4].map_val == 1 && block_type == 7)//!L2
			temp_flag[i] = 1;
		if (mpos[head_x + 3][head_y + 2].map_val == 1 && block_type == 8)//!L3
			temp_flag[3] = 1;
		if (block[i][3] == 1 && mpos[head_x + i][head_y + 4].map_val == 1 && block_type == 9)//!L4
			temp_flag[i] = 1;

		if (mpos[head_x + 3][head_y + 3].map_val == 1 && mpos[head_x + 3][head_y + 4].map_val == 1 && block_type == 10)//Z1
			temp_flag[3] = 1;
		if (mpos[head_x + 1][head_y + 3].map_val == 1 && block_type == 11)//!Z2
			temp_flag[3] = 1;

		if (mpos[head_x + 2][head_y + 3].map_val == 1 && mpos[head_x + 2][head_y + 4].map_val == 1 && block_type == 12)//!Z1
			temp_flag[3] = 1;
		if (mpos[head_x + 3][head_y + 3].map_val == 1 && block_type == 13)//!Z2
			temp_flag[3] = 1;

		if (mpos[head_x + 3][head_y + 3].map_val == 1 && block_type == 15)//山2
			temp_flag[3] = 1;
		if (mpos[head_x + 2][head_y + 3].map_val == 1 && block_type == 17)//山4
			temp_flag[3] = 1;

		if (mpos[head_x + i][head_y + 4].map_val == 1 && block_type == 19)//-
			temp_flag[3] = 1;


		if (block[i][3] == 1 && mpos[head_x + i][head_y + 4].map_val == 1 && block_type != 1)//ALL
			temp_flag[i] = 1;

	}
	if (temp_flag[0] == 1 || temp_flag[1] == 1 || temp_flag[2] == 1 || temp_flag[3] == 1)
		return true;

	return false;
}
void Block::fullLine()
{
	int j, down_times;
	for (int i = 19; i >= 0; i--)//满行下移 
	{
		if (mpos[0][i].map_val == 1
			&& mpos[1][i].map_val == 1 && mpos[2][i].map_val == 1 && mpos[3][i].map_val == 1
			&& mpos[4][i].map_val == 1 && mpos[5][i].map_val == 1 && mpos[6][i].map_val == 1
			&& mpos[7][i].map_val == 1 && mpos[8][i].map_val == 1 && mpos[9][i].map_val == 1)//满行
		{
			for (int down_times = i; down_times > 0; down_times--)
			{
				for (int j = 9; j >= 0; j--)
				{
					mpos[j][down_times].map_val = mpos[j][down_times - 1].map_val;
					mpos[j][down_times].map_col = mpos[j][down_times - 1].map_col;
				}
				for (int m = 9; m >= 0; m--)// 1BLUE 2GREEN 3RED 4WHITE 5YELLOW 6CYAN  7BROWN 
				{
					for (int j = 19; j >= 0; j--)
					{
						if (mpos[m][j].map_col == 1) setfillcolor(BLUE);
						else if (mpos[m][j].map_col == 2) setfillcolor(GREEN);
						else if (mpos[m][j].map_col == 3) setfillcolor(RED);
						else if (mpos[m][j].map_col == 4) setfillcolor(WHITE);
						else if (mpos[m][j].map_col == 5) setfillcolor(YELLOW);
						else if (mpos[m][j].map_col == 6) setfillcolor(CYAN);
						else if (mpos[m][j].map_col == 7) setfillcolor(BROWN);
						else if (mpos[m][j].map_col == 0) setfillcolor(BLACK);
						floodfill(mpos[m][j].x, mpos[m][j].y, BLUE, 1);
						floodfill(mpos[m][j].x, mpos[m][j].y, WHITE, 1);
						floodfill(mpos[m][j].x, mpos[m][j].y, YELLOW, 1);
						floodfill(mpos[m][j].x, mpos[m][j].y, BROWN, 1);
						floodfill(mpos[m][j].x, mpos[m][j].y, CYAN, 1);
						floodfill(mpos[m][j].x, mpos[m][j].y, RED, 1);
						floodfill(mpos[m][j].x, mpos[m][j].y, GREEN, 1);
						floodfill(mpos[m][j].x, mpos[m][j].y, BLACK, 1);
					}
				}
			}
			score += 10;
			char tempScore[10];
			sprintf(tempScore, "%08d", score);
			settextstyle(50, 0, _T("楷体"));
			outtextxy(500, 850, tempScore);

		}
	}
}
bool Block::gameover()
{
	for (int i = 0; i < 10; i++)
	{
		if (mpos[i][0].map_val == 1)
		{
			RECT gamePause = { 480,640,719,740 };
			settextstyle(25, 0, _T("楷体"));
			settextcolor(WHITE);
			drawtext(_T("GAME OVER !!"), &gamePause, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//区域居中打印
			rank();
			menu();
			system("pause");

			settextcolor(BLACK);
			drawtext(_T("GAME OVER  !!"), &gamePause, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//覆盖掉白字Pause
		}
	}
	return true;
}
void Block::rank()//输入前十五个分数 以及对应的时间 ID
{
	int sco;
	no user[20]; no temp;
	char file_id[20];
	InputBox(file_id, 20, "Please input your ID :");

	setfillcolor(BLACK);
	fillrectangle(30, 30, 450, 930);
	settextcolor(YELLOW);
	settextstyle(30, 0, _T("黑体"));
	outtextxy(135, 80, "Tetris Ranking !!");
	settextcolor(BLUE);
	outtextxy(60, 120, "-------------------------");
	settextstyle(20, 0, _T("黑体"));
	settextcolor(CYAN);
	outtextxy(95, 150, "ID   | Score  |     Time");

	struct tm* newtime;
	char tmpbuf[128];
	time_t lt1;
	time(&lt1);
	newtime = localtime(&lt1);
	strftime(tmpbuf, 128, "%D %T", newtime);

	ofstream outtofile("rank.txt", ios::binary | ios::app);//输出到文件
	outtofile << file_id << endl;
	outtofile << Block::score << endl;
	outtofile << tmpbuf << endl;
	outtofile.close();

	ifstream Fromfile("rank.txt", ios::binary | ios::in);//从文件输入
	for (int i = 0; i < 20; i++)
	{

		Fromfile.getline(user[i].id, '\r\n');
		Fromfile.getline(user[i].score, '\r\n');
		user[i].score_int = atoi(user[i].score);
		Fromfile.getline(user[i].time, '\r\n');
	}
	Fromfile.close();

	for (int i = 0; i < 19; i++)
	{
		for (int j = 0; j < 19; j++)
		{
			if (user[i].score_int > user[i].score_int)
			{
				temp = user[i];
				user[i] = user[i + 1];
				user[i + 1] = temp;
			}
		}
	}

	ofstream outtofileagain("rank.txt", ios::binary | ios::out);//输出到文件
	for (int i = 0; i < 15; i++)
	{
		outtofileagain << user[i].id << endl;
		outtofileagain << user[i].score << endl;
		outtofileagain << user[i].time << endl;
	}
	outtofileagain.close();

	ifstream Fromfileagain("rank.txt", ios::binary | ios::in);//从文件输入
	char rank_id[20];
	char rank_sco_str[10];
	char rank_time[100] = { '\n' };
	for (int i = 0; i < 15; i++)
	{
		Fromfileagain.getline(rank_id, '\r\n');
		Fromfileagain.getline(rank_sco_str, '\r\n');
		Fromfileagain.getline(rank_time, '\r\n');

		settextstyle(20, 0, _T("黑体"));
		char no_str[3];
		int no = i + 1;
		itoa(no, no_str, 10);

		settextcolor(RED);
		outtextxy(50, 35 * i + 235, no_str);
		settextcolor(CYAN);
		outtextxy(80, 35 * i + 235, "|");
		outtextxy(145, 35 * i + 235, "|");
		outtextxy(235, 35 * i + 235, "|");
		settextcolor(WHITE);
		outtextxy(95, 35 * i + 235, rank_id);
		outtextxy(160, 35 * i + 235, rank_sco_str);
		outtextxy(250, 35 * i + 235, rank_time);
	}
	Fromfileagain.close();
	settextcolor(CYAN);
	settextstyle(50, 0, _T("黑体"));
	roundrect(100, 800, 380, 900, 1, 1);
	outtextxy(135, 820, "回到菜单");
	MOUSEMSG m;
	while (TRUE)
	{
		m = GetMouseMsg();
		for (int i = 0; i < 20; i++)
		{

			switch (m.uMsg)
			{
			case WM_LBUTTONDOWN:
				if (m.x >= 100 && m.x <= 380 && m.y >= 800 && m.y <= 900) menu();
			}
		}
	}
	system("pause");
}
void Block::run()
{
	initgraph(720, 960);
	init_game();
	int head_x = 2, head_y = -2;
	srand((unsigned)time(NULL));
	int block_type = rand() % 19 + 1;
	int nextBlock = next_block();
	int kb;
	drawBlock(head_x, head_y, block_type);
	cleanBlock(10, 5, 20);
	drawBlock(10, 5, nextBlock);

	while (TRUE)
	{
		
		drawBlock(10, 5, nextBlock);
		
		if (!_kbhit())
		{
			if (head_y <= 15 && !isBottom(head_x, head_y, block_type))
			{
				cleanBlock(head_x, head_y, block_type);
				fullLine();
				head_x = isLRBorder(head_x, block_type);
				head_y = down(head_x, head_y, block_type);
				drawBlock(head_x, head_y, block_type);
			}
			else //if (head_y > 15)
			{

				gameover();
				block_type = nextBlock;
				cleanBlock(10, 5, 20);
				nextBlock = next_block();
				//drawBlock(10, 5, nextBlock);


				head_x = 3;
				head_y = -TopBorder(head_x, head_y, block_type);
			}
			Sleep(gameSpeed);
			continue;

		}

		else if (_kbhit()) //判断有键按下时才执行下面的操作
		{
			kb = getche();   //用kb来获取键值
			if (kb == 224)
			{
				kb = _getch();//值为224时表示按下了方向键，要再次获取才行
				if (kb == 75 && !isBottom(head_x, head_y, block_type))//左
				{
					if (head_y <= 15)
					{
						if (!MoveLR(head_x, head_y, block_type))
							continue;

						cleanBlock(head_x, head_y, block_type);
						fullLine();
						head_x -= 1;
						head_x = isLRBorder(head_x, block_type);
						drawBlock(head_x, head_y, block_type);
					}
					else //if (head_y > 15)
					{
						gameover();
						block_type = nextBlock;
						cleanBlock(10, 5, 20);
						nextBlock = next_block();
						//drawBlock(10, 5, nextBlock);


						head_x = 2;
						head_y = -TopBorder(head_x, head_y, block_type);
					}
					continue;
				}
				if (kb == 72 && !isBottom(head_x, head_y, block_type))//上
				{
					if (head_y <= 15)
					{
						cleanBlock(head_x, head_y, block_type);
						fullLine();
						head_x = isLRBorder(head_x, block_type);
						if (block_type == 1)
						{
							block_type++;
							if (block_type > 1)
								block_type = 1;
						}
						else if (block_type >= 2 && block_type <= 5)
						{
							block_type++;
							if (block_type > 5)
								block_type = 2;
						}
						else if (block_type >= 6 && block_type <= 9)
						{
							block_type++;
							if (block_type > 9)
								block_type = 6;
						}
						else if (block_type >= 10 && block_type <= 11)
						{
							block_type++;
							if (block_type > 11)
								block_type = 10;
						}
						else if (block_type >= 12 && block_type <= 13)
						{
							block_type++;
							if (block_type > 13)
								block_type = 12;
						}
						else if (block_type >= 14 && block_type <= 17)
						{
							block_type++;
							if (block_type > 17)
								block_type = 14;
						}
						else if (block_type >= 18 && block_type <= 19)
						{
							block_type++;
							if (block_type > 19)
								block_type = 18;
						}
						drawBlock(head_x, head_y, block_type);
					}
					else //if (head_y > 15)
					{
						gameover();
						block_type = nextBlock;
						cleanBlock(10, 5, 20);
						nextBlock = next_block();
						//drawBlock(10, 5, nextBlock);


						head_x = 2;
						head_y = -TopBorder(head_x, head_y, block_type);
					}
					continue;
				}
				if (kb == 77 && !isBottom(head_x, head_y, block_type))//右
				{
					if (head_y <= 15)
					{
						if (!MoveLR(head_x, head_y, block_type))
							continue;

						cleanBlock(head_x, head_y, block_type);
						fullLine();
						head_x += 1;
						head_x = isLRBorder(head_x, block_type);
						drawBlock(head_x, head_y, block_type);
					}
					else //if (head_y > 15)
					{
						gameover();
						block_type = nextBlock;
						cleanBlock(10, 5, 20);
						nextBlock = next_block();
						//drawBlock(10, 5, nextBlock);


						head_x = 2;
						head_y = -TopBorder(head_x, head_y, block_type);
					}
					continue;
				}
				if (kb == 80 && !isBottom(head_x, head_y, block_type))//下
				{
					if (head_y <= 15 && !isBottom(head_x, head_y, block_type))
					{
						cleanBlock(head_x, head_y, block_type);
						fullLine();
						head_x = isLRBorder(head_x, block_type);
						head_y += 1;
						drawBlock(head_x, head_y, block_type);

					}
					else //if (head_y > 15)
					{
						gameover();
						block_type = nextBlock;
						cleanBlock(10, 5, 20);
						nextBlock = next_block();
						//drawBlock(10, 5, nextBlock);

						head_x = 2;
						head_y = -TopBorder(head_x, head_y, block_type);
					}
					continue;
				}
			}
			else if (kb == 'f' && !isBottom(head_x, head_y, block_type))
			{

				RECT gamePause = { 480,640,719,740 };

				settextcolor(WHITE);
				settextstyle(25, 0, _T("楷体"));
				drawtext(_T("GAME PAUSE !!"), &gamePause, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//区域居中打印
				system("pause");
				settextcolor(BLACK);
				drawtext(_T("GAME PAUSE !!"), &gamePause, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//区域居中打印
			}
			else if (kb == 32 && !isBottom(head_x, head_y, block_type))
			{
				while (head_y <= 15 && !isBottom(head_x, head_y, block_type))
				{
					cleanBlock(head_x, head_y, block_type);
					fullLine();
					head_x = isLRBorder(head_x, block_type);
					head_y = down(head_x, head_y, block_type);
					drawBlock(head_x, head_y, block_type);
				}
				gameover();
				block_type = nextBlock;
				cleanBlock(10, 5, 20);
				nextBlock = next_block();

				//drawBlock(10, 5, nextBlock);


				head_x = 2;
				head_y = -TopBorder(head_x, head_y, block_type);
				continue;
			}
			continue;
		}


	}

}
void Block::menu() {
	User usr;
	cleardevice();
	initgraph(720, 960);
	init_game();
	settextcolor(YELLOW);
	outtextxy(145, 80, "Tetris Game!!");
	settextstyle(50, 0, _T("黑体"));
	settextcolor(CYAN);	
	setlinestyle(PS_DASH, 2);
	roundrect(90, 190, 420, 310, 1, 1);
	roundrect(90, 320, 420, 440, 1, 1);
	roundrect(90, 450, 420, 570, 1, 1);
	roundrect(90, 590, 420, 710, 1, 1);
	outtextxy(95, 220, "1.Start Game ");
	outtextxy(95, 350, "2.Exit  Game ");
	outtextxy(95, 480, "3.Ranking !! ");
	outtextxy(95, 620, "4.Exit Login ");
	MOUSEMSG m;
	while (TRUE)
	{
		m = GetMouseMsg();
		for (int i = 0; i < 20; i++)
		{
			switch (m.uMsg)
			{
			case WM_LBUTTONDOWN:
				if (m.x >= 90 && m.x <= 420 && m.y >= 190 && m.y <= 310) run();
				if (m.x >= 90 && m.x <= 420 && m.y >= 320 && m.y <= 440) exit(0);
				if (m.x >= 90 && m.x <= 420 && m.y >= 450 && m.y <= 570) rank();
				if (m.x >= 90 && m.x <= 420 && m.y >= 590 && m.y <= 710) usr.login();
			}
		}
	}
	int kb;
	while (TRUE)
	{
		if (_kbhit()) //判断有键按下时才执行下面的操作
		{
			kb = getche();   //用kb来获取键值
			switch (kb)
			{
			case 49:/*closegraph()*/;run();
			case 50:exit(0);
			case 51:rank();menu();
			case 52:usr.login();
			}
		}
	}
	closegraph();
}
bool User::reg()
{
	
	User* user = new User;
	InputBox(user->name, 20, "请输入用户名: ");
	ifstream checkname("User.txt", ios::binary | ios::in);
	char tempName[20]={0};
	char tempPasswd[20] = {0};
	here:
	for (int i = 0; !checkname.eof() ; i++)
	{
		checkname.getline(tempName, '\r\n');
		checkname.getline(tempPasswd, '\r\n');
		if (strcmp(tempName, user->name) == 0)
		{
			InputBox(user->name, 20, "已有此用户! !\n请重新输入用户名: ");
			goto here;
		}
	}
	InputBox(user->password, 20, "请输入密码: ");
	checkname.close();
	ofstream newAccount("User.txt", ios::binary | ios::app);
	newAccount << user->name << endl;
	newAccount << user->password << endl;
	newAccount.close();
	login();
	return true;
}
bool User::login()
{
	ifstream loginFromfile("User.txt", ios::binary | ios::in);
	Users Usr[100];
	for (int i = 0; !loginFromfile.eof(); i++)
	{
		loginFromfile >> Usr[i].name;
		loginFromfile >> Usr[i].password;
	}

	initgraph(720, 960);
	b.init_game();
	settextcolor(YELLOW);
	outtextxy(145, 80, "Tetris Game!!");
	settextstyle(50, 0, _T("黑体"));
	settextcolor(CYAN);	setlinestyle(PS_DASH, 2);
	roundrect(90, 190, 450, 310, 1, 1);
	roundrect(90, 320, 450, 440, 1, 1);
	roundrect(90, 450, 450, 570, 1, 1);
	roundrect(90, 590, 450, 710, 1, 1);
	outtextxy(95, 220, "1.User  Login ");
	outtextxy(95, 350, "2.Admin Login ");
	outtextxy(95, 480, "3.Register    ");
	outtextxy(95, 620, "4.Exit  Game  ");
	MOUSEMSG m;
	User usr;
	Admin administrator;
	char logName[20];
	char logPasswd[20] = { 0 };
	int count = 0;
	int kb=0;
	string s = password;
	char disp[20] = { 0 };
	char stringdisp[100];
	int i = 0;

	while (TRUE)
	{
		m = GetMouseMsg();
		for (int i = 0; i < 20; i++)
		{
			switch (m.uMsg)
			{
			case WM_LBUTTONDOWN:
				if (m.x >= 90 && m.x <= 420 && m.y >= 190 && m.y <= 310) {
					count = 0;
					InputBox(logName, 20, "输入用户名: ");
					closegraph();
					initgraph(300, 100);
					outtextxy(30, 40, "password:");
					roundrect(100, 35, 250, 60, 2, 2);
					logPasswd[i] = _getch();
					while (1) {
						if (i >= 20 || logPasswd[i] == 13)
							break;
						for (int j = 0; j < i + 1; j++)
						{
							disp[j] = '*';
							disp[j + 1] = '\0';
						}
						sprintf(stringdisp, "password: %s", disp);
						roundrect(100, 35, 250, 60, 2, 2);
						outtextxy(30, 40, stringdisp);

						if (_kbhit)
						{
							if (logPasswd[i] == '\b')
							{
								//cleardevice();
								outtextxy(239 - ((19 - i) * 7), 40, " ");							logPasswd[i] = { '\0' };
								i--;
							}
							else if (logPasswd[i] == 13)
							{
								continue;
							}
							else
							{
								i++;
							}
							logPasswd[i] = _getch();
						}
					}
					logPasswd[i] = '\0';
					for (int i = 0; i < 100; i++)
					{
						if (strcmp(logName, Usr[i].name) == 0 && strcmp(logPasswd, Usr[i].password) == 0)
						{
							count++;
							break;
						}
					}
					if (count != 0)
						b.menu();
					else
					{
						cleardevice();
						outtextxy(30, 40, "password wrong");
						Sleep(2000);
						usr.login();
					}
				}
				if (m.x >= 90 && m.x <= 420 && m.y >= 320 && m.y <= 440) {
					count = 0;
					InputBox(logName, 20, "输入用户名: ");
					closegraph();
					initgraph(300, 100);
					settextstyle(15, 7, _T("黑体"));
					outtextxy(30, 40, "password:");
					roundrect(100, 35, 250, 60, 2, 2);
					logPasswd[i] = _getch();
					while (1) {
						if (i >= 20 || logPasswd[i] == 13)
							break;
						for (int j = 0; j < i +1; j++)
						{
							disp[j] = '*';
							disp[j + 1] = '\0';
						}
						sprintf(stringdisp, "password: %s", disp);
						roundrect(100, 35, 250, 60, 2, 2);
						outtextxy(30, 40, stringdisp);

						if (_kbhit)
						{
							if (logPasswd[i] == '\b')
							{
								//cleardevice();
								outtextxy(239-((19-i)*7), 40, " ");
								logPasswd[i] = { '\0' };
								i--;
							}
							else if(logPasswd[i]==13)
							{
								continue;
							}
							else
							{
								i++;
							}
							logPasswd[i] = _getch();
						}
					}
					logPasswd[i] = '\0';

					if (strcmp(logName, "Admin") == 0 && strcmp(logPasswd, "password") == 0)
					{
						administrator.menu();
						break;
					}
					else
					{
						cleardevice();
						outtextxy(30, 40, "password wrong");
						Sleep(2000);
						usr.login();
					}
				}
				if (m.x >= 90 && m.x <= 420 && m.y >= 450 && m.y <= 570) usr.reg();
				if (m.x >= 90 && m.x <= 420 && m.y >= 590 && m.y <= 710) exit(0);
			}
		}
	}
	while (TRUE)
	{
		if (_kbhit()) //判断有键按下时才执行下面的操作
		{
			kb = getche();   //用kb来获取键值
			switch (kb)
			{
			case 49:
				count = 0;
				InputBox(logName, 20, "输入用户名: ");
				closegraph();
				initgraph(300, 100);
				outtextxy(30, 40, "password:");
				roundrect(100, 35, 250, 60, 2, 2);
				logPasswd[i] = _getch();
				while (1) {
					if (i >= 20 || logPasswd[i] == 13)
						break;
					for (int j = 0; j < i + 1; j++) 
					{
						disp[j] = '*';
						disp[j + 1] = 0;
					}
					sprintf(stringdisp, "password: %s", disp);
					roundrect(100, 35, 250, 60, 2, 2);
					outtextxy(30, 40, stringdisp);
					if (_kbhit) {

						if (kb ==8)
						{
							logPasswd[i] = { 0 };
							i--;
						}
						else
						{
							logPasswd[i] = _getch();
							i++;
							

						}
					}
				}
				logPasswd[i] = '\0';
				for (int i = 0; i < 100; i++)
				{					
					if (strcmp(logName, Usr[i].name) == 0 && strcmp(logPasswd, Usr[i].password) == 0)
					{
						count++;
						break;
					}
				}
				if(count!=0)
				b.menu();
				else
				{
					cleardevice();
					outtextxy(30, 40, "password wrong");
					Sleep(2000);
					usr.login();
				}
			case 50:
				count = 0;
				InputBox(logName, 20, "输入用户名: ");
				closegraph();
				initgraph(300, 100);
				outtextxy(30, 40, "password:");
				roundrect(100, 35, 250, 60, 2, 2);
				logPasswd[i] = _getch();
				while (1) {
					if (i >= 20 || logPasswd[i] == 13)
						break;
					for (int j = 0; j < i + 1; j++) 
					{
							disp[j] = '*';
							//disp[j + 1] = '\n';
					}
					sprintf(stringdisp, "password: %s", disp);
					roundrect(100, 35, 250, 60, 2, 2);
					outtextxy(30, 40, stringdisp);
					if (_kbhit) 
					{
						i++;
						logPasswd[i] = _getch();
					}
				}
				logPasswd[i] = '\0';

				if (strcmp(logName, "Admin") == 0 && strcmp(logPasswd, "password") == 0)
				{
					administrator.menu();
					break;
				}
				else
				{
					cleardevice();
					outtextxy(30, 40, "password wrong");
					Sleep(2000);
					usr.login();
				}
			case 51:

				usr.reg();
				break;

			case 52:exit(0);
			}
		}
	}
	closegraph();

	loginFromfile.close();
	return true;
}
void Admin::menu() {
	initgraph(720, 960);
	b.init_game();
	settextcolor(YELLOW);
	settextstyle(30, 0, _T("黑体"));
	outtextxy(145, 80, "Tetris Game!!");
	settextstyle(50, 0, _T("黑体"));
	settextcolor(CYAN);
	setlinestyle(PS_DASH, 2);
	roundrect(90, 190, 420, 310, 1, 1);
	roundrect(90, 320, 420, 440, 1, 1);
	roundrect(90, 450, 420, 570, 1, 1);
	roundrect(90, 590, 420, 710, 1, 1);
	outtextxy(95, 220, "1.Start Game ");
	outtextxy(95, 350, "2.Exit  Game ");
	outtextxy(95, 480, "3.Ranking !! ");
	outtextxy(95, 620, "4.User  list ");
	MOUSEMSG m;
	while (TRUE)
	{
		m = GetMouseMsg();
		for (int i = 0; i < 20; i++)
		{
			switch (m.uMsg)
			{
			case WM_LBUTTONDOWN:
				if (m.x >= 90 && m.x <= 420 && m.y >= 190 && m.y <= 310) b.run();
				if (m.x >= 90 && m.x <= 420 && m.y >= 320 && m.y <= 440) exit(0);
				if (m.x >= 90 && m.x <= 420 && m.y >= 450 && m.y <= 570) b.rank();
				if (m.x >= 90 && m.x <= 420 && m.y >= 590 && m.y <= 710)
				{
					initgraph(720, 960);
					b.init_game();
					settextcolor(YELLOW);
					settextstyle(30, 0, _T("黑体"));
					outtextxy(135, 80, "User List !!");
					settextstyle(20, 0, _T("黑体"));
					settextcolor(CYAN);
					outtextxy(50, 135, "No");
					outtextxy(95, 135, "ID");
					outtextxy(200, 135, "Password");
					outtextxy(310, 135, "operator");
					ifstream loginFromfile("User.txt", ios::binary | ios::in);
					Users Usr[20];
					MOUSEMSG m;
					for (int i = 0; i < 20; i++)
					{
						loginFromfile.getline(Usr[i].name, '\r\n');
						loginFromfile.getline(Usr[i].password, '\r\n');
					}
					loginFromfile.close();
					for (int i = 0; i < 20; i++)
					{
						settextstyle(20, 0, _T("黑体"));
						char no_str[3];
						int no = i + 1;
						itoa(no, no_str, 10);
						settextcolor(RED);
						outtextxy(50, 35 * i + 175, no_str);
						settextcolor(CYAN);
						outtextxy(300, 35 * i + 175, "|");
						outtextxy(190, 35 * i + 175, "|");
						outtextxy(80, 35 * i + 175, "|");
						settextcolor(WHITE);
						outtextxy(95, 35 * i + 175, Usr[i].name);
						outtextxy(200, 35 * i + 175, Usr[i].password);
						setlinestyle(PS_DASH, 2);
						settextstyle(19, 0, _T("黑体"));
						settextcolor(LIGHTGRAY);
						roundrect(310, 35 * i + 175, 380, 35 * i + 195, 1, 1);
						outtextxy(325, 35 * i + 175, "改密");
						roundrect(390, 35 * i + 175, 460, 35 * i + 195, 1, 1);
						outtextxy(405, 35 * i + 175, "删除");
						settextstyle(30, 0, _T("黑体"));
						roundrect(100, 885, 380, 930, 1, 1);
						outtextxy(170, 890, "回到菜单");
					}
					while (TRUE)
					{
						m = GetMouseMsg();
						for (int i = 0; i < 20; i++)
						{

							switch (m.uMsg)
							{
							case WM_LBUTTONDOWN:
								if (m.x >= 100 && m.x <= 380 && m.y >= 885 && m.y <= 930) menu();
								if (m.x >= 310 && m.x <= 380 && m.y >= 35 * i + 175 && m.y <= 35 * i + 195)//改密
								{
									ofstream chpwd("User.txt", ios::binary | ios::out);

									if (Usr[i].name == 0) break;
									InputBox(Usr[i].password, 20, "请输入修改后的密码: ");
									for (int i = 0; i < 20; i++)
									{
										chpwd << Usr[i].name << endl;
										chpwd << Usr[i].password << endl;
									}
									chpwd.close();
									menu();
								}
								if (m.x >= 390 && m.x <= 460 && m.y >= 35 * i + 175 && m.y <= 35 * i + 195)//删除
								{
									ofstream delusr("User.txt", ios::binary | ios::out);
									if (Usr[i].name == 0) break;
									for (int j = i; j < 19; j++)
									{
										Usr[j] = Usr[j + 1];
									}

									for (int i = 0; i < 20; i++)
									{
										delusr << Usr[i].name << endl;
										delusr << Usr[i].password << endl;
									}
									delusr.close();
									menu();

								}
								break;
							}
						}
					}
					menu();
				}
			}
		}
	}
	int kb;
	while (TRUE)
	{
		if (_kbhit()) //判断有键按下时才执行下面的操作
		{
			kb = getche();   //用kb来获取键值
			switch (kb)
			{
			case 49:/*closegraph();*/ b.run(); break;
			case 50:exit(0);
			case 51:b.rank(); menu();
			case 52:
				initgraph(720, 960);
				b.init_game();
				settextcolor(YELLOW);
				settextstyle(30, 0, _T("黑体"));
				outtextxy(135, 80, "User List !!");
				settextstyle(20, 0, _T("黑体"));
				settextcolor(CYAN);
				outtextxy(50, 135, "No");
				outtextxy(95, 135, "ID");
				outtextxy(200, 135, "Password");
				outtextxy(310, 135, "operator");
				ifstream loginFromfile("User.txt", ios::binary | ios::in);
				Users Usr[20];
				MOUSEMSG m;
				for (int i = 0; i < 20; i++)
				{
					loginFromfile.getline(Usr[i].name, '\r\n');
					loginFromfile.getline(Usr[i].password, '\r\n');
				}
				loginFromfile.close();
				for (int i = 0; i < 20; i++)
				{
					settextstyle(20, 0, _T("黑体"));
					char no_str[3];
					int no = i + 1;
					itoa(no, no_str, 10);
					settextcolor(RED);
					outtextxy(50, 35 * i + 175, no_str);
					settextcolor(CYAN);
					outtextxy(300, 35 * i + 175, "|");
					outtextxy(190, 35 * i + 175, "|");
					outtextxy(80, 35 * i + 175, "|");
					settextcolor(WHITE);
					outtextxy(95, 35 * i + 175, Usr[i].name);
					outtextxy(200, 35 * i + 175, Usr[i].password);
					setlinestyle(PS_DASH, 2);
					settextstyle(19, 0, _T("黑体"));
					settextcolor(LIGHTGRAY);
					roundrect(310, 35 * i + 175, 380, 35 * i + 195, 1, 1);
					outtextxy(325, 35 * i + 175, "改密");
					roundrect(390, 35 * i + 175, 460, 35 * i + 195, 1, 1);
					outtextxy(405, 35 * i + 175, "删除");
					settextstyle(30, 0, _T("黑体"));
					roundrect(100, 885, 380, 930, 1, 1);
					outtextxy(170, 890, "回到菜单");
				}
				while (TRUE)
				{
					m = GetMouseMsg();
					for (int i = 0; i < 20; i++)
					{

						switch (m.uMsg)
						{
						case WM_LBUTTONDOWN:
							if (m.x >= 100 && m.x <= 380 && m.y >= 885 && m.y <= 930) menu();
							if (m.x >= 310 && m.x <= 380 && m.y >= 35 * i + 175 && m.y <= 35 * i + 195)//改密
							{
								ofstream chpwd("User.txt", ios::binary | ios::out);

								if (Usr[i].name == 0) break;
								InputBox(Usr[i].password, 20, "请输入修改后的密码: ");
								for (int i = 0; i < 20; i++)
								{
									chpwd << Usr[i].name << endl;
									chpwd << Usr[i].password << endl;
								}
								chpwd.close();
								menu();
							}
							if (m.x >= 390 && m.x <= 460 && m.y >= 35 * i + 175 && m.y <= 35 * i + 195)//删除
							{
								ofstream delusr("User.txt", ios::binary | ios::out);
								if (Usr[i].name == 0) break;
								for (int j = i; j < 19; j++)
								{
									Usr[j] = Usr[j + 1];
								}

								for (int i = 0; i < 20; i++)
								{
									delusr << Usr[i].name << endl;
									delusr << Usr[i].password << endl;
								}
								delusr.close();
								menu();

							}
							break;
						}
					}
				}
				menu();




			}
		}
	}
	closegraph();
}

int main()
{
	for (int i = 0; i < 15; i++)//划分地图为 48*48  一格
	{
		static int flagx = 10;
		int flagy = 10;
		for (int j = 0; j < 25; j++)
		{
			mpos[i][j].x = flagx;
			mpos[i][j].y = flagy;
			mpos[i][j].map_val = 0;
			flagy += 48;
		}
		flagx += 48;
	}
	User  usr;
	Admin ad;
	Block block;
	//block.run();
	//ad.menu();
 	usr.login();
	return 0;
}
