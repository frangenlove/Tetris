#include <iostream>
#include <thread>
#include<ctime>
#include <vector>
using namespace std;

//string���ڴ洢խ�ַ�(��ASCII�ַ�)
//wstring���ڴ洢���ַ�(��Unicode�ַ�).stringʹ�õ����ֽ�����ʾÿ���ַ���wstringʹ�ö���ֽڻ���ַ�����ʾÿ���ַ�.
//���,wstring���Ը��õ�֧�ָ������Ժ������ַ���,�����������ַ���������ŵ�.
//Lǰ׺���ڽ��ַ������������Ϊ���ַ��ַ���.������ñ�����֪�����ַ������Կ��ַ���ʽ�洢��

#include <stdio.h>
#include <Windows.h>

int nScreenWidth = 80;			// CMD���
int nScreenHeight = 30;			// CMD�߶�
wstring tetromino[7];           //����Ϊ7���ַ�������,���ڱ���7�ַ���
int nFieldWidth = 12;           //����
int nFieldHeight = 18;          //����

//pField��һ��ָ���޷����ַ���ָ��,��ʼ��Ϊ nullptr.
//���ָ��ͨ�����ڶ�̬�����ڴ�,����ʾ���ص�״̬�򲼾�.ͨ��ʹ��ָ��,�����ڳ�������ʱΪ���ط���������ڴ�ռ�.
unsigned char* pField = nullptr;

int Rotate(int px, int py, int r)//x��,y��,��ת����
{
	int pi = 0;
	switch (r % 4)
	{
	case 0: // 0��			        // 0  1  2  3
		pi = py * 4 + px;			// 4  5  6  7
		break;						// 8  9 10 11
		                            //12 13 14 15

	case 1: // 90��			        //12  8  4  0
		pi = 12 + py - (px * 4);	//13  9  5  1
		break;						//14 10  6  2
		                            //15 11  7  3

	case 2: // 180��			        //15 14 13 12
		pi = 15 - (py * 4) - px;	//11 10  9  8
		break;						// 7  6  5  4
		                            // 3  2  1  0

	case 3: // 270��			        // 3  7 11 15
		pi = 3 - py + (px * 4);		// 2  6 10 14
		break;						// 1  5  9 13
	}								// 0  4  8 12

	return pi;
}

//��鷽���Ƿ��ʺϷ���ָ��λ��
//nTetromino���������ͣ�nRotation��������ת״̬��nPosX����Ҫ���÷���λ�õ�X���꣬nPosY����Ҫ���÷���λ�õ�Y����
bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
{
	// All Field cells >0 are occupied
	for (int px = 0; px < 4; px++)
		for (int py = 0; py < 4; py++)
		{
			// ��ȡ�����ڲ�λ�õ�����
			int pi = Rotate(px, py, nRotation);

			// ��ȡ��������Ϸ�����е�����
			int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

			// Check that test is in bounds. Note out of bounds does
			// not necessarily mean a fail, as the long vertical piece
			// can have cells that lie outside the boundary, so we'll
			// just ignore them
			if (nPosX + px >= 0 && nPosX + px < nFieldWidth)//��鷽���Ƿ��ں���Χ��
			{
				if (nPosY + py >= 0 && nPosY + py < nFieldHeight)//��鷽���Ƿ�������Χ��
				{
					if (tetromino[nTetromino][pi] != L'.' && pField[fi] != 0)//��ײ���
						return false; //��һ����ײʱ�ͷ���FALSE
				}
			}
		}

	return true;
}

