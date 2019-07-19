#include <windows.h>
#include <fstream>
#include <iomanip>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <process.h>
#include <winsock.h>
#include <tchar.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

/*-------------------------------------------------- �궨�� -------------------------------------------------- */
#define WindWidth 800
#define WindHighth 600

//����״̬����
#define ProConnect 0
#define ProLoad 1
#define ProCreate 2
#define ProMain 3
#define ProSure 100
#define ProPersonInformation 4
#define ProAddPerson 5
#define ProAddUnit 6
#define ProDeleteSure 7
#define ProSelect 8
#define ProSelectPersonInformation 9

struct person
{
	char name[100];		//����
	char nick[100];		//���
	char sex[100];		//�Ա�
	char birth[100];	//����
	char unit[26];		//��λ
	char habit[100];	//����	
	char qq[100];		//QQ	
	char phone[100];	//�绰
	char tips[200];		//��ע
	int relation;		//��ϵ
	int match;			//�Ƿ�ƥ����
};

/*-------------------------------------------------- ȫ�ֱ��� -------------------------------------------------- */
static HINSTANCE hIns;	//����ʵ�����
static HWND hWnd;		//���ڲ���

static HDC hdc ;			//�豸�������
static PAINTSTRUCT ps ;		//���ƽṹ
static POINT pt ;			//���λ��

static HDC hh;					
static HBITMAP bitMap;			//���ڼ���ͼƬ

static LOGFONT logFont;
static HFONT hFont;				//����

static HPEN hPen;			//����
static HBRUSH hBrush;		//��ˢ

int proState = ProConnect;
int lastState = ProConnect;
int nextState = ProConnect;

int hStateConnect = 0;	//�Ƿ���Կ��������߳�
int mainShow = 0;		//�������Ƿ��Ѿ���ʾ����

char serverIP[100] = "127.0.0.1";		//������IP
int port = 60001;

TCHAR g_buffT[100];
TCHAR g_buffT1[100];
TCHAR g_buffT2[100];

//-------------------- �б�����
char stage = 0;					//�׶� 0 - 4
char unit = 0;					//��λ 0 - 4
char unitCount = 0;		//��ǰ�м�����λ
char unitName[5][100];	//��ǰ��λ����

char matchOrNot = 1;			// 0 Ϊȡ��ƥ�䣬 1 Ϊ����ƥ��
struct person listPerson[100];	//��ǰ�����е���������
char listPersonCount = 0;

char nowPersonNumber;			//��ǰѡ�еı�� 0 - 71
struct person nowPerson;
char nowUnit[100];					//��ǰ��������
char unitDeletePersonCount = 0;		//ɾ����ǰ����Ĺ���ɾ������

char selectInformation[100];	//��ѯ��Ϣ


/*-------------------------------------------------- ���ܺ��� -------------------------------------------------- */
//һ�ֽ��ַ���ת���ֽ��ַ���
void charToTchar(char * buff, TCHAR * buffT)  
{
	int iLength;  
	iLength = MultiByteToWideChar(CP_ACP, 0, buff, strlen(buff)+1, NULL, 0);  
	MultiByteToWideChar(CP_ACP, 0, buff, strlen(buff)+1, buffT, iLength);  
}

//���ֽ��ַ���תһ�ֽ��ַ���
void tcharToChar(TCHAR * buffT, char * buff)
{   
	int iLength;
	iLength = WideCharToMultiByte(CP_ACP, 0, buffT, wcslen(buffT), NULL, 0, NULL, NULL);     
	WideCharToMultiByte(CP_ACP, 0, buffT, wcslen(buffT), buff, iLength, NULL, NULL);   
	buff[iLength] = '\0';   
}	

/*-------------------------------------------------- ��ͼ���� -------------------------------------------------- */
//����ǰ�׶Ρ���λ���Ƿ�ƥ��
void drawNowStateUnitMatch()
{
	int i;
	int x, y;
	TCHAR buffT[100];
	
	//�ı��������Ϊ����    
	ZeroMemory(&logFont, sizeof(LOGFONT));   
	logFont.lfCharSet = GB2312_CHARSET;   
	hFont = CreateFontIndirect(&logFont);   
	SelectObject(hdc, hFont); 

	SetBkMode(hdc, TRANSPARENT);
	SetTextAlign(hdc, TA_CENTER);	
	
	//д��λ����
	x = 109;
	y = 41;
	for(i = 0; i < unitCount; i++)
	{
		memset(buffT, 0, 200);
		charToTchar(unitName[i], buffT);
		TextOut(hdc, x+i*98, y, buffT, wcslen(buffT));	
	}
	
	//���׶�
	if(stage < 5)
	{
		x = 672;
		y = 98;
		y += 48*stage;
		hPen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
		SelectObject(hdc,hPen);
		hBrush = CreateSolidBrush(RGB(0, 0, 0));  
		SelectObject(hdc, hBrush);
		Ellipse(hdc, x-3, y-3, x+3, y+3);
	}
	
	//����λ
	if(unit < 5)
	{
		x = 65;
		y = 32;
		x += 98*unit;
		MoveToEx(hdc, x, y, NULL);
		LineTo(hdc, x+90, y);
		y++;
		MoveToEx(hdc, x-1, y, NULL);
		LineTo(hdc, x+91, y);
	}
	
	//��ƥ��
	if(matchOrNot == 1)
	{
		x = 695;
		y = 343;
		hPen = CreatePen(PS_SOLID, 1, RGB(154,236,165));
		SelectObject(hdc,hPen);
		hBrush = CreateSolidBrush(RGB(128,222,143));  
		SelectObject(hdc, hBrush);
		Ellipse(hdc, x-15, y-15, x+15, y+15);
		x = 710;
		y = 353;
		hPen = CreatePen(PS_SOLID, 1, RGB(255,178,178));
		SelectObject(hdc,hPen);
		hBrush = CreateSolidBrush(RGB(255,129,129));  
		SelectObject(hdc, hBrush);
		Ellipse(hdc, x-15, y-15, x+15, y+15);
	}
}

//��ʾ��������е�����
void drawPersons()
{
	int i, j, k;
	int x, y;

	//�ı��������Ϊ����      
	ZeroMemory(&logFont, sizeof(LOGFONT));   
	logFont.lfCharSet = GB2312_CHARSET;   
	hFont = CreateFontIndirect(&logFont);   
	SelectObject(hdc, hFont); 
	
	SetBkMode(hdc, TRANSPARENT);
	SetTextAlign(hdc, TA_LEFT);	
	TCHAR buffT[100];
	
	for(i = 0; i < listPersonCount; i++)
	{
		x = 95;
		y = 90;
		j = i/6;
		k = i%6;
		y += j*25;
		x += k*90;
		memset(buffT, 0, 200);
		charToTchar(listPerson[i].name, buffT);
		TextOut(hdc, x, y, buffT, wcslen(buffT));	
		if(listPerson[i].match == 1 && matchOrNot == 1)
		{
			if(strcmp(listPerson[i].sex, "��") == 0)
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(128,222,143));
			}
			else 
			{
				hPen = CreatePen(PS_SOLID, 1, RGB(255,129,129));
			}
			SelectObject(hdc,hPen); 
			MoveToEx(hdc, x, y+18, NULL);
			LineTo(hdc, x+8*strlen(listPerson[i].name), y+18);
			
		}
	}
}

//��ʾ��ѯ������е�����
void drawSelectPersons()
{
	int i, j, k;
	int x, y;

	//�ı��������Ϊ����    
	ZeroMemory(&logFont, sizeof(LOGFONT));   
	logFont.lfCharSet = GB2312_CHARSET;   
	hFont = CreateFontIndirect(&logFont);   
	SelectObject(hdc, hFont); 

	SetBkMode(hdc, TRANSPARENT);
	SetTextAlign(hdc, TA_LEFT);	
	TCHAR buffT[100];
	
	for(i = 0; i < listPersonCount && i < 45; i++)
	{
		x = 175;
		y = 80;
		j = i/5;
		k = i%5;
		y += j*25;
		x += k*90;
		memset(buffT, 0, 200);
		charToTchar(listPerson[i].name, buffT);
		TextOut(hdc, x, y, buffT, wcslen(buffT));	
	}
}

