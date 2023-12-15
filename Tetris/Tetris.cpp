#include <iostream>
#include <thread>
#include<ctime>
#include <vector>
using namespace std;

//string用于存储窄字符(如ASCII字符)
//wstring用于存储宽字符(如Unicode字符).string使用单个字节来表示每个字符而wstring使用多个字节或宽字符来表示每个字符.
//因此,wstring可以更好地支持各种语言和特殊字符集,包括非拉丁字符、表情符号等.
//L前缀用于将字符串字面量标记为宽字符字符串.这可以让编译器知道该字符串是以宽字符形式存储的

#include <stdio.h>
#include <Windows.h>

int nScreenWidth = 80;			// CMD宽度
int nScreenHeight = 30;			// CMD高度
wstring tetromino[7];           //长度为7的字符串数组,用于保存7种方块
int nFieldWidth = 12;           //场宽
int nFieldHeight = 18;          //场高

//pField是一个指向无符号字符的指针,初始化为 nullptr.
//这个指针通常用于动态分配内存,并表示场地的状态或布局.通过使用指针,可以在程序运行时为场地分配所需的内存空间.
unsigned char* pField = nullptr;

int Rotate(int px, int py, int r)//x轴,y轴,旋转次数
{
	int pi = 0;
	switch (r % 4)
	{
	case 0: // 0°			        // 0  1  2  3
		pi = py * 4 + px;			// 4  5  6  7
		break;						// 8  9 10 11
		                            //12 13 14 15

	case 1: // 90°			        //12  8  4  0
		pi = 12 + py - (px * 4);	//13  9  5  1
		break;						//14 10  6  2
		                            //15 11  7  3

	case 2: // 180°			        //15 14 13 12
		pi = 15 - (py * 4) - px;	//11 10  9  8
		break;						// 7  6  5  4
		                            // 3  2  1  0

	case 3: // 270°			        // 3  7 11 15
		pi = 3 - py + (px * 4);		// 2  6 10 14
		break;						// 1  5  9 13
	}								// 0  4  8 12

	return pi;
}

//检查方块是否适合放在指定位置
//nTetromino代表方块类型，nRotation代表方块旋转状态，nPosX代表要放置方块位置的X坐标，nPosY代表要放置方块位置的Y坐标
bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
{
	// All Field cells >0 are occupied
	for (int px = 0; px < 4; px++)
		for (int py = 0; py < 4; py++)
		{
			// 获取方块内部位置的索引
			int pi = Rotate(px, py, nRotation);

			// 获取方块在游戏区域中的索引
			int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

			// Check that test is in bounds. Note out of bounds does
			// not necessarily mean a fail, as the long vertical piece
			// can have cells that lie outside the boundary, so we'll
			// just ignore them
			if (nPosX + px >= 0 && nPosX + px < nFieldWidth)//检查方块是否在横向范围内
			{
				if (nPosY + py >= 0 && nPosY + py < nFieldHeight)//检查方块是否在纵向范围内
				{
					if (tetromino[nTetromino][pi] != L'.' && pField[fi] != 0)//碰撞检测
						return false; //第一次碰撞时就返回FALSE
				}
			}
		}

	return true;
}