int main()
{
	//������Ļ������
	
	//wchar_t ��һ�����ڱ�ʾ���ַ����������ͣ�ͨ����Windows������֧��Unicode�ַ���

	//���ȣ�ʹ�� new ������Ϊ��Ļ�����������ڴ棬��СΪ nScreenWidth* nScreenHeight �����ַ�(wchar_t)

	//Ȼ��ͨ��ѭ����������screen�е�ÿ��Ԫ�ض�����Ϊ���ַ��ո�L' '������ʾ��Ļ��������ʼ��Ϊ�հ�

	//������������CreateConsoleScreenBuffer��������һ���µĿ���̨��Ļ����������������������hConsole�����С��ú��������е� GENERIC_READ | GENERIC_WRITE ��ʾ�û������ɹ���ȡ��д��

	//4,SetConsoleActiveScreenBuffer ��������ǰ��Ŀ���̨��Ļ����������Ϊ�ոմ����Ļ��������⽫ʹ�����ǿ����ڿ���̨����ʾ�������е�����

	//5,������һ����Ϊ dwBytesWritten �� DWORD �������ڼ�¼д�뵽����̨��Ļ���������ֽ���.DWORDʵ������unsigned long�ı������������ڶ���һ��32λ�޷�����������

	//Ŀ��:����һ�����пո��ַ���ʼ������Ļ������������������Ϊ�����Ļ���������Ա�������Խ��ַ����������̨��Ļ��

	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	for (int i = 0; i < nScreenWidth * nScreenHeight; ++i) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	tetromino[0].append(L"..X...X...X...X."); //7��4x4����,��β׷���ַ�
	tetromino[1].append(L"..X..XX...X.....");
	tetromino[2].append(L".....XX..XX.....");
	tetromino[3].append(L"..X..XX..X......");
	tetromino[4].append(L".X...XX...X.....");
	tetromino[5].append(L".X...X...XX.....");
	tetromino[6].append(L"..X...X..XX.....");

	//��̬�����ڴ�,ͨ��pField���Է��ʵ���СΪnFieldWidth * nFieldHeight���ڴ�
	pField = new unsigned char[nFieldWidth * nFieldHeight];
	for (int x = 0; x < nFieldWidth; x++)
		for (int y = 0; y < nFieldHeight; y++)
			//�����߽磬�����y * ��� + x����ʾ���� i ��pField[i] = ...��ʾ������Ǳ߽磬��ֵ9���ڲ�����ֵ0
	        //x == 0����߽�      x == nFieldWidth - 1���ұ߽�        y == nFieldHeight - 1���±߽�
	        //��Ϊ������Ϸ����֣����Բ���Ҫ�ϱ߽�
			pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;//9����#,0����ո�
	

	//��Ϸ�߼�
	bool bKey[4];
	int nCurrentPiece = 0;
	int nCurrentRotation = 0;
	int nCurrentX = nFieldWidth / 3;//��ʼˮƽλ��Ϊ��Ϸ��������
	int nCurrentY = 0;
	int nSpeed = 20;//��ʼ�����ٶȱ���
	int nSpeedCount = 0;//�����������ڼ�¼���������֡��
	bool bForceDown = false;//��Ƿ����Ƿ�ǿ�������ƶ�
	bool bRotateHold = true;//��¼����Ƿ�ס��Z��
	int nPieceCount = 0;//�Ѷ�����
	int nScore = 0;//��ʼ����
	vector<int> vLines;//�洢����ʱ��=,�����Ժ�����
	bool bGameOver = false;

	while (!bGameOver) //��ѭ��
	{
		srand((unsigned int)time(NULL));
		// ��ʱ    =======================
		/*this_thread::sleep_for(50ms) ��һ��C++�е��̲߳���������ʹ��ǰ�߳���ִͣ��һ��ʱ��
		this_thread ��C++��׼���е�һ�������ռ䣬�ṩ�����߳���صĺ�������
		sleep_for ��this_thread�����ռ��е�һ������������ʹ��ǰ�߳���ִͣ��ָ����ʱ���*/
		this_thread::sleep_for(50ms); //���ڿ�����Ϸ֡��
		nSpeedCount++;//�������Լ�1
		bForceDown = (nSpeedCount == nSpeed);//����20ms(��ʼ�Ѷ�)*50��1��(1֡)��,��������һ��

		// ����    ========================
		for (int k = 0; k < 4; k++)								// R   L   D Z
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;//����ȡ�İ���״̬�洢������bKey�еĲ���
		//0x8000 ��һ��ʮ�����������������������Ŀ���Ǽ�鰴��״̬�е����λ�Ƿ���λ
		//GetAsyncKeyState()��һ�� Windows API ���������ڼ��ָ����������Ӧ�İ���״̬��������һ����������״̬��Ϣ��ֵ
		
		//0x8000�������Ʊ�ʾΪ 1000000000000000
		/*�� GetAsyncKeyState() ���������Ի�ȡָ����������Ӧ�İ���״̬
		�ú����᷵��һ��ֵ�����λ��ʾ����״̬��1��ʾ���£�0��ʾδ����
		��ͨ����λ��& ����֪0x8000���λ��1��������������ˣ���ô& �Ľ����Ϊ1
		�� 1 �洢�� bKey[] ��*/


		// ��Ϸ�߼� ===================

		//���Ʒ��鲻����(��ײ���)
		//Ƕ��ѭ��������&&����Ŀ������Ż�
		nCurrentX += (bKey[0] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;
		nCurrentX -= (bKey[1] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;
		nCurrentY += (bKey[2] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0;

		//��ֹ��סZʱһֱ��ת
		if (bKey[3])
		{
			nCurrentRotation += (bRotateHold && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;
			bRotateHold = false;
		}
		else
			bRotateHold = true;

		//��bForceDownΪ�棬��ǿ�����Ʒ���
		if (bForceDown)
		{
			//ÿ50����������Ѷ�
			nSpeedCount = 0;
			nPieceCount++;
			if (nPieceCount % 50 == 0)
				if (nSpeed >= 10) nSpeed--;

			//ͨ��DoesPieceFit�����жϷ����Ƿ��ܼ��������ƶ�
			//���ƶ���Y�����Լ�1
			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
				nCurrentY++;
			//�����ƶ����������²���
			//����ǰ�������״��������Ϸ���������еĶ�Ӧλ��
			//����Ƿ����������б�����������ִ�������еĲ���
			//ѡ����һ������ĳ�ʼλ�ú���״
			//����µķ����޷�������ָ��λ�ã�����Ϸ״̬���Ϊ������bGameOverΪtrue��
			else
			{
				//���鲻�ܼ��������,������������ǰλ����
				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
						if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != L'.')
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;//��ʱ!=0����ռ����,���Ƿ�����ײ���

				//���ͬһ���Ƿ�����
				for (int py = 0; py < 4; py++)
					if (nCurrentY + py < nFieldHeight - 1)
					{
						bool bLine = true;
						for (int px = 1; px < nFieldWidth - 1; px++)//�ų����ұ߽�
							bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;//�κ�����0����0,����bLine����false

						if (bLine)
						{
							//����������Ϊ=
							for (int px = 1; px < nFieldWidth - 1; px++)//�ų����ұ߽�
								pField[(nCurrentY + py) * nFieldWidth + px] = 8;
							vLines.push_back(nCurrentY + py);//������=����vLine��,�Ա��������
						}
					}

				nScore += 25;
				if (!vLines.empty())	nScore += (1 << vLines.size()) * 100;//2��vLines.size()�η�*100

				//������һ������
				nCurrentX = nFieldWidth / 2;
				nCurrentY = 0;
				nCurrentRotation = 0;
				nCurrentPiece = rand() % 7;//�����Ҫ��srand(��ʵҲ��α���)

				//װ���¾�GAME OVER
				bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
			}
		}

		// ��ʾ ======================

		// ���Ƴ���
		for (int x = 0; x < nFieldWidth; x++)
			for (int y = 0; y < nFieldHeight; y++)
				//(y+2)��(x+2)��ƫ����,ʹ�����ܹ�������ʾ
				screen[(y+2) * nScreenWidth + x+2] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]];//9����#,0����ո�,����������ʹ�ø��ӵ�switch��ifǶ��

		// ���ƶ���˹����
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != L'.')
					screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;//+2����ƫ����,+65��ʾת������д

		//���Ʒ���
		//swprintf_s() ��һ����ʽ���ַ����ĺ��������ڽ���ʽ���������д��һ�� wide character �ַ�����
		
		//int swprintf_s(wchar_t* buffer, size_t sizeInWords, const wchar_t* format, ...)
		//buffer��ָ��Ŀ���ַ�����ָ�롣��ʽ��������ݽ���д�뵽����ַ�����
		//sizeInWords��Ŀ���ַ����Ĵ�С�����ֽ�Ϊ��λ��������д�������ַ������ڽ���д�����ʱ��Ҫȷ��Ŀ���ַ��������㹻�Ŀռ������ɸ�ʽ���������
		//format����ʽ���ַ���������ָ������ĸ�ʽ
		//...���ɱ������Ĳ��������ڸ��� format �еĸ�ʽָ��Ҫ�����ֵ
		swprintf_s(&screen[2 * nScreenWidth + nFieldWidth + 6], 16, L"SCORE: %8d", nScore);

		//һ����ʱ��������
		if (!vLines.empty())
		{
			// Display Frame (cheekily to draw lines)
			
			//WriteConsoleOutputCharacterW ��һ�� Windows API ���������ڽ��ַ�д�����̨�������������
			//	�����������£�
			//	hConsole������̨����������ʾҪд���Ŀ�����̨���ڡ�
			//	screen��ָ�����Ҫд����ַ����ݵ��ַ������ָ�롣����δ����У�����ָ�� screen �����ָ�롣
			//	nScreenWidth* nScreenHeight��Ҫд����ַ���Ŀ������Ļ��ȳ�����Ļ�߶ȡ���ʾ����������Ĵ�С��
			//{ 0,0 }��һ������ָ��д�������ʼλ�õ������ COORD �ṹ�塣�����{ 0,0 } ��ʾ��������������Ͻǿ�ʼд�롣
			//	& dwBytesWritten��һ��ָ�� DWORD ���ͱ�����ָ�룬���ڽ���ʵ��д����ַ�������
			//	������������δ���������ǽ� screen �����е��ַ�����д�뵽����̨������������У�����ʾ�ڿ���̨�����ϡ�
			WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
			this_thread::sleep_for(400ms); //400ms�ӳٺ�,=����ʧ

			//for (auto& v : vLines)�ȼ���
			//for (auto it = vLines.begin(); it != vLines.end(); ++it) {
			//auto& v = *it;}
			for (auto& v : vLines)
				for (int px = 1; px < nFieldWidth - 1; px++)
				{
					for (int py = v; py > 0; py--)
						pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];//������ɺ������������һ��
					pField[px] = 0;
				}

			vLines.clear();//����=��
		}

		//��ʾ���
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}

	CloseHandle(hConsole);//���ڹرվ��
	cout << "GAME OVER!! Score:" << nScore << endl;
	system("pause");
	return 0;
}