/*-------------------------------------------------- ���纯�� -------------------------------------------------- */
WSAData WSAData;
struct sockaddr_in sockAddr;
int socketClient;

//��������
unsigned int __stdcall Connection(void *)
{
	if(WSAStartup(MAKEWORD(1,1),&WSAData))
	{
		MessageBox(NULL, TEXT("�����ʼ��ʧ��!"), TEXT("ERROR"), NULL);
		WSACleanup();
		return 0;
	}

	//����ͨ�ŵ�ַ
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;							
	sockAddr.sin_addr.s_addr = inet_addr(serverIP);		
	sockAddr.sin_port = htons(port);						//�˿ں�
	

	//����һ��socket
	socketClient = socket(AF_INET, SOCK_STREAM, 0);
	int sockGet = -1;

	//ѭ������
	while(sockGet < 0)
	{
		sockGet = connect(socketClient,(struct sockaddr *)&sockAddr,sizeof(sockAddr));
		Sleep(500);
	}
	//MessageBox(NULL, TEXT("�������Ѿ�����!"), TEXT("Welcome"), NULL);
	proState = ProLoad;
	InvalidateRect(hWnd, NULL, TRUE);
	return 0;
}	

/*------------------------------ // ����Լ�� // ------------------------------
1���ͻ������ȴ���һ���ַ�
	0�������û���
	1�������û���������
	z����ֹ����

2��֮����д��ַ�Լ���Ĵ���
3���ص�����1
------------------------------------------------------------*/

int sendAll(char s)
{
	int i;
	char recvBuff[100];
	char sendBuff[100];
	char sendBuff1[100];
	char sendBuff2[100];
	TCHAR buffT[100];
	memset(recvBuff, 0 ,100);
	memset(sendBuff, 0 ,100);
	memset(sendBuff1, 0 ,100);
	memset(sendBuff2, 0 ,100);
	
	sendBuff[0] = s;
	send(socketClient, sendBuff, 1, 0);
	switch(s)
	{
		//------------------------------ �����û�
		case '0':
		{
			tcharToChar(g_buffT, sendBuff);
			tcharToChar(g_buffT1, sendBuff1);
			send(socketClient, sendBuff, 80, 0);
			send(socketClient, sendBuff1, 80, 0);
			recv(socketClient, recvBuff, 1, 0);
			if(recvBuff[0] == '1') return 1;
			else return 0;
			
			break;
		}
		//------------------------------ �û���¼
		case '1':
		{
			tcharToChar(g_buffT, sendBuff);
			tcharToChar(g_buffT1, sendBuff1);
			send(socketClient, sendBuff, 80, 0);
			send(socketClient, sendBuff1, 80, 0);
			recv(socketClient, recvBuff, 1, 0);
			if(recvBuff[0] == '1') return 1;
			else if(recvBuff[0] == '2') return 2;
			else return 0;
			break;
		}

		//------------------------------ ��������������
		case 'a':
		{
			sendBuff[0] = matchOrNot;
			send(socketClient, sendBuff, 1, 0);		//�����Ƿ�ƥ��
			sendBuff[0] = stage;
			send(socketClient, sendBuff, 1, 0);		//���͵�ǰ�׶�
			sendBuff[0] = unit;
			send(socketClient, sendBuff, 1, 0);		//���͵�ǰ��λ
			
			recv(socketClient, recvBuff, 1, 0);		//���ܵ�ǰ�׶��м�����λ
			unitCount = recvBuff[0];
			
			//���յ�ǰ�׶ε�λ����
			recv(socketClient, unitName[0], 100, 0);
			recv(socketClient, unitName[1], 100, 0);
			recv(socketClient, unitName[2], 100, 0);
			recv(socketClient, unitName[3], 100, 0);
			recv(socketClient, unitName[4], 100, 0);
			
			recv(socketClient, recvBuff, 1, 0);		//���յ�ǰ��λ�м�����Ա
			listPersonCount = recvBuff[0];
			
			//���յ�ǰ��λ��Ա����
			for(i = 0; i < listPersonCount; i++)
			{
				recv(socketClient, listPerson[i].name, 100, 0);		//����
				recv(socketClient, listPerson[i].nick, 100, 0);		//���
				recv(socketClient, listPerson[i].sex, 100, 0);		//�Ա�				
				recv(socketClient, listPerson[i].birth, 100, 0);	//����	
				recv(socketClient, listPerson[i].unit, 26, 0);		//����			
				recv(socketClient, listPerson[i].habit, 100, 0);	//����				
				memset(recvBuff, 0 ,100);
				recv(socketClient, recvBuff, 4, 0);					//��ϵ
				sscanf(recvBuff, "%d", &listPerson[i].relation);				
				recv(socketClient, listPerson[i].qq, 100, 0);		//QQ				
				recv(socketClient, listPerson[i].phone, 100, 0);	//�绰					
				recv(socketClient, listPerson[i].tips, 200, 0);		//��ע
				
				if(matchOrNot == 1)
				{
					recv(socketClient, recvBuff, 1, 0);					//�Ƿ�ƥ����
					listPerson[i].match = recvBuff[0];
				}
			}
			break;
		}
		
		//------------------------------ �޸�ĳ������
		case 'b':
		{
			send(socketClient, nowPerson.name, 50, 0);
			send(socketClient, nowPerson.nick, 50, 0);
			send(socketClient, nowPerson.sex, 50, 0);
			send(socketClient, nowPerson.birth, 50, 0);
			send(socketClient, nowPerson.habit, 100, 0);	
			send(socketClient, nowPerson.qq, 50, 0);
			send(socketClient, nowPerson.phone, 50, 0);
			send(socketClient, nowPerson.tips, 200, 0);
			memset(sendBuff1, 0, 100);
			sendBuff1[0] = nowPerson.relation+10;
			send(socketClient, sendBuff1, 1, 0);
			
			
			break;
		}
		//------------------------------ ɾ��ĳ����ĳ��λ������
		case 'c':
		{
			send(socketClient, nowPerson.name, 50, 0);
			memset(sendBuff, 0, 100);	
			sendBuff[0] = stage;
			send(socketClient, sendBuff, 1, 0);	
			memset(sendBuff, 0, 100);	
			sendBuff[0] = unit;
			send(socketClient, sendBuff, 1, 0);	
			
			memset(buffT, 0, 100);
			charToTchar(nowPerson.unit, buffT);
			send(socketClient, nowPerson.unit, 26, 0);	
		
			break;
		}
		
		//------------------------------ ��ӵ�λ�е�ĳ��
		case 'd':
		{
			send(socketClient, nowPerson.name, 50, 0);
			send(socketClient, nowPerson.nick, 50, 0);
			send(socketClient, nowPerson.sex, 50, 0);
			send(socketClient, nowPerson.birth, 50, 0);
			send(socketClient, nowPerson.habit, 100, 0);	
			send(socketClient, nowPerson.qq, 50, 0);
			send(socketClient, nowPerson.phone, 50, 0);
			send(socketClient, nowPerson.tips, 200, 0);
			memset(sendBuff1, 0, 100);
			sendBuff1[0] = nowPerson.relation+10;
			send(socketClient, sendBuff1, 1, 0);
			
			memset(recvBuff, 0, 100);
			recv(socketClient, recvBuff, 1, 0);
			if(recvBuff[0] == '1') MessageBox(NULL, TEXT("��⵽��ͬ��������������Ϣ�Ѽ��뱾����"), TEXT("��ʾ"), NULL);
			
			break;
		}
		//------------------------------ ��ӷ���
		case 'e':
		{
			send(socketClient, nowUnit, 80, 0);
			break;
		}
		//------------------------------ ɾ������ǰ��ȷ��
		case 'f':
		{
			memset(recvBuff, 0, 100);
			recv(socketClient, recvBuff, 1, 0);
			unitDeletePersonCount = recvBuff[0];
			
			break;
		}
		//------------------------------ ɾ������
		case 'g':
		{
			break;
		}
		
		//------------------------------ ����
		case 'h':
		case 'i':
		case 'j':
		{
			send(socketClient, selectInformation, 50, 0);
			
			recv(socketClient, recvBuff, 1, 0);		//�����м�����Ա
			listPersonCount = recvBuff[0];

			//������Ա����
			for(i = 0; i < listPersonCount; i++)
			{
				recv(socketClient, listPerson[i].name, 100, 0);		//����
				recv(socketClient, listPerson[i].nick, 100, 0);		//���
				recv(socketClient, listPerson[i].sex, 100, 0);		//�Ա�				
				recv(socketClient, listPerson[i].birth, 100, 0);	//����				
				recv(socketClient, listPerson[i].habit, 100, 0);	//����				
				memset(recvBuff, 0 ,100);
				recv(socketClient, recvBuff, 4, 0);					//��ϵ
				sscanf(recvBuff, "%d", &listPerson[i].relation);				
				recv(socketClient, listPerson[i].qq, 100, 0);		//QQ				
				recv(socketClient, listPerson[i].phone, 100, 0);	//�绰					
				recv(socketClient, listPerson[i].tips, 200, 0);		//��ע
			}

			break;
		}
		
		//------------------------------ �Ͽ�����
		case 'z':
		{
			hStateConnect = 0;
			proState = ProConnect;
			break;
		}
	}	
	return 0;
}