int main()
{
	//创建屏幕缓冲区
	
	//wchar_t 是一种用于表示宽字符的数据类型，通常在Windows中用于支持Unicode字符集

	//首先，使用 new 操作符为屏幕缓冲区分配内存，大小为 nScreenWidth* nScreenHeight 个宽字符(wchar_t)

	//然后，通过循环，将数组screen中的每个元素都设置为宽字符空格（L' '），表示屏幕缓冲区初始化为空白

	//接下来，调用CreateConsoleScreenBuffer函数创建一个新的控制台屏幕缓冲区，并将其句柄保存在hConsole变量中。该函数参数中的 GENERIC_READ | GENERIC_WRITE 表示该缓冲区可供读取和写入

	//4,SetConsoleActiveScreenBuffer 函数将当前活动的控制台屏幕缓冲区设置为刚刚创建的缓冲区。这将使得我们可以在控制台上显示缓冲区中的内容

	//5,声明了一个名为 dwBytesWritten 的 DWORD 变量用于记录写入到控制台屏幕缓冲区的字节数.DWORD实际上是unsigned long的别名，可以用于定义一个32位无符号整数变量

	//目的:创建一个带有空格字符初始化的屏幕缓冲区，并将其设置为活动的屏幕缓冲区，以便后续可以将字符输出到控制台屏幕上

	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; ++i) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	tetromino[0].append(L"..X...X...X...X."); //7种4x4方块,结尾追加字符
	tetromino[1].append(L"..X..XX...X.....");
	tetromino[2].append(L".....XX..XX.....");
	tetromino[3].append(L"..X..XX..X......");
	tetromino[4].append(L".X...XX...X.....");
	tetromino[5].append(L".X...X...XX.....");
	tetromino[6].append(L"..X...X..XX.....");

	//动态分配内存,通过pField可以访问到大小为nFieldWidth * nFieldHeight的内存
	pField = new unsigned char[nFieldWidth * nFieldHeight];
	for (int x = 0; x < nFieldWidth; x++)
		for (int y = 0; y < nFieldHeight; y++)
			//设立边界，如果（y * 宽度 + x）表示索引 i ，pField[i] = ...表示，如果是边界，赋值9，内部，则赋值0
	        //x == 0，左边界      x == nFieldWidth - 1，右边界        y == nFieldHeight - 1，下边界
	        //因为方块从上方出现，所以不需要上边界
			pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;//9代表#,0代表空格
	

	//游戏逻辑
	bool bKey[4];
	int nCurrentPiece = 0;
	int nCurrentRotation = 0;
	int nCurrentX = nFieldWidth / 3;//初始水平位置为游戏场地中央
	int nCurrentY = 0;
	int nSpeed = 20;//初始下落速度变量
	int nSpeedCount = 0;//计数器，用于记录方块下落的帧数
	bool bForceDown = false;//标记方块是否强制向下移动
	bool bRotateHold = true;//记录玩家是否按住了Z键
	int nPieceCount = 0;//难度设置
	int nScore = 0;//初始分数
	vector<int> vLines;//存储满行时的=,方便以后消除
	bool bGameOver = false;

	while (!bGameOver) //主循环
	{
		srand((unsigned int)time(NULL));
		// 计时    =======================
		/*this_thread::sleep_for(50ms) 是一个C++中的线程操作，用于使当前线程暂停执行一段时间
		this_thread 是C++标准库中的一个命名空间，提供了与线程相关的函数和类
		sleep_for 是this_thread命名空间中的一个函数，用于使当前线程暂停执行指定的时间段*/
		this_thread::sleep_for(50ms); //用于控制游戏帧率
		nSpeedCount++;//计数器自加1
		bForceDown = (nSpeedCount == nSpeed);//经过20ms(初始难度)*50即1秒(1帧)后,方块下落一次

		// 输入    ========================
		for (int k = 0; k < 4; k++)								// R   L   D Z
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;//将获取的按键状态存储到数组bKey中的操作
		//0x8000 是一个十六进制数，用于掩码操作，目的是检查按键状态中的最高位是否被置位
		//GetAsyncKeyState()是一个 Windows API 函数，用于检查指定虚拟键码对应的按键状态。它返回一个包含按键状态信息的值
		
		//0x8000，二进制表示为 1000000000000000
		/*而 GetAsyncKeyState() 函数，可以获取指定虚拟键码对应的按键状态
		该函数会返回一个值，最高位表示按键状态，1表示按下，0表示未按下
		再通过按位与& ，已知0x8000最高位是1，如果按键按下了，那么& 的结果就为1
		将 1 存储到 bKey[] 中*/


		// 游戏逻辑 ===================

		//限制方块不出界(碰撞检测)
		//嵌套循环可以用&&和三目运算符优化
		nCurrentX += (bKey[0] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;
		nCurrentX -= (bKey[1] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;
		nCurrentY += (bKey[2] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0;

		//防止按住Z时一直旋转
		if (bKey[3])
		{
			nCurrentRotation += (bRotateHold && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;
			bRotateHold = false;
		}
		else
			bRotateHold = true;

		//若bForceDown为真，则强制下移方块
		if (bForceDown)
		{
			//每50个方块提高难度
			nSpeedCount = 0;
			nPieceCount++;
			if (nPieceCount % 50 == 0)
				if (nSpeed >= 10) nSpeed--;

			//通过DoesPieceFit函数判断方块是否能继续向下移动
			//能移动，Y坐标自加1
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
				nCurrentY++;
			//不能移动，则有以下操作
			//将当前方块的形状锁定在游戏场景数组中的对应位置
			//检查是否有完整的行被填满，可以执行消除行的操作
			//选择下一个方块的初始位置和形状
			//如果新的方块无法放置在指定位置，则将游戏状态标记为结束（bGameOver为true）
			else
			{
				//方块不能继续下落后,将其锁定至当前位置上
				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
						if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != L'.')
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;//此时!=0代表被占用了,于是发生碰撞检测

				//检测同一行是否已满
				for (int py = 0; py < 4; py++)
					if (nCurrentY + py < nFieldHeight - 1)
					{
						bool bLine = true;
						for (int px = 1; px < nFieldWidth - 1; px++)//排除左右边界
							bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;//任何数与0都是0,所有bLine返回false

						if (bLine)
						{
							//将整行设置为=
							for (int px = 1; px < nFieldWidth - 1; px++)//排除左右边界
								pField[(nCurrentY + py) * nFieldWidth + px] = 8;
							vLines.push_back(nCurrentY + py);//将整行=存入vLine中,以便后续消除
						}
					}

				nScore += 25;
				if (!vLines.empty())	nScore += (1 << vLines.size()) * 100;//2的vLines.size()次方*100

				//设置下一个方块
				nCurrentX = nFieldWidth / 2;
				nCurrentY = 0;
				nCurrentRotation = 0;
				nCurrentPiece = rand() % 7;//真随机要用srand(其实也是伪随机)

				//装不下就GAME OVER
				bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
			}
		}

		// 显示 ======================

		// 绘制场地
		for (int x = 0; x < nFieldWidth; x++)
			for (int y = 0; y < nFieldHeight; y++)
				//(y+2)和(x+2)是偏移量,使方块能够正常显示
				screen[(y+2) * nScreenWidth + x+2] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]];//9代表#,0代表空格,这样避免了使用复杂的switch和if嵌套

		// 绘制俄罗斯方块
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != L'.')
					screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;//+2代表偏移量,+65表示转换至大写

		//绘制分数
		//swprintf_s() 是一个格式化字符串的函数，用于将格式化后的内容写入一个 wide character 字符串中
		
		//int swprintf_s(wchar_t* buffer, size_t sizeInWords, const wchar_t* format, ...)
		//buffer：指向目标字符串的指针。格式化后的内容将被写入到这个字符串中
		//sizeInWords：目标字符串的大小（以字节为单位）或允许写入的最大字符数。在进行写入操作时，要确保目标字符串具有足够的空间来容纳格式化后的内容
		//format：格式化字符串，用于指定输出的格式
		//...：可变数量的参数，用于根据 format 中的格式指定要插入的值
		swprintf_s(&screen[2 * nScreenWidth + nFieldWidth + 6], 16, L"SCORE: %8d", nScore);

		//一行满时消除整行
		if (!vLines.empty())
		{
			// Display Frame (cheekily to draw lines)
			
			//WriteConsoleOutputCharacterW 是一个 Windows API 函数，用于将字符写入控制台的输出缓冲区。
			//	参数解释如下：
			//	hConsole：控制台输出句柄，表示要写入的目标控制台窗口。
			//	screen：指向包含要写入的字符数据的字符数组的指针。在这段代码中，它是指向 screen 数组的指针。
			//	nScreenWidth* nScreenHeight：要写入的字符数目，即屏幕宽度乘以屏幕高度。表示输出缓冲区的大小。
			//{ 0,0 }：一个用于指定写入操作开始位置的坐标的 COORD 结构体。在这里，{ 0,0 } 表示从输出缓冲区左上角开始写入。
			//	& dwBytesWritten：一个指向 DWORD 类型变量的指针，用于接收实际写入的字符数量。
			//	综上所述，这段代码的作用是将 screen 数组中的字符数据写入到控制台的输出缓冲区中，并显示在控制台窗口上。
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
			this_thread::sleep_for(400ms); //400ms延迟后,=行消失

			//for (auto& v : vLines)等价于
			//for (auto it = vLines.begin(); it != vLines.end(); ++it) {
			//auto& v = *it;}
			for (auto& v : vLines)
				for (int px = 1; px < nFieldWidth - 1; px++)
				{
					for (int py = v; py > 0; py--)
						pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];//消除完成后将上面的行下移一行
					pField[px] = 0;
				}

			vLines.clear();//消除=行
		}

		//显示框架
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}

	CloseHandle(hConsole);//用于关闭句柄
	cout << "GAME OVER!! Score:" << nScore << endl;
	system("pause");
	return 0;
}