void DeleteAllGDI()
{
	/*
	static HINSTANCE hIns;	//����ʵ�����
	static HWND hWnd;		//���ڲ���

	static HDC hdc ;			//�豸�������
	static PAINTSTRUCT ps ;		//���ƽṹ
	static POINT pt ;			//���λ��

	static HDC hh;					
	static HBITMAP bitMap;			//���ڼ���ͼƬ

	static LOGFONT logFont;
	static HFONT hFont;				//����

	static HPEN hPen;			//����
	static HBRUSH hBrush;		//��ˢ
	*/
	
	DeleteDC(hdc);
	DeleteDC(hh);
	DeleteObject(bitMap);
	//DeleteObject(hFont);
	DeleteObject(hPen);
	DeleteObject(hBrush);
}

/*-------------------------------------------------- �������� -------------------------------------------------- */
LRESULT CALLBACK WindProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int i = 0, j = 0 ,k = 0, l = 0, m = 0, n = 0, x = 0, y = 0;
	char c;
	char buff[200];
	char buff1[200];
	char buff2[200];
	TCHAR buffT[100];
	TCHAR buffT1[100];
	TCHAR buffT2[100];
	
	switch(proState)
	{
		//---------------------------------------- ȷ�Ͻ��� ----------------------------------------
		case ProSure:
		{
			switch(message)
			{
				case WM_PAINT:
				{
					hdc = BeginPaint(hwnd, &ps);
					bitMap = (HBITMAP)LoadImage(NULL, L"Face/ȷ�Ͻ���.bmp", IMAGE_BITMAP, WindWidth, WindHighth, LR_LOADFROMFILE);
					hh = CreateCompatibleDC(NULL);
					SelectObject(hh, bitMap);
					BitBlt(hdc, 0, 0, WindWidth, WindHighth, hh, 0, 0, SRCCOPY);
					
					DeleteAllGDI();
					EndPaint(hwnd, &ps);
				
					return 0;
				}
				case WM_LBUTTONUP:
				{	
					GetCursorPos(&pt);
					ScreenToClient(hwnd, &pt);   

					//---------------------------------------- ������ǡ�
					if(pt.x > 266 && pt.x < 364 && pt.y > 267 && pt.y < 325)
					{
						if(nextState == ProConnect)
						{
							sendAll('z');
							closesocket(socketClient);
							WSACleanup();
						}
						else if(lastState == ProPersonInformation && nextState == ProMain)
						{
							sendAll('c');		//ɾ��ĳ����ĳ��λ������
						}
						proState = nextState;
							
					}
					//---------------------------------------- �������
					else if(pt.x > 435 && pt.x < 533 && pt.y > 267 && pt.y < 325)
					{
						proState = lastState;
					}
					else return 0;
					
					InvalidateRect(hwnd, NULL, TRUE);
					return 0;
				}
				case WM_DESTROY:
				{
					sendAll('z');
					closesocket(socketClient);
					WSACleanup();
					PostQuitMessage(0);
					return 0;
				}
			}
			
			break;
		}
		
		//---------------------------------------- ����ɾ��ȷ�Ͻ��� ----------------------------------------
		case ProDeleteSure:
		{
			static int state = 0;
			if(state == 0)
			{
				sendAll('f');
				state = 1;
			}
			switch(message)
			{
				case WM_PAINT:
				{
					hdc = BeginPaint(hwnd, &ps);
					bitMap = (HBITMAP)LoadImage(NULL, L"Face/ɾ����λ����.bmp", IMAGE_BITMAP, WindWidth, WindHighth, LR_LOADFROMFILE);
					hh = CreateCompatibleDC(NULL);
					SelectObject(hh, bitMap);
					BitBlt(hdc, 0, 0, WindWidth, WindHighth, hh, 0, 0, SRCCOPY);
					
					memset(buff, 0 ,200);
					memset(buffT, 0, 200);
					buff[0] = unitDeletePersonCount + '0';
					charToTchar(buff, buffT);
					TextOut(hdc, 422, 132, buffT, wcslen(buffT));	
					
					DeleteAllGDI();
					EndPaint(hwnd, &ps);
				
					return 0;
				}
				case WM_LBUTTONUP:
				{	
					GetCursorPos(&pt);
					ScreenToClient(hwnd, &pt);   

					//---------------------------------------- ������ǡ�
					if(pt.x > 265 && pt.x < 364 && pt.y > 301 && pt.y < 359)
					{
						proState = ProMain;
						sendAll('g');			//ɾ������
						if(unit > 0) unit--;
						state = 0;
					}
					//---------------------------------------- �������
					else if(pt.x > 435 && pt.x < 534 && pt.y > 301 && pt.y < 359)
					{
						proState = ProMain;
						state = 0;
					}
					else return 0;
					
					InvalidateRect(hwnd, NULL, TRUE);
					return 0;
				}
				case WM_DESTROY:
				{
					sendAll('z');
					closesocket(socketClient);
					WSACleanup();
					PostQuitMessage(0);
					return 0;
				}
			}
			
			break;
		}
		
		//---------------------------------------- ���ӵ������� ----------------------------------------
		case ProConnect:
		{
			if(hStateConnect == 0)
			{
				hStateConnect = 1;
				_beginthreadex(NULL, 0, Connection, NULL, 0, NULL);		//���������߳�
				stage = 0;					
				unit = 0;				
				matchOrNot = 0;			
			}

			switch(message)
			{
				case WM_PAINT:
				{
					hdc = BeginPaint(hwnd, &ps);
					bitMap = (HBITMAP)LoadImage(NULL, L"Face/���ӽ���.bmp", IMAGE_BITMAP, WindWidth, WindHighth, LR_LOADFROMFILE);
					hh = CreateCompatibleDC(NULL);
					SelectObject(hh, bitMap);
					BitBlt(hdc, 0, 0, WindWidth, WindHighth, hh, 0, 0, SRCCOPY);
					
					DeleteAllGDI();
					EndPaint(hwnd, &ps);
				
					return 0;
				}
				case WM_DESTROY:
				{
					closesocket(socketClient);
					WSACleanup();
					PostQuitMessage(0);
					return 0;
				}
			}
			break;
		}	
		
		//---------------------------------------- ��¼ ----------------------------------------
		case ProLoad:
		{
			static HWND hText;
			static HWND hText1;
			static int textState = 0;
			
			//����һ�δ��������
			if(textState == 0)
			{
				textState = 1;
				hText = CreateWindow		//�û���
				( 
					TEXT("edit"), NULL,			
					WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER,
					350, 178, 160, 20,
					hwnd, NULL, hIns, NULL 
				);
				hText1 = CreateWindow		//����
				( 
					TEXT("edit"), NULL,			
					WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_PASSWORD,
					350, 225, 160, 20,
					hwnd, NULL, hIns, NULL 
				);		
				//�ı�����
				SendMessage(hText, WM_SETFONT, (WPARAM)hFont, TRUE);
				SendMessage(hText1, WM_SETFONT, (WPARAM)hFont, TRUE);
			}
			
			switch(message)
			{
				case WM_PAINT:
				{
					hdc = BeginPaint(hwnd, &ps);
					bitMap = (HBITMAP)LoadImage(NULL, L"Face/��¼����.bmp", IMAGE_BITMAP, WindWidth, WindHighth, LR_LOADFROMFILE);
					hh = CreateCompatibleDC(NULL);
					SelectObject(hh, bitMap);
					BitBlt(hdc, 0, 0, WindWidth, WindHighth, hh, 0, 0, SRCCOPY);
							
					DeleteAllGDI();
					EndPaint(hwnd, &ps);
					
					return 0;
				}
				case WM_LBUTTONUP:	
				{
					GetCursorPos(&pt);
					ScreenToClient(hwnd, &pt);   
					
					//---------------------------------------- �����ȷ�ϡ�
					if(pt.x > 359 && pt.x < 438 && pt.y > 348 && pt.y < 387)
					{
						GetWindowText(hText, g_buffT, 50);
						GetWindowText(hText1, g_buffT1, 50);
						i = sendAll('1');
						if(i == 0)			//��¼ʧ��
						{
							MessageBox(NULL, TEXT("�������! ��˶Ժ���������"), TEXT("Error"), NULL);
							return 0;
						}
						else				//�û���¼
						{
							textState = 0;
							proState = ProMain;
						}
					}
					//---------------------------------------- �����ע�ᡱ
					else if(pt.x > 440 && pt.x < 478 && pt.y > 104 && pt.y < 120)
					{
						
						textState = 0;
						proState = ProCreate;
					}
					else return 0;

					DestroyWindow(hText);
					DestroyWindow(hText1);
					InvalidateRect(hwnd, NULL, TRUE);
					return 0;
				}
				case WM_DESTROY:
				{
					sendAll('z');
					closesocket(socketClient);
					WSACleanup();
					PostQuitMessage(0);
					return 0;
				}
			}
		
			break;
		}

		//---------------------------------------- �����û� ----------------------------------------
		case ProCreate:
		{
			static HWND hText;
			static HWND hText1;
			static HWND hText2;
			static int textState = 0;
			
			//����һ�δ��������
			if(textState == 0)
			{
				textState = 1;
				hText = CreateWindow		//�û���
				( 
					TEXT("edit"), NULL,			
					WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER,
					350, 178, 160, 20,
					hwnd, NULL, hIns, NULL 
				);
				hText1 = CreateWindow		//����
				( 
					TEXT("edit"), NULL,			
					WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_PASSWORD,
					350, 225, 160, 20,
					hwnd, NULL, hIns, NULL 
				);
				hText2 = CreateWindow		//ȷ������
				( 
					TEXT("edit"), NULL,			
					WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER | ES_PASSWORD,
					350, 273, 160, 20,
					hwnd, NULL, hIns, NULL 
				);
				SendMessage(hText, WM_SETFONT, (WPARAM)hFont, TRUE);
				SendMessage(hText1, WM_SETFONT, (WPARAM)hFont, TRUE);
				SendMessage(hText2, WM_SETFONT, (WPARAM)hFont, TRUE);
			}
			
			switch(message)
			{
				case WM_PAINT:
				{
					hdc = BeginPaint(hwnd, &ps);
					bitMap = (HBITMAP)LoadImage(NULL, L"Face/ע�����.bmp", IMAGE_BITMAP, WindWidth, WindHighth, LR_LOADFROMFILE);
					hh = CreateCompatibleDC(NULL);
					SelectObject(hh, bitMap);
					BitBlt(hdc, 0, 0, WindWidth, WindHighth, hh, 0, 0, SRCCOPY);
				
					DeleteAllGDI();
					EndPaint(hwnd, &ps);
				
					return 0;
				}
				case WM_LBUTTONUP:	
				{
					GetCursorPos(&pt);
					ScreenToClient(hwnd, &pt);   
					
					//---------------------------------------- �����ȷ�ϡ�
					if(pt.x > 359 && pt.x < 438 && pt.y > 348 && pt.y < 387)
					{
						GetWindowText(hText, g_buffT, 50);
						GetWindowText(hText1, g_buffT1, 50);
						GetWindowText(hText2, g_buffT2, 50);
						
						tcharToChar(g_buffT, buff);
						tcharToChar(g_buffT1, buff1);
						tcharToChar(g_buffT2, buff2);	
						
						//�ͻ����жϵĴ���
						if(strlen(buff) <= 0)
						{
							MessageBox(NULL, TEXT("�û�������Ϊ��"), TEXT("��ʾ"), NULL);
							return 0;
						}
						if(strcmp(buff1, buff2))			
						{
							MessageBox(NULL, TEXT("�������벻��ͬ"), TEXT("��ʾ"), NULL);
							return 0;
						}
						if(strlen(buff1) <= 3)
						{
							MessageBox(NULL, TEXT("��������4���ַ�"), TEXT("��ʾ"), NULL);
							return 0;
						}
						
						i = sendAll('0');
						if(i == 0)			//��¼ʧ��
						{
							MessageBox(NULL, TEXT("�û��Ѵ���!"), TEXT("Error"), NULL);
							return 0;
						}
						else if(i == 1)		//�û������ɹ�
						{
							
							textState = 0;
							proState = ProMain;
						}
					}
					//---------------------------------------- �������¼��
					else if(pt.x > 440 && pt.x < 478 && pt.y > 104 && pt.y < 120)
					{
						textState = 0;
						proState = ProLoad;
					}
					else return 0;

					DestroyWindow(hText);
					DestroyWindow(hText1);
					DestroyWindow(hText2);
					InvalidateRect(hwnd, NULL, TRUE);
					return 0;
				}
				case WM_DESTROY:
				{
					sendAll('z');
					closesocket(socketClient);
					WSACleanup();
					PostQuitMessage(0);
					return 0;
				}
			}
			
			break;
		}
		
		//---------------------------------------- ��ѯ���� ----------------------------------------
		case ProSelect:
		{
			static int state = 0;
			static HWND hText;
			if(state == 0 || state == 2)
			{
				if(state == 0) listPersonCount = 0;
				state = 1;
				
				hText = CreateWindow		//��ѯ����
				( 
					TEXT("edit"), NULL,			
					WS_CHILD | WS_VISIBLE | ES_CENTER,
					358, 389, 85, 20,
					hwnd, NULL, hIns, NULL 
				);		
				SendMessage(hText, WM_SETFONT, (WPARAM)hFont, TRUE);
			}
			
			switch(message)
			{
				case WM_PAINT:
				{
					hdc = BeginPaint(hwnd, &ps);
					bitMap = (HBITMAP)LoadImage(NULL, L"Face/��ѯ����.bmp", IMAGE_BITMAP, WindWidth, WindHighth, LR_LOADFROMFILE);
					hh = CreateCompatibleDC(NULL);
					SelectObject(hh, bitMap);
					BitBlt(hdc, 0, 0, WindWidth, WindHighth, hh, 0, 0, SRCCOPY);
					
					drawSelectPersons();
					
					DeleteAllGDI();
					EndPaint(hwnd, &ps);
				
					return 0;
				}
				case WM_LBUTTONUP:
				{	
					
					GetCursorPos(&pt);
					ScreenToClient(hwnd, &pt);   

					//---------------------------------------- ��������ء�
					if(pt.x > 590 && pt.x < 670 && pt.y > 325 && pt.y < 360)
					{
						proState = ProMain;
						state = 0;
						DestroyWindow(hText);
					}
					
					//---------------------------------------- ����������֡�
					else if(pt.x > 261 && pt.x < 336 && pt.y > 446 && pt.y < 521)
					{
						memset(buffT, 0, 100);
						GetWindowText(hText, buffT, 100);
						if(wcslen(buffT) == 0)
						{
							MessageBox(NULL, TEXT("��ѯ���ݲ���Ϊ��"), TEXT("��ʾ"), NULL);
							return 0;
						}
						memset(selectInformation, 0, 100);
						tcharToChar(buffT, selectInformation);
						
						sendAll('h');		
					}
					
					//---------------------------------------- ������Ұ��á�
					else if(pt.x > 362 && pt.x < 437 && pt.y > 446 && pt.y < 521)
					{
						memset(buffT, 0, 100);
						GetWindowText(hText, buffT, 100);
						if(wcslen(buffT) == 0)
						{
							MessageBox(NULL, TEXT("��ѯ���ݲ���Ϊ��"), TEXT("��ʾ"), NULL);
							return 0;
						}
						memset(selectInformation, 0, 100);
						tcharToChar(buffT, selectInformation);

						sendAll('i');		
					}
					
					//---------------------------------------- ����������ѡ�
					else if(pt.x > 465 && pt.x < 540 && pt.y > 446 && pt.y < 521)
					{
						memset(buffT, 0, 100);
						GetWindowText(hText, buffT, 100);
						if(wcslen(buffT) == 0)
						{
							MessageBox(NULL, TEXT("��ѯ���ݲ���Ϊ��"), TEXT("��ʾ"), NULL);
							return 0;
						}
						memset(selectInformation, 0, 100);
						tcharToChar(buffT, selectInformation);

						sendAll('j');		
					}
					
					//---------------------------------------- �������Ա��
					else if(pt.x > 175 && pt.x < 625 && pt.y > 80 && pt .y < 305)
					{
						j = pt.y-80;
						k = pt.x-175;
						j = j/25;
						k = k/90;
						i = j*5+k;
						if(i >= listPersonCount) return 0;	//����հ�����
						
						nowPerson = listPerson[i];
						nowPersonNumber = i;
						
						proState = ProSelectPersonInformation;
						state = 2;
						DestroyWindow(hText);
					}
					else return 0;
					
					InvalidateRect(hwnd, NULL, TRUE);
					return 0;
					
				}
				case WM_DESTROY:
				{
					sendAll('z');
					closesocket(socketClient);
					WSACleanup();
					PostQuitMessage(0);
					return 0;
				}
			}
			break;
		}
		
		//---------------------------------------- ������ ----------------------------------------
		case ProMain:
		{
			if(mainShow == 0)	
			{
				mainShow = 1;
				sendAll('a');
			}
			switch(message)
			{
				case WM_PAINT:
				{
					hdc = BeginPaint(hwnd, &ps);
					bitMap = (HBITMAP)LoadImage(NULL, L"Face/������.bmp", IMAGE_BITMAP, WindWidth, WindHighth, LR_LOADFROMFILE);
					hh = CreateCompatibleDC(NULL);
					SelectObject(hh, bitMap);
					BitBlt(hdc, 0, 0, WindWidth, WindHighth, hh, 0, 0, SRCCOPY);
					
					drawNowStateUnitMatch();	
					drawPersons();
					
					DeleteAllGDI();
					EndPaint(hwnd, &ps);
				
					return 0;
				}
				case WM_LBUTTONUP:
				{	
					
					GetCursorPos(&pt);
					ScreenToClient(hwnd, &pt);   

					//---------------------------------------- ������˳���
					if(pt.x > 645 && pt.x < 720 && pt.y > 453 && pt.y < 528)
					{
						lastState = ProMain;
						nextState = ProConnect;
						proState = ProSure;
						mainShow = 0;
					}
					//---------------------------------------- �����ƥ�䡱
					else if(pt.x > 387 && pt.x < 462 && pt.y > 453 && pt.y < 528)
					{
						matchOrNot = !matchOrNot;
						sendAll('a');				//��ȡ�б�����������
					}
					//---------------------------------------- �������ѯ��
					else if(pt.x > 106 && pt.x < 367 && pt.y > 453 && pt.y < 528)
					{
						proState = ProSelect;
						mainShow = 0;
					}
					
					//---------------------------------------- ������׶Ρ�
					else if(pt.x > 661 && pt.x < 739 && pt.y > 72 && pt.y < 312)
					{
						if(pt.y < 122) stage = 0;						
						else if(pt.y < 170) stage = 1;	
						else if(pt.y < 217) stage = 2;
						else if(pt.y < 265) stage = 3;
						else stage = 4;
						unit = 0;
						sendAll('a');				//��ȡ�б�����������
					}
					//---------------------------------------- �������λ��
					else if(pt.x > 60 && pt.x < 550 && pt.y > 35 && pt.y < 63)
					{
						if(pt.x < 158 && unitCount >= 1) unit = 0;						
						else if(pt.x < 256 && unitCount >= 2) unit = 1;	
						else if(pt.x < 354 && unitCount >= 3) unit = 2;
						else if(pt.x < 452 && unitCount >= 4) unit = 3;
						else if(unitCount >= 5) unit = 4;
						else return 0;
						sendAll('a');				//��ȡ�б�����������
					}
					//---------------------------------------- �������Ա��
					else if(pt.x > 95 && pt.x < 635 && pt.y > 90 && pt .y < 390)
					{
						j = pt.y-90;
						k = pt.x-95;
						j = j/25;
						k = k/90;
						i = j*6+k;
						if(i >= listPersonCount) return 0;	//����հ�����
						
						nowPerson = listPerson[i];
						nowPersonNumber = i;
						
						proState = ProPersonInformation;
						mainShow = 0;
					}
					//---------------------------------------- ����������Ա��
					else if(pt.x > 680 && pt.x < 732 && pt.y > 387 && pt .y < 407)
					{
						if(unitCount > 0 && listPersonCount < 72)
						{
							proState = ProAddPerson;
							mainShow = 0;
						}
						else MessageBox(NULL, TEXT("��ǰ�޷����"), TEXT("��ʾ"), NULL);
					}
					//---------------------------------------- �������ӷ��顱
					else if(pt.x > 550 && pt.x < 579 && pt.y > 35 && pt .y < 63)
					{
						if(unitCount < 5)
						{
							proState = ProAddUnit;
							mainShow = 0;
						}
						else MessageBox(NULL, TEXT("��������"), TEXT("��ʾ"), NULL);
					}
					//---------------------------------------- �����ɾ�����顱
					else if(pt.x > 579 && pt.x < 608 && pt.y > 35 && pt .y < 63)
					{
						if(unitCount > 0)
						{
							proState = ProDeleteSure;
							mainShow = 0;
						}
						else MessageBox(NULL, TEXT("��ǰ�׶�û�з���"), TEXT("��ʾ"), NULL);
					}	
					else return 0;
					
					InvalidateRect(hwnd, NULL, TRUE);
					return 0;
					
				}
				case WM_DESTROY:
				{
					sendAll('z');
					closesocket(socketClient);
					WSACleanup();
					PostQuitMessage(0);
					return 0;
				}
			}
			break;
		}
		
		//---------------------------------------- ��ѯ��Ϣ���� ----------------------------------------
		case ProSelectPersonInformation:
		{
			static HWND hTextName, hTextNick, hTextSex, hTextBirth, hTextPhone, hTextQq, hTextHabit, hTextTips, hTextRelation;
			
			switch(message)
			{
				case WM_PAINT:
				{
					hdc = BeginPaint(hwnd, &ps);
					bitMap = (HBITMAP)LoadImage(NULL, L"Face/��ѯ��Ϣ����.bmp", IMAGE_BITMAP, WindWidth, WindHighth, LR_LOADFROMFILE);
					hh = CreateCompatibleDC(NULL);
					SelectObject(hh, bitMap);
					BitBlt(hdc, 0, 0, WindWidth, WindHighth, hh, 0, 0, SRCCOPY);
					
					//�ı��������Ϊ����      
					ZeroMemory(&logFont, sizeof(LOGFONT));   
					logFont.lfCharSet = GB2312_CHARSET;   
					hFont = CreateFontIndirect(&logFont);   
					SelectObject(hdc, hFont); 
					
					memset(buffT, 0 ,200);					
					charToTchar(nowPerson.name, buffT);
					TextOut(hdc, 220, 113, buffT, wcslen(buffT));
					
					memset(buffT, 0 ,200);
					charToTchar(nowPerson.nick, buffT);
					TextOut(hdc, 220, 157, buffT, wcslen(buffT));
					
					memset(buffT, 0 ,200);
					charToTchar(nowPerson.sex, buffT);
					TextOut(hdc, 220, 199, buffT, wcslen(buffT));
					
					memset(buffT, 0 ,200);
					charToTchar(nowPerson.birth, buffT);
					TextOut(hdc, 392, 113, buffT, wcslen(buffT));
					
					memset(buffT, 0 ,200);
					charToTchar(nowPerson.phone, buffT);
					TextOut(hdc, 572, 113, buffT, wcslen(buffT));
				
					memset(buffT, 0 ,200);
					charToTchar(nowPerson.qq, buffT);
					TextOut(hdc, 572, 157, buffT, wcslen(buffT));
			
					memset(buffT, 0 ,200);
					charToTchar(nowPerson.habit, buffT);
					TextOut(hdc, 160, 274, buffT, wcslen(buffT));
					
					memset(buff, 0, 100);
					strncpy(buff, nowPerson.tips, 26);
					memset(buffT, 0 ,200);
					charToTchar(buff, buffT);
					TextOut(hdc, 420, 274, buffT, wcslen(buffT));
					memset(buff, 0, 100);
					char *p = nowPerson.tips;
					p += 26;
					strncpy(buff, p, 26);
					memset(buffT, 0 ,200);
					charToTchar(buff, buffT);
					TextOut(hdc, 420, 290, buffT, wcslen(buffT));
					
					memset(buff, 0 ,200);
					memset(buffT, 0 ,200);
					sprintf(buff, "%d", nowPerson.relation);
					charToTchar(buff, buffT);
					TextOut(hdc, 395, 382, buffT, wcslen(buffT));
				
					DeleteAllGDI();
					EndPaint(hwnd, &ps);
			
					return 0;
				}
				case WM_LBUTTONUP:
				{	
					
					GetCursorPos(&pt);
					ScreenToClient(hwnd, &pt);   

					//---------------------------------------- ��������ء�
					if(pt.x > 497 && pt.x < 615 && pt.y > 361 && pt.y < 419)
					{
						proState = ProSelect;
					}
					else return 0;

					InvalidateRect(hwnd, NULL, TRUE);
					return 0;
					
				}
				case WM_DESTROY:
				{
					sendAll('z');
					closesocket(socketClient);
					WSACleanup();
					PostQuitMessage(0);
					return 0;
				}
			}
			
			break;
		}
			
		//---------------------------------------- ������Ϣ���� ----------------------------------------
		case ProPersonInformation:
		{
			static HWND hTextName, hTextNick, hTextSex, hTextBirth, hTextPhone, hTextQq, hTextHabit, hTextTips, hTextRelation;
			static int textState = 0;
			
			//����һ�δ��������
			if(textState == 0)
			{
				textState = 1;
				//------------------------------ ����8���ı���
				{
				hTextNick = CreateWindow		//���
				( 
					TEXT("edit"), NULL,			
					WS_CHILD | WS_VISIBLE | ES_LEFT,
					220, 157, 85, 20,
					hwnd, NULL, hIns, NULL 
				);
				memset(buffT, 0 ,200);
				charToTchar(nowPerson.nick, buffT);
				SetWindowText(hTextNick, buffT);
				
				hTextSex = CreateWindow		//�Ա�
				( 
					TEXT("edit"), NULL,			
					WS_CHILD | WS_VISIBLE | ES_LEFT,
					220, 199, 20, 20,
					hwnd, NULL, hIns, NULL 
				);
				memset(buffT, 0 ,200);
				charToTchar(nowPerson.sex, buffT);
				SetWindowText(hTextSex, buffT);
				
				hTextBirth = CreateWindow		//����
				( 
					TEXT("edit"), NULL,			
					WS_CHILD | WS_VISIBLE | ES_LEFT | ES_NUMBER,
					392, 113, 70, 20,
					hwnd, NULL, hIns, NULL 
				);
				memset(buffT, 0 ,200);
				charToTchar(nowPerson.birth, buffT);
				SetWindowText(hTextBirth, buffT);
				
				hTextPhone = CreateWindow		//�绰
				( 
					TEXT("edit"), NULL,			
					WS_CHILD | WS_VISIBLE | ES_LEFT | ES_NUMBER,
					572, 113, 95, 20,
					hwnd, NULL, hIns, NULL 
				);
				memset(buffT, 0 ,200);
				charToTchar(nowPerson.phone, buffT);
				SetWindowText(hTextPhone, buffT);
				
				hTextQq = CreateWindow		//QQ
				( 
					TEXT("edit"), NULL,			
					WS_CHILD | WS_VISIBLE | ES_LEFT | ES_NUMBER,
					572, 157, 95, 20,
					hwnd, NULL, hIns, NULL 
				);
				memset(buffT, 0 ,200);
				charToTchar(nowPerson.qq, buffT);
				SetWindowText(hTextQq, buffT);
				
				hTextHabit = CreateWindow		//����
				( 
					TEXT("edit"), NULL,			
					WS_CHILD | WS_VISIBLE | ES_LEFT,
					160, 274, 220, 20,
					hwnd, NULL, hIns, NULL 
				);
				memset(buffT, 0 ,200);
				charToTchar(nowPerson.habit, buffT);
				SetWindowText(hTextHabit, buffT);
				
				hTextTips = CreateWindow		//��ע
				( 
					TEXT("edit"), NULL,			
					WS_CHILD | WS_VISIBLE | ES_LEFT | ES_MULTILINE,
					420, 274, 220, 35,
					hwnd, NULL, hIns, NULL 
				);
				memset(buffT, 0 ,200);
				charToTchar(nowPerson.tips, buffT);
				SetWindowText(hTextTips, buffT);
				
				hTextRelation = CreateWindow		//���ܶ�
				( 
					TEXT("edit"), NULL,			
					WS_CHILD | WS_VISIBLE | ES_CENTER | ES_NUMBER,
					392, 382, 20, 20,
					hwnd, NULL, hIns, NULL 
				);
				memset(buff, 0 ,200);
				memset(buffT, 0 ,200);
				sprintf(buff, "%d", nowPerson.relation);
				charToTchar(buff, buffT);
				SetWindowText(hTextRelation, buffT);
				
				//�ı�����
				SendMessage(hTextNick, WM_SETFONT, (WPARAM)hFont, TRUE);
				SendMessage(hTextSex, WM_SETFONT, (WPARAM)hFont, TRUE);
				SendMessage(hTextBirth, WM_SETFONT, (WPARAM)hFont, TRUE);
				SendMessage(hTextPhone, WM_SETFONT, (WPARAM)hFont, TRUE);
				SendMessage(hTextQq, WM_SETFONT, (WPARAM)hFont, TRUE);
				SendMessage(hTextHabit, WM_SETFONT, (WPARAM)hFont, TRUE);
				SendMessage(hTextTips, WM_SETFONT, (WPARAM)hFont, TRUE);
				SendMessage(hTextRelation, WM_SETFONT, (WPARAM)hFont, TRUE);
				}
			}
			switch(message)
			{
				case WM_PAINT:
				{
					hdc = BeginPaint(hwnd, &ps);
					bitMap = (HBITMAP)LoadImage(NULL, L"Face/������Ϣ����.bmp", IMAGE_BITMAP, WindWidth, WindHighth, LR_LOADFROMFILE);
					hh = CreateCompatibleDC(NULL);
					SelectObject(hh, bitMap);
					BitBlt(hdc, 0, 0, WindWidth, WindHighth, hh, 0, 0, SRCCOPY);
					
					//�ı��������Ϊ����    
					ZeroMemory(&logFont, sizeof(LOGFONT));   
					logFont.lfCharSet = GB2312_CHARSET;   
					hFont = CreateFontIndirect(&logFont);   
					SelectObject(hdc, hFont); 
					memset(buffT, 0 ,200);
					charToTchar(nowPerson.name, buffT);
					SetWindowText(hTextName, buffT);
					TextOut(hdc, 220, 113, buffT, wcslen(buffT));
					
					DeleteAllGDI();
					EndPaint(hwnd, &ps);
			
					return 0;
				}
				case WM_LBUTTONUP:
				{	
					
					GetCursorPos(&pt);
					ScreenToClient(hwnd, &pt);   

					//---------------------------------------- ��������ء�
					if(pt.x > 497 && pt.x < 615 && pt.y > 361 && pt.y < 419)
					{
						proState = ProMain;
						textState = 0;
					}
					//---------------------------------------- ������޸ġ�
					else if(pt.x > 153 && pt.x < 222 && pt.y > 361 && pt.y < 418)
					{						
						memset(buffT, 0, 200);
						GetWindowText(hTextNick, buffT, 100);
						tcharToChar(buffT, nowPerson.nick);
						
						memset(buffT, 0, 200);
						GetWindowText(hTextBirth, buffT, 100);
						tcharToChar(buffT, nowPerson.birth);
						
						memset(buffT, 0, 200);
						GetWindowText(hTextSex, buffT, 100);
						tcharToChar(buffT, nowPerson.sex);
						if(strcmp(nowPerson.sex, "��") && strcmp(nowPerson.sex, "Ů"))
						{
							memset(nowPerson.sex, 0, 10);
							strcpy(nowPerson.sex, "��");
						}
						
						memset(buffT, 0, 200);
						GetWindowText(hTextQq, buffT, 100);
						tcharToChar(buffT, nowPerson.qq);
						
						memset(buffT, 0, 200);
						GetWindowText(hTextPhone, buffT, 100);
						tcharToChar(buffT, nowPerson.phone);
						
						memset(buffT, 0, 200);
						GetWindowText(hTextHabit, buffT, 100);
						tcharToChar(buffT, nowPerson.habit);
						
						memset(buffT, 0, 200);
						GetWindowText(hTextTips, buffT, 100);
						tcharToChar(buffT, nowPerson.tips);
						
						memset(buff, 0, 100);
						memset(buffT, 0, 200);
						GetWindowText(hTextRelation, buffT, 100);
						tcharToChar(buffT, buff);
						sscanf(buff, "%d", &nowPerson.relation);
						
						sendAll('b');		//�޸�ĳ������
						proState = ProMain;
						textState = 0;
					}
					//---------------------------------------- �����ɾ����
					else if(pt.x > 247 && pt.x < 326 && pt.y > 361 && pt.y < 418)
					{
						nextState = ProMain;
						proState = ProSure;
						lastState = ProPersonInformation;
						textState = 0;
					}
					else return 0;
					
					DestroyWindow(hTextName);
					DestroyWindow(hTextNick);
					DestroyWindow(hTextSex);
					DestroyWindow(hTextBirth);
					DestroyWindow(hTextPhone);
					DestroyWindow(hTextQq);
					DestroyWindow(hTextHabit);
					DestroyWindow(hTextTips);
					DestroyWindow(hTextRelation);
					InvalidateRect(hwnd, NULL, TRUE);
					return 0;
					
				}
				case WM_DESTROY:
				{
					sendAll('z');
					closesocket(socketClient);
					WSACleanup();
					PostQuitMessage(0);
					return 0;
				}
			}
			
			break;
		}
		
		//---------------------------------------- ���������� ----------------------------------------
		case ProAddPerson:
		{
			static HWND hTextName, hTextNick, hTextSex, hTextBirth, hTextPhone, hTextQq, hTextHabit, hTextTips, hTextRelation;
			static int textState = 0;
			
			//����һ�δ��������
			if(textState == 0)
			{
				memset(&nowPerson, 0, sizeof(struct person));
				textState = 1;
				//------------------------------ ����9���ı���
				{
				hTextName = CreateWindow		//����
				( 
					TEXT("edit"), NULL,			
					WS_CHILD | WS_VISIBLE | ES_LEFT,
					220, 113, 85, 20,
					hwnd, NULL, hIns, NULL 
				);
				
				hTextNick = CreateWindow		//���
				( 
					TEXT("edit"), NULL,			
					WS_CHILD | WS_VISIBLE | ES_LEFT,
					220, 157, 85, 20,
					hwnd, NULL, hIns, NULL 
				);

				hTextSex = CreateWindow		//�Ա�
				( 
					TEXT("edit"), NULL,			
					WS_CHILD | WS_VISIBLE | ES_LEFT,
					220, 199, 20, 20,
					hwnd, NULL, hIns, NULL 
				);
				
				hTextBirth = CreateWindow		//����
				( 
					TEXT("edit"), NULL,			
					WS_CHILD | WS_VISIBLE | ES_LEFT | ES_NUMBER,
					392, 113, 70, 20,
					hwnd, NULL, hIns, NULL 
				);

				hTextPhone = CreateWindow		//�绰
				( 
					TEXT("edit"), NULL,			
					WS_CHILD | WS_VISIBLE | ES_LEFT | ES_NUMBER,
					572, 113, 95, 20,
					hwnd, NULL, hIns, NULL 
				);
				
				hTextQq = CreateWindow		//QQ
				( 
					TEXT("edit"), NULL,			
					WS_CHILD | WS_VISIBLE | ES_LEFT | ES_NUMBER,
					572, 157, 95, 20,
					hwnd, NULL, hIns, NULL 
				);

				hTextHabit = CreateWindow		//����
				( 
					TEXT("edit"), NULL,			
					WS_CHILD | WS_VISIBLE | ES_LEFT,
					160, 274, 220, 20,
					hwnd, NULL, hIns, NULL 
				);
				
				hTextTips = CreateWindow		//��ע
				( 
					TEXT("edit"), NULL,			
					WS_CHILD | WS_VISIBLE | ES_LEFT | ES_MULTILINE,
					420, 274, 220, 35,
					hwnd, NULL, hIns, NULL 
				);
				
				hTextRelation = CreateWindow		//���ܶ�
				( 
					TEXT("edit"), NULL,			
					WS_CHILD | WS_VISIBLE | ES_CENTER | ES_MULTILINE | ES_NUMBER,
					392, 382, 20, 20,
					hwnd, NULL, hIns, NULL 
				);
				
				//�ı�����
				SendMessage(hTextName, WM_SETFONT, (WPARAM)hFont, TRUE);
				SendMessage(hTextNick, WM_SETFONT, (WPARAM)hFont, TRUE);
				SendMessage(hTextSex, WM_SETFONT, (WPARAM)hFont, TRUE);
				SendMessage(hTextBirth, WM_SETFONT, (WPARAM)hFont, TRUE);
				SendMessage(hTextPhone, WM_SETFONT, (WPARAM)hFont, TRUE);
				SendMessage(hTextQq, WM_SETFONT, (WPARAM)hFont, TRUE);
				SendMessage(hTextHabit, WM_SETFONT, (WPARAM)hFont, TRUE);
				SendMessage(hTextTips, WM_SETFONT, (WPARAM)hFont, TRUE);
				SendMessage(hTextRelation, WM_SETFONT, (WPARAM)hFont, TRUE);
				}
			}
			switch(message)
			{
				case WM_PAINT:
				{
					hdc = BeginPaint(hwnd, &ps);
					bitMap = (HBITMAP)LoadImage(NULL, L"Face/�޸�����Ӹ�����Ϣ����.bmp", IMAGE_BITMAP, WindWidth, WindHighth, LR_LOADFROMFILE);
					hh = CreateCompatibleDC(NULL);
					SelectObject(hh, bitMap);
					BitBlt(hdc, 0, 0, WindWidth, WindHighth, hh, 0, 0, SRCCOPY);
					
					DeleteAllGDI();
					EndPaint(hwnd, &ps);
				
					return 0;
				}
				case WM_LBUTTONUP:
				{	
					
					GetCursorPos(&pt);
					ScreenToClient(hwnd, &pt);   

					//---------------------------------------- �����ȡ����
					if(pt.x > 497 && pt.x < 615 && pt.y > 361 && pt.y < 419)
					{
						proState = ProMain;
						textState = 0;
					}
					//---------------------------------------- �����ȷ����
					else if(pt.x > 183 && pt.x < 301 && pt.y > 361 && pt.y < 418)
					{
						memset(buffT, 0, 200);
						GetWindowText(hTextName, buffT, 100);
						tcharToChar(buffT, nowPerson.name);
						
						memset(buffT, 0, 200);
						GetWindowText(hTextNick, buffT, 100);
						tcharToChar(buffT, nowPerson.nick);
						
						memset(buffT, 0, 200);
						GetWindowText(hTextBirth, buffT, 100);
						tcharToChar(buffT, nowPerson.birth);
						
						memset(buffT, 0, 200);
						GetWindowText(hTextSex, buffT, 100);
						tcharToChar(buffT, nowPerson.sex);
						if(strcmp(nowPerson.sex, "��") && strcmp(nowPerson.sex, "Ů"))
						{
							memset(nowPerson.sex, 0, 10);
							strcpy(nowPerson.sex, "��");
						}
						
						memset(buffT, 0, 200);
						GetWindowText(hTextQq, buffT, 100);
						tcharToChar(buffT, nowPerson.qq);
						
						memset(buffT, 0, 200);
						GetWindowText(hTextPhone, buffT, 100);
						tcharToChar(buffT, nowPerson.phone);
						
						memset(buffT, 0, 200);
						GetWindowText(hTextHabit, buffT, 100);
						tcharToChar(buffT, nowPerson.habit);
						
						memset(buffT, 0, 200);
						GetWindowText(hTextTips, buffT, 100);
						tcharToChar(buffT, nowPerson.tips);
						
						memset(buff, 0, 100);
						memset(buffT, 0, 200);
						GetWindowText(hTextRelation, buffT, 100);
						tcharToChar(buffT, buff);
						sscanf(buff, "%d", &nowPerson.relation);
						
						for(i = 0; i < listPersonCount; i++)	//�ж������Ƿ�����
						{	
							if(!strcmp(nowPerson.name, listPerson[i].name))
							{
								MessageBox(NULL, TEXT("�˷������и�����"), TEXT("��ʾ"), NULL);
								return 0;
							}
						}
						if(nowPerson.name[0] == '\0')
						{
							MessageBox(NULL, TEXT("��������Ϊ��"), TEXT("��ʾ"), NULL);
							return 0;
						}
						
						sendAll('d');		//���
						proState = ProMain;
						textState = 0;
					}
					else return 0;
					
					DestroyWindow(hTextName);
					DestroyWindow(hTextNick);
					DestroyWindow(hTextSex);
					DestroyWindow(hTextBirth);
					DestroyWindow(hTextPhone);
					DestroyWindow(hTextQq);
					DestroyWindow(hTextHabit);
					DestroyWindow(hTextTips);
					DestroyWindow(hTextRelation);
					InvalidateRect(hwnd, NULL, TRUE);
					return 0;
					
				}
				case WM_DESTROY:
				{
					sendAll('z');
					closesocket(socketClient);
					WSACleanup();
					PostQuitMessage(0);
					return 0;
				}
			}
			
			break;
		}
		
		//---------------------------------------- ��ӷ������ ----------------------------------------
		case ProAddUnit:
		{
			static HWND hText;
			static int textState = 0;
			
			//����һ�δ��������
			if(textState == 0)
			{
				textState = 1;
				hText = CreateWindow		
				( 
					TEXT("edit"), NULL,			
					WS_CHILD | WS_VISIBLE | ES_CENTER,
					355, 228, 90, 17,
					hwnd, NULL, hIns, NULL 
				);
				SendMessage(hText, WM_SETFONT, (WPARAM)hFont, TRUE);
			}
			switch(message)
			{
				case WM_PAINT:
				{
					hdc = BeginPaint(hwnd, &ps);
					bitMap = (HBITMAP)LoadImage(NULL, L"Face/��ӷ������.bmp", IMAGE_BITMAP, WindWidth, WindHighth, LR_LOADFROMFILE);
					hh = CreateCompatibleDC(NULL);
					SelectObject(hh, bitMap);
					BitBlt(hdc, 0, 0, WindWidth, WindHighth, hh, 0, 0, SRCCOPY);
					
					DeleteAllGDI();
					EndPaint(hwnd, &ps);
				
					return 0;
				}
				case WM_LBUTTONUP:
				{	
					
					GetCursorPos(&pt);
					ScreenToClient(hwnd, &pt);   

					//---------------------------------------- �����ȡ����
					if(pt.x > 497 && pt.x < 615 && pt.y > 361 && pt.y < 419)
					{
						proState = ProMain;
						textState = 0;
					}
					//---------------------------------------- �����ȷ����
					else if(pt.x > 183 && pt.x < 301 && pt.y > 361 && pt.y < 418)
					{
						memset(buffT, 0, 200);
						GetWindowText(hText, buffT, 100);
						tcharToChar(buffT, nowUnit);
						if(nowUnit[0] == '\0') 
						{
							MessageBox(NULL, TEXT("��λ������Ϊ��"), TEXT("��ʾ"), NULL);
							return 0;
						}
						for(i = 0; i < unitCount; i++)
						{
							if(!strcmp(nowUnit, unitName[i]))
							{
								MessageBox(NULL, TEXT("��λ����"), TEXT("��ʾ"), NULL);
								return 0;
							}
						}
						
						sendAll('e');		//���
						proState = ProMain;
						textState = 0;
					}
					else return 0;
					
					DestroyWindow(hText);
					InvalidateRect(hwnd, NULL, TRUE);
					return 0;
					
				}
				case WM_DESTROY:
				{
					sendAll('z');
					closesocket(socketClient);
					WSACleanup();
					PostQuitMessage(0);
					return 0;
				}
			}
			
			break;
		}
		
		return 0;
	}

	//DefWindowProc�����Զ������Ϣ������û�д�������Ϣ
	return DefWindowProc(hwnd, message, wParam, lParam);		
}





/*-------------------------------------------------- WinMain -------------------------------------------------- */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	hIns = hInstance;
	static TCHAR myName[] = TEXT("MyWindow");
	HWND hwnd;					
	MSG msg;
	WNDCLASS wndclass;			

	wndclass.style = CS_HREDRAW | CS_VREDRAW;							//������ʽ
	wndclass.lpszClassName = myName;									//��������
	wndclass.lpszMenuName = NULL;										//���ڲ˵�:��
	wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);		//���ڱ�����ɫ
	wndclass.lpfnWndProc = WindProc;									//���ڴ�����
	wndclass.cbWndExtra = 0;											//����ʵ����չ����
	wndclass.cbClsExtra = 0;											//��������չ����
	wndclass.hInstance = hInstance;										//����ʵ�����
	wndclass.hIcon = LoadIcon(NULL, IDI_SHIELD);						//������С��ͼ�꣺ʹ��ȱʡͼ��
	wndclass.hCursor = LoadCursor(NULL,IDC_HAND);						//���ڹ��

	RegisterClass( &wndclass );	//ע�ᴰ����

	//��������
	hwnd = CreateWindow			
	(                   
		myName,														//��������
		TEXT("�ͻ���"),												//���ڱ���
		WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,		//���ڵķ��
		CW_USEDEFAULT,												//���ڳ�ʼ��ʾλ��x��ʹ��ȱʡֵ
		CW_USEDEFAULT,												//���ڳ�ʼ��ʾλ��y��ʹ��ȱʡֵ
		WindWidth,													//���ڵĿ��
		WindHighth,													//���ڵĸ߶�
		NULL,														//�����ڣ���
		NULL,														//�Ӳ˵�����
		hInstance,													//�ô���Ӧ�ó����ʵ����� 
		NULL                       
	);
	hWnd = hwnd;

	RECT rcWindow; 
	RECT rcClient;
	int borderWidth, borderHeight;

	//�������ڿͻ��˴�С����ȷ���سߴ�
	GetWindowRect(hwnd, &rcWindow);
	GetClientRect(hwnd, &rcClient);
	borderWidth = (rcWindow.right-rcWindow.left)-(rcClient.right-rcClient.left);
	borderHeight = (rcWindow.bottom-rcWindow.top)-(rcClient.bottom-rcClient.top);
	SetWindowPos(hwnd, 0, 0, 0, borderWidth+WindWidth, borderHeight+WindHighth, SWP_NOMOVE | SWP_NOZORDER);

	ShowWindow(hwnd, iCmdShow);		//��ʾ����
	UpdateWindow(hwnd);				//���´���
	
	//�ı䴰������
	hdc = GetDC(hwnd);
	logFont.lfHeight = MulDiv(12, GetDeviceCaps(hdc, LOGPIXELSY), 72);	//��һ���������ֺŴ�С
	ReleaseDC(hwnd, hdc);
	logFont.lfWidth = 0;
	logFont.lfEscapement = 0;
	logFont.lfOrientation = 0;
	logFont.lfWeight = FW_REGULAR;
	logFont.lfItalic = 0;
	logFont.lfUnderline = 0;
	logFont.lfStrikeOut = 0;
	logFont.lfCharSet = GB2312_CHARSET;
	logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logFont.lfQuality = PROOF_QUALITY;
	logFont.lfPitchAndFamily = VARIABLE_PITCH  | FF_ROMAN;
	wcscpy(logFont.lfFaceName, TEXT("����")); 
	hFont = CreateFontIndirect(&logFont);

	while(GetMessage(&msg, NULL, 0, 0))			//����Ϣ�����л�ȡ��Ϣ
	{
		TranslateMessage(&msg);                 //���������Ϣת��Ϊ�ַ���Ϣ
		DispatchMessage(&msg);                  //�ַ����ص�����(���̺���)
	}
	return msg.wParam;
}