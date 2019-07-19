#include <windows.h>
#include <fstream>
#include <iomanip>
#include <string.h>
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <process.h>
#include <winsock.h>
#include <tchar.h>          
#include <sql.h>     
#include <sqlext.h>     
#include <sqltypes.h>     
#include <odbcss.h> 
#pragma comment(lib,"ws2_32.lib")
using namespace std;

/*-------------------------------------------------- �궨�� -------------------------------------------------- */
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
char stage = 0;					//�׶� 0 - 4
char unit = 0;					//��λ 0 - 4
char matchOrNot = 0;			// 0 Ϊȡ��ƥ�䣬 1 Ϊ����ƥ��
struct person listPerson[1000];	//��ǰ�����е���������
char listPersonCount = 0;

char nowPersonNumber;			//��ǰѡ�еı�� 0 - 71
struct person nowPerson;

char unitCount = 0;		//��ǰ�м�����λ
char unitName[5][100];		//��ǰ��λ����

int port = 60001;

char username[100];

/*-------------------------------------------------- �������� -------------------------------------------------- */
WSAData WSAData;
struct sockaddr_in sockAddr;
int socketServer;
int sockRecpet;

int Connection()
{	
	if(WSAStartup(MAKEWORD(1,1),&WSAData))
	{
		WSACleanup();
		return 1;
	}

	//����ͨ�ŵ�ַ
	memset(&sockAddr,0,sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;		
	sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sockAddr.sin_port = htons(port);

	socketServer = socket(AF_INET,SOCK_STREAM,0);

	bind(socketServer,(sockaddr *)&sockAddr,sizeof(sockAddr));
	listen(socketServer,10);

	printf("�ȴ��ͻ�������...\n");
	sockRecpet = accept(socketServer,NULL,NULL);

	return 0;
}

/*-------------------------------------------------- main -------------------------------------------------- */
SQLHENV henv = SQL_NULL_HENV;     
SQLHDBC hdbc = SQL_NULL_HDBC;     
SQLHSTMT hstmt = SQL_NULL_HSTMT; 
RETCODE retcode;
SQLHENV henv1 = SQL_NULL_HENV;     
SQLHDBC hdbc1 = SQL_NULL_HDBC;     
SQLHSTMT hstmt1 = SQL_NULL_HSTMT; 


int main()
{
	//---------------------------------------- �������ݿ� ----------------------------------------
	//SQL���  
	//1.��������Դ  
	//1.�������  
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);     
	retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);     
	//2.���Ӿ��    
	retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);    
	retcode = SQLConnect(hdbc,   (SQLCHAR *)"wyx", SQL_NTS, (SQLCHAR *)"sa", SQL_NTS, (SQLCHAR *)"123456", SQL_NTS);  

	//�ж������Ƿ�ɹ�  
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)) 
	{       
		printf("����ʧ��!\n");  
		return 0;
	}   
	printf("���������ݿ� data !\n");
	//2.������ִ��һ�������SQL���  
	/* 
	1.����һ�������(statement handle) 
	2.����SQL��� 
	3.ִ����� 
	4.������� 
	*/  
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);    
	
	//---------------------------------------- �������ݿ���� ----------------------------------------

		
	int i, j, k;
	int x, y;
	//--------------------------------------------------
	
	again:;
	if(Connection())
	{
		printf("�����ʼ��ʧ��\n");
		getch();
		return 0;
	}
	printf("�ͻ���������\n");
	
	char recvBuff[100];
	char recvBuff1[100];
	char recvBuff2[100];
	char sendBuff[100];
	char sqlBuff[300];
	char buff[100];
	char buff1[100];
	char buff2[100];
	char buff3[100];
	char buff4[100];
	
	int stage = 6;	//�׶� 0 - 4
	int unit = 6;	//��λ 0 - 9
	
	while(1)
	{
		memset(recvBuff, 0, 100);
		memset(recvBuff1, 0, 100);
		memset(recvBuff2, 0, 100);
		memset(sendBuff, 0, 100);
		
		recv(sockRecpet, recvBuff, 1, 0);
		switch(recvBuff[0])
		{
			//------------------------------ �����û�
			case '0':
			{
				printf("-----�û�ע��\n");
				recv(sockRecpet, recvBuff1, 80, 0);
				recv(sockRecpet, recvBuff2, 80, 0);
				printf("�յ��û��� %s ���¼���� %s\n", recvBuff1, recvBuff2);
				
				//---------- �жϽ��������Ƿ���ȷ
				memset(sqlBuff, 0, 300);
				strcpy(sqlBuff, "insert into UsernamePassword values('");
				strcat(sqlBuff, recvBuff1);
				strcat(sqlBuff, "','");
				strcat(sqlBuff, recvBuff2);
				strcat(sqlBuff, "')");
				printf("sqlBuff:\n%s\n", sqlBuff);
				i = SQLExecDirect(hstmt, (SQLCHAR *)sqlBuff, strlen(sqlBuff));
				retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);  
				printf("����û� i = SQLExecDirect = %d\n", i);
				if(i != SQL_SUCCESS)
				{
					send(sockRecpet, "0", 1, 0);
					printf("�ظ�ע��\n");
					break;
				}
				send(sockRecpet, "1", 1, 0);
				printf("�û���ע�Ტ��¼\n");
				memset(username, 0, 100);
				strcpy(username, recvBuff1);
				printf("user:%s\n",username);
				break;
			}
			//------------------------------ �û���¼
			case '1':
			{
				printf("-----�û������¼\n");
				recv(sockRecpet, recvBuff1, 80, 0);
				recv(sockRecpet, recvBuff2, 80, 0);
				printf("�յ��û��� %s ���¼���� %s\n", recvBuff1,recvBuff2);
				
				//---------- �жϽ��������Ƿ���ȷ
				memset(sqlBuff, 0, 300);
				strcpy(sqlBuff, "select * from UsernamePassword");
				printf("sqlBuff:\n%s\n", sqlBuff);
				
				i = SQLExecDirect(hstmt, (SQLCHAR *)sqlBuff, strlen(sqlBuff));
				printf("�û���¼ȫ������ i = SQLExecDirect = %d\n", i);
				
				SQLBindCol(hstmt, 1, SQL_C_CHAR, buff1, 50, 0);	
				SQLBindCol(hstmt, 2, SQL_C_CHAR, buff2, 50, 0);	
				i = 0;
				do
				{
					memset(buff1, 0, 100);
					memset(buff2, 0, 100);
					retcode = SQLFetch(hstmt);
					
					if(retcode == SQL_NO_DATA)
					{
						printf("DATA OVER!\n");
						break;
					}
					printf("Username %s, Password %s\n", buff1, buff2);
					
					if(!strcmp(recvBuff1, buff1) && !strcmp(recvBuff2, buff2))
					{
						send(sockRecpet, "1", 1, 0);
						printf("�û��Ѿ��ɹ���¼\n");
						memset(username, 0, 100);
						strcpy(username, recvBuff1);
						i = 1;
						continue;
					}
				}while(1);
				if(i == 0)
				{
					send(sockRecpet, "0", 1, 0);
					printf("��¼����\n");
				}

				retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt); 
				break;
			}
			//------------------------------ ��������������
			case 'a':
			{
				printf("-----�ͻ��������б�����\n");
				recv(sockRecpet, recvBuff1, 1, 0);
				matchOrNot = recvBuff1[0];				//��ȡ�Ƿ�ƥ��
				recv(sockRecpet, recvBuff1, 1, 0);
				stage = recvBuff1[0];					//��ȡ��ǰ�׶�
				recv(sockRecpet, recvBuff1, 1, 0);
				unit = recvBuff1[0];					//��ȡ��ǰ��λ
				printf("matchOrNot = %d, stage = %d, unit = %d\n", matchOrNot, stage, unit);
				
				
				//-------------------- ��ȡ�ý׶η�����Ϣ
				memset(sqlBuff, 0, 300);
				strcpy(sqlBuff, "select * from Units where Username like '");
				strcat(sqlBuff, username);
				strcat(sqlBuff, "' and stage =");
				memset(buff, 0, 100);
				sprintf(buff, "%d", stage);
				strcat(sqlBuff, buff);
				
				printf("sqlBuff:\n%s\n", sqlBuff);
				SQLExecDirect(hstmt, (SQLCHAR *)sqlBuff, strlen(sqlBuff));
				printf("��ȡȫ��������Ϣ i = SQLExecDirect = %d\n", i);
				
				SQLBindCol(hstmt, 3, SQL_INTEGER, buff, 50, 0);	
				SQLBindCol(hstmt, 4, SQL_C_CHAR, buff1, 50, 0);	
				i = 0;
				memset(unitName, 0, 500);
				do
				{
					memset(buff1, 0, 100);
					retcode = SQLFetch(hstmt);
					if(retcode == SQL_NO_DATA)
					{
						printf("DATA OVER!\n");
						break;
					}
					j = buff[0];
					printf("unit %d, name %s\n", j, buff1);
					i++;
					strcpy(unitName[j], buff1);
					
				}while(1); 
				unitCount = i;
				retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt); 
				
				sendBuff[0] = unitCount;
				send(sockRecpet, sendBuff, 1, 0);		//���͵�ǰ�׶��м�����λ
				//���͵�λ����
				send(sockRecpet, unitName[0], 100, 0);
				send(sockRecpet, unitName[1], 100, 0);
				send(sockRecpet, unitName[2], 100, 0);
				send(sockRecpet, unitName[3], 100, 0);
				send(sockRecpet, unitName[4], 100, 0);
				
				//-------------------- ��ȡ�÷������г�Ա��Ϣ
				memset(sqlBuff, 0, 300);
				strcpy(sqlBuff, "select * from Persons where Username like '");
				strcat(sqlBuff, username);
				strcat(sqlBuff, "'");
	
				printf("sqlBuff:\n%s\n", sqlBuff);
				i = SQLExecDirect(hstmt, (SQLCHAR *)sqlBuff, strlen(sqlBuff));
				printf("��ȡ�����������˵���Ϣ i = SQLExecDirect = %d\n", i);
				
				SQLBindCol(hstmt, 2, SQL_C_CHAR, nowPerson.name, 50, 0);	
				SQLBindCol(hstmt, 3, SQL_C_CHAR, nowPerson.nick, 50, 0);	
				SQLBindCol(hstmt, 4, SQL_C_CHAR, nowPerson.sex, 50, 0);	
				SQLBindCol(hstmt, 5, SQL_C_CHAR, nowPerson.birth, 50, 0);	
				SQLBindCol(hstmt, 6, SQL_C_CHAR, nowPerson.unit, 26, 0);	
				SQLBindCol(hstmt, 7, SQL_C_CHAR, nowPerson.habit, 100, 0);	
				SQLBindCol(hstmt, 8, SQL_C_CHAR, nowPerson.qq, 50, 0);	
				SQLBindCol(hstmt, 9, SQL_C_CHAR, nowPerson.phone, 50, 0);	
				SQLBindCol(hstmt, 10, SQL_C_CHAR, nowPerson.tips, 200, 0);	
				SQLBindCol(hstmt, 11, SQL_INTEGER, buff, 50, 0);	
				
				i = 0;
				do
				{
					memset(&nowPerson, 0, sizeof(struct person));
					retcode = SQLFetch(hstmt);
					if(retcode == SQL_NO_DATA)
					{
						printf("DATA OVER!\n");
						break;
					}
					if(nowPerson.unit[stage*5+unit] == '1')
					{
						nowPerson.relation = buff[0];
						listPerson[i++] = nowPerson;
						printf("nowPerson.unit:%s\n", nowPerson.unit);
						printf("name %s\n", nowPerson.name);
					}

				}while(1);
				listPersonCount = i;
				retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt); 
				
				sendBuff[0] = listPersonCount;
				send(sockRecpet, sendBuff, 1, 0);		//���͵�ǰ��λ�м�����Ա
				
				//���͵�ǰ��λ��Ա����
				for(i = 0; i < listPersonCount; i++)
				{
					send(sockRecpet, listPerson[i].name, 100, 0);	//����					
					send(sockRecpet, listPerson[i].nick, 100, 0);	//���					
					send(sockRecpet, listPerson[i].sex, 100, 0);	//�Ա�					
					send(sockRecpet, listPerson[i].birth, 100, 0);	//����			
					send(sockRecpet, listPerson[i].unit, 26, 0);	//����			
					send(sockRecpet, listPerson[i].habit, 100, 0);	//����				
					memset(sendBuff, 0 ,100);
					sprintf(sendBuff, "%d", listPerson[i].relation);
					send(sockRecpet, sendBuff, 4, 0);				//��ϵ					
					send(sockRecpet, listPerson[i].qq, 100, 0);		//QQ
					send(sockRecpet, listPerson[i].phone, 100, 0);	//�绰
					send(sockRecpet, listPerson[i].tips, 200, 0);	//��ע
					
					if(matchOrNot == 1)
					{
						//-------------------- ����ƥ��
						memset(sqlBuff, 0, 300);
						strcpy(sqlBuff, "select * from Persons where Username like '");
						strcat(sqlBuff, listPerson[i].name);
						strcat(sqlBuff, "' and name like '");
						strcat(sqlBuff, username);
						strcat(sqlBuff, "'");

						printf("sqlBuff:\n%s\n", sqlBuff);
						k = SQLExecDirect(hstmt, (SQLCHAR *)sqlBuff, strlen(sqlBuff));
						printf("��ȡƥ����Ϣ i = SQLExecDirect = %d\n", k);
						
						memset(buff, 0, 100);
						SQLBindCol(hstmt, 11, SQL_INTEGER, buff, 50, 0);
						retcode = SQLFetch(hstmt);
						retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt); 
						
						j = buff[0];
						if(j > 5 && listPerson[i].relation > 5) listPerson[i].match = 1;
						else listPerson[i].match = 0;
						
						sendBuff[0] = listPerson[i].match;
						send(sockRecpet, sendBuff, 1, 0);				//�Ƿ�ƥ����
					}
				}
				
				break;
			}
			
			//------------------------------ �޸���������
			case 'b':
			{
				recv(sockRecpet, nowPerson.name, 50, 0);
				recv(sockRecpet, nowPerson.nick, 50, 0);
				recv(sockRecpet, nowPerson.sex, 50, 0);
				recv(sockRecpet, nowPerson.birth, 50, 0);
				//recv(sockRecpet, nowPerson.unit, 25, 0);
				recv(sockRecpet, nowPerson.habit, 100, 0);
				recv(sockRecpet, nowPerson.qq, 50, 0);
				recv(sockRecpet, nowPerson.phone, 50, 0);
				recv(sockRecpet, nowPerson.tips, 200, 0);
				memset(recvBuff1, 0, 100);
				recv(sockRecpet, recvBuff1, 1, 0);
				nowPerson.relation = recvBuff1[0]-10;
				
				printf("�޸ĸ�������: %s\n",nowPerson.name);
				
				memset(sqlBuff, 0, 300);
				strcpy(sqlBuff, "update Persons set nick ='");
				strcat(sqlBuff, nowPerson.nick);
				strcat(sqlBuff, "', sex ='");
				strcat(sqlBuff, nowPerson.sex);
				strcat(sqlBuff, "', birth ='");
				strcat(sqlBuff, nowPerson.birth);
				strcat(sqlBuff, "', habit ='");
				strcat(sqlBuff, nowPerson.habit);
				strcat(sqlBuff, "', qq ='");
				strcat(sqlBuff, nowPerson.qq);
				strcat(sqlBuff, "', phone ='");
				strcat(sqlBuff, nowPerson.phone);
				strcat(sqlBuff, "', tips ='");
				strcat(sqlBuff, nowPerson.tips);
				strcat(sqlBuff, "', relation =");
				memset(buff, 0, 100);
				sprintf(buff, "%d", nowPerson.relation);
				strcat(sqlBuff, buff);
				strcat(sqlBuff, " where Username ='");
				strcat(sqlBuff, username);
				strcat(sqlBuff, "' and name ='");
				strcat(sqlBuff, nowPerson.name);
				strcat(sqlBuff, "'");

				printf("sqlBuff:\n%s\n", sqlBuff);
				i = SQLExecDirect(hstmt, (SQLCHAR *)sqlBuff, strlen(sqlBuff));
				retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);  
				
				
				break;
			}
			
			//------------------------------ ɾ��ĳ����ĳ��λ������
			case 'c':
			{
				recv(sockRecpet, nowPerson.name, 50, 0);
				memset(recvBuff, 0, 100);
				recv(sockRecpet, recvBuff, 1, 0);
				stage = recvBuff[0];
				memset(recvBuff, 0, 100);
				recv(sockRecpet, recvBuff, 1, 0);
				unit = recvBuff[0];
				memset(nowPerson.unit, 0, 26);
				recv(sockRecpet, nowPerson.unit, 26, 0);
				nowPerson.unit[stage*5+unit] = '0';
				printf("-----ɾ�� %s ��unit[26]: %s\n", nowPerson.name, nowPerson.unit);
				
				//���û��ɾ��
				if(strcmp(nowPerson.unit, "0000000000000000000000000"))
				{						  
					memset(sqlBuff, 0, 300);
					strcpy(sqlBuff, "update Persons set unit ='");
					strcat(sqlBuff, nowPerson.unit);
					strcat(sqlBuff, "' where Username ='");
					strcat(sqlBuff, username);
					strcat(sqlBuff, "' and name ='");
					strcat(sqlBuff, nowPerson.name);
					strcat(sqlBuff, "'");
				
					printf("sqlBuff:\n%s\n", sqlBuff);
					SQLExecDirect(hstmt, (SQLCHAR *)sqlBuff, strlen(sqlBuff));
					printf("������ɾ������ i = SQLExecDirect = %d\n", i);
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt); 
				}
				//����ɾ��
				else
				{
					memset(sqlBuff, 0, 300);
					strcpy(sqlBuff, "delete from Persons where Username ='");
					strcat(sqlBuff, username);
					strcat(sqlBuff, "' and name ='");
					strcat(sqlBuff, nowPerson.name);
					strcat(sqlBuff, "'");

					printf("sqlBuff:\n%s\n", sqlBuff);
					SQLExecDirect(hstmt, (SQLCHAR *)sqlBuff, strlen(sqlBuff));
					printf("����ɾ������ i = SQLExecDirect = %d\n", i);
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt); 
				}
				
				break;
			}
			//------------------------------ ��ӵ�λ�е�ĳ��
			case 'd':
			{
				recv(sockRecpet, nowPerson.name, 50, 0);
				recv(sockRecpet, nowPerson.nick, 50, 0);
				recv(sockRecpet, nowPerson.sex, 50, 0);
				recv(sockRecpet, nowPerson.birth, 50, 0);
				//recv(sockRecpet, nowPerson.unit, 25, 0);
				recv(sockRecpet, nowPerson.habit, 100, 0);
				recv(sockRecpet, nowPerson.qq, 50, 0);
				recv(sockRecpet, nowPerson.phone, 50, 0);
				recv(sockRecpet, nowPerson.tips, 200, 0);
				memset(recvBuff1, 0, 100);
				recv(sockRecpet, recvBuff1, 1, 0);
				nowPerson.relation = recvBuff1[0]-10;
				
				printf("-----�����ϵ�� %s\n",nowPerson.name);
				
				//---------- �ж���ϵ���Ƿ��Ѿ��������������
				memset(sqlBuff, 0, 300);
				strcpy(sqlBuff, "select * from Persons where Username like '");
				strcat(sqlBuff, username);
				strcat(sqlBuff, "'");
				
				printf("sqlBuff:\n%s\n", sqlBuff);
				SQLExecDirect(hstmt, (SQLCHAR *)sqlBuff, strlen(sqlBuff));
				printf("�û���¼ȫ������ i = SQLExecDirect = %d\n", i);
				
				SQLBindCol(hstmt, 2, SQL_C_CHAR, buff1, 50, 0);	
				SQLBindCol(hstmt, 6, SQL_C_CHAR, buff2, 50, 0);	
				i = 0;
				do
				{
					memset(buff1, 0, 100);
					memset(buff2, 0, 100);
					retcode = SQLFetch(hstmt);
					if(retcode == SQL_NO_DATA)
					{
						printf("DATA OVER!\n");
						break;
					}
					printf("name %s\n", buff1);
					
					if(!strcmp(nowPerson.name, buff1))	//�����Ѿ����������
					{
						i = 1;
						strcpy(buff, buff2);
					}
					
				}while(1);
				retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt); 
				
				//���Ѿ����ڵ��˶����һ������
				if(i == 1)
				{
					printf("�����ظ���Ա\n");
					buff[stage*5+unit] = '1';
					send(sockRecpet, "1", 1, 0);
					
					memset(sqlBuff, 0, 300);
					strcpy(sqlBuff, "update Persons set unit = '");
					strcat(sqlBuff, buff);
					strcat(sqlBuff, "' where Username like '");
					strcat(sqlBuff, username);
					strcat(sqlBuff, "' and name like '");
					strcat(sqlBuff, nowPerson.name);
					strcat(sqlBuff, "'");
					
					printf("sqlBuff:\n%s\n", sqlBuff);
					SQLExecDirect(hstmt, (SQLCHAR *)sqlBuff, strlen(sqlBuff));
					printf("���������unit�� i = SQLExecDirect = %d\n", i);
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt); 
				}
				//û���������ˣ������µ�һ��
				else		
				{
					send(sockRecpet, "0", 1, 0);
					printf("�����µ�����\n");
					memset(sqlBuff, 0, 300);
					strcpy(sqlBuff, "insert into Persons values('");
					strcat(sqlBuff, username);
					strcat(sqlBuff, "','");
					strcat(sqlBuff, nowPerson.name);
					strcat(sqlBuff, "','");
					strcat(sqlBuff, nowPerson.nick);
					strcat(sqlBuff, "','");
					strcat(sqlBuff, nowPerson.sex);
					strcat(sqlBuff, "','");
					strcat(sqlBuff, nowPerson.birth);
					strcat(sqlBuff, "','");
					memset(buff, 0, 100);
					strcpy(buff, "0000000000000000000000000");
					buff[stage*5+unit] = '1';
					strcat(sqlBuff, buff);
					strcat(sqlBuff, "','");
					strcat(sqlBuff, nowPerson.habit);
					strcat(sqlBuff, "','");
					strcat(sqlBuff, nowPerson.qq);
					strcat(sqlBuff, "','");
					strcat(sqlBuff, nowPerson.phone);
					strcat(sqlBuff, "','");
					strcat(sqlBuff, nowPerson.tips);
					strcat(sqlBuff, "',");
					memset(buff, 0, 100);
					sprintf(buff, "%d", nowPerson.relation);
					strcat(sqlBuff, buff);
					strcat(sqlBuff, ")");

					printf("sqlBuff:\n%s\n", sqlBuff);
					i = SQLExecDirect(hstmt, (SQLCHAR *)sqlBuff, strlen(sqlBuff));
					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);  
					
					printf("�����µ����� i = SQLExecDirect = %d\n", i);
				}
					
				break;
			}
			//------------------------------ ��ӷ���
			case 'e':
			{
				
				recv(sockRecpet, recvBuff1, 80, 0);
				printf("-----��ӷ��� %s\n", recvBuff1);

				memset(sqlBuff, 0, 300);
				strcpy(sqlBuff, "insert into Units values('");
				strcat(sqlBuff, username);
				strcat(sqlBuff, "',");
				memset(buff, 0 ,100);
				buff[0] = stage+'0';
				strcat(sqlBuff, buff);
				strcat(sqlBuff, ",");
				memset(buff, 0 ,100);
				buff[0] = unitCount+'0';
				unitCount++;
				strcat(sqlBuff, buff);
				strcat(sqlBuff, ",'");
				strcat(sqlBuff, recvBuff1);
				strcat(sqlBuff, "')");

				printf("sqlBuff:\n%s\n", sqlBuff);
				SQLExecDirect(hstmt, (SQLCHAR *)sqlBuff, strlen(sqlBuff));
				retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt); 
				printf("������� i = SQLExecDirect = %d\n", i);

				break;
			}
			
			//------------------------------ ɾ������ǰ��ȷ��
			case 'f':
			{
				i = 0;
				printf("-----׼��ɾ������ stage = %d, unit = %d\n", stage, unit);
				
				memset(sqlBuff, 0, 300);
				strcpy(sqlBuff, "select * from Persons where Username = '");
				strcat(sqlBuff, username);
				strcat(sqlBuff, "'");
				
				printf("sqlBuff:\n%s\n", sqlBuff);
				SQLExecDirect(hstmt, (SQLCHAR *)sqlBuff, strlen(sqlBuff));
				printf("ɾ������ǰ������ i = SQLExecDirect = %d\n", i);

				SQLBindCol(hstmt, 6, SQL_C_CHAR, nowPerson.unit, 26, 0);	
				i = 0;
				do
				{
					memset(nowPerson.unit, 0, 26);
					retcode = SQLFetch(hstmt);
					if(retcode == SQL_NO_DATA)
					{
						printf("DATA OVER!\n");
						break;
					}
					printf("nowPerson.unit:%s\n", nowPerson.unit);
					nowPerson.unit[stage*5+unit] = '0';	
					if(!strcmp(nowPerson.unit, "0000000000000000000000000"))	//ɾ���һ������ʣ
					i++;
					
				}while(1);
				retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt); 
				
				memset(buff, 0, 100);
				buff[0] = i;
				send(sockRecpet, buff, 1, 0);
				break;
			}
			
			//------------------------------ ɾ������
			case 'g':
			{
				printf("-----ɾ������ stage = %d, unit = %d\n", stage, unit);
				
				//ɾ��Units���е�һ��
				memset(sqlBuff, 0, 300);
				strcpy(sqlBuff, "delete from Units where Username = '");
				strcat(sqlBuff, username);
				strcat(sqlBuff, "' and stage = ");
				memset(buff, 0, 100);
				buff[0] = stage+'0';
				strcat(sqlBuff, buff);
				strcat(sqlBuff, " and unit = ");
				memset(buff, 0, 100);
				buff[0] = unit+'0';
				strcat(sqlBuff, buff);
				
				printf("sqlBuff:\n%s\n", sqlBuff);
				i = SQLExecDirect(hstmt, (SQLCHAR *)sqlBuff, strlen(sqlBuff));
				printf("ɾ������ i = SQLExecDirect = %d\n", i);
				retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
				
				//����Units��
				memset(sqlBuff, 0, 300);
				strcpy(sqlBuff, "update Units set unit = unit-1 where Username = '");
				strcat(sqlBuff, username);
				strcat(sqlBuff, "' and stage = ");
				memset(buff, 0, 100);
				buff[0] = stage+'0';
				strcat(sqlBuff, buff);
				strcat(sqlBuff, " and unit > ");
				memset(buff, 0, 100);
				buff[0] = unit+'0';
				strcat(sqlBuff, buff);

				printf("sqlBuff:\n%s\n", sqlBuff);
				SQLExecDirect(hstmt, (SQLCHAR *)sqlBuff, strlen(sqlBuff));
				printf("�������� i = SQLExecDirect = %d\n", i);
				retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
				
				//��ȡ����Username��name�Լ�unit[26]
				memset(sqlBuff, 0, 300);
				strcpy(sqlBuff, "select * from Persons where Username ='");
				strcat(sqlBuff, username);
				strcat(sqlBuff, "'");
				
				printf("sqlBuff:\n%s\n", sqlBuff);
				SQLExecDirect(hstmt, (SQLCHAR *)sqlBuff, strlen(sqlBuff));
				printf("����Personsǰ�Ĳ��� i = SQLExecDirect = %d\n", i);

				SQLBindCol(hstmt, 2, SQL_C_CHAR, nowPerson.name, 50, 0);
				SQLBindCol(hstmt, 6, SQL_C_CHAR, nowPerson.unit, 26, 0);
				
				i = 0;
				do
				{
					memset(nowPerson.name, 0, 50);
					retcode = SQLFetch(hstmt);
					if(retcode == SQL_NO_DATA)
					{
						printf("DATA OVER!\n");
						break;
					}
					printf("name: %s unit: %s\n", nowPerson.name, nowPerson.unit);
					listPerson[i++] = nowPerson;

				}while(1);
				k = i;
				retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
				
				//�������е�unit
				printf("������\n");
				for(i = 0; i < k; i++)
				{
					j = stage*5;
					if(unit == 0)
					{
						listPerson[i].unit[j+0] = listPerson[i].unit[j+1];
						listPerson[i].unit[j+1] = listPerson[i].unit[j+2];
						listPerson[i].unit[j+2] = listPerson[i].unit[j+3];
						listPerson[i].unit[j+3] = listPerson[i].unit[j+4];
						listPerson[i].unit[j+4] = '0';
					}
					else if(unit == 1)
					{
						listPerson[i].unit[j+1] = listPerson[i].unit[j+2];
						listPerson[i].unit[j+2] = listPerson[i].unit[j+3];
						listPerson[i].unit[j+3] = listPerson[i].unit[j+4];
						listPerson[i].unit[j+4] = '0';
					}
					else if(unit == 2)
					{
						listPerson[i].unit[j+2] = listPerson[i].unit[j+3];
						listPerson[i].unit[j+3] = listPerson[i].unit[j+4];
						listPerson[i].unit[j+4] = '0';
					}
					else if(unit == 3)
					{
						listPerson[i].unit[j+3] = listPerson[i].unit[j+4];
						listPerson[i].unit[j+4] = '0';
					}
					else if(unit == 4)
					{
						listPerson[i].unit[j+4] = '0';
					}
					printf("name: %s unit: %s\n", listPerson[i].name, listPerson[i].unit);
				}
				
				//���²���unit
				for(i = 0; i < k; i++)
				{
					memset(sqlBuff, 0, 300);
					strcpy(sqlBuff, "update Persons set unit ='");
					strcat(sqlBuff, listPerson[i].unit);
					strcat(sqlBuff, "' where Username ='");
					strcat(sqlBuff, username);
					strcat(sqlBuff, "' and name ='");
					strcat(sqlBuff, listPerson[i].name);
					strcat(sqlBuff, "'");

					printf("sqlBuff:\n%s\n", sqlBuff);
					SQLExecDirect(hstmt, (SQLCHAR *)sqlBuff, strlen(sqlBuff));
					printf("����Personsǰ�Ĳ��� i = SQLExecDirect = %d\n", i);

					retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
				}
				
				//ɾ�������Persons
				memset(sqlBuff, 0, 300);
				strcpy(sqlBuff, "delete from Persons where Username ='");
				strcat(sqlBuff, username);
				strcat(sqlBuff, "' and unit ='0000000000000000000000000'");

				printf("sqlBuff:\n%s\n", sqlBuff);
				SQLExecDirect(hstmt, (SQLCHAR *)sqlBuff, strlen(sqlBuff));
				printf("����ɾ������ i = SQLExecDirect = %d\n", i);
				retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt); 
				
				break;
			}
			
			//------------------------------ ����
			case 'h':
			case 'i':
			case 'j':
			{
				memset(buff, 0, 100);
				recv(sockRecpet, buff, 50, 0);
				
				//-------------------- ��ȡ��ѯ�����е���Ϣ
				memset(sqlBuff, 0, 300);
				strcpy(sqlBuff, "select * from Persons where Username like '");
				strcat(sqlBuff, username);
				
				if(recvBuff[0] == 'h')			//������
				{
					printf("-----��ѯ����: %s\n", buff);
					strcat(sqlBuff, "' and name like '");
					strcat(sqlBuff, buff);
					strcat(sqlBuff, "'");
				}
				else if(recvBuff[0] == 'i')		//�Ұ���
				{
					printf("-----��ѯ����: %s\n", buff);
					strcat(sqlBuff, "' and habit like '%");
					strcat(sqlBuff, buff);
					strcat(sqlBuff, "%'");
				}
				else if(recvBuff[0] == 'j')		//������
				{	
					printf("-----��ѯ��ϵ: %s\n", buff);
					strcat(sqlBuff, "' and relation = ");
					if(buff[1] != 0) strcpy(buff, "10");
					strcat(sqlBuff, buff);
				}

				printf("sqlBuff:\n%s\n", sqlBuff);
				i = SQLExecDirect(hstmt, (SQLCHAR *)sqlBuff, strlen(sqlBuff));
				printf("��ȡ��ѯ��Ϣ i = SQLExecDirect = %d\n", i);

				SQLBindCol(hstmt, 2, SQL_C_CHAR, nowPerson.name, 50, 0);	
				SQLBindCol(hstmt, 3, SQL_C_CHAR, nowPerson.nick, 50, 0);	
				SQLBindCol(hstmt, 4, SQL_C_CHAR, nowPerson.sex, 50, 0);	
				SQLBindCol(hstmt, 5, SQL_C_CHAR, nowPerson.birth, 50, 0);	
				SQLBindCol(hstmt, 7, SQL_C_CHAR, nowPerson.habit, 100, 0);	
				SQLBindCol(hstmt, 8, SQL_C_CHAR, nowPerson.qq, 50, 0);	
				SQLBindCol(hstmt, 9, SQL_C_CHAR, nowPerson.phone, 50, 0);	
				SQLBindCol(hstmt, 10, SQL_C_CHAR, nowPerson.tips, 200, 0);	
				SQLBindCol(hstmt, 11, SQL_INTEGER, buff, 50, 0);	

				i = 0;
				do
				{
					memset(&nowPerson, 0, sizeof(struct person));
					retcode = SQLFetch(hstmt);
					if(retcode == SQL_NO_DATA)
					{
						printf("DATA OVER!\n");
						break;
					}
					nowPerson.relation = buff[0];
					listPerson[i++] = nowPerson;
					printf("name %s\n", nowPerson.name);

				}while(1);
				listPersonCount = i;
				retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt); 

				sendBuff[0] = listPersonCount;
				send(sockRecpet, sendBuff, 1, 0);		//���͵�ǰ��λ�м�����Ա

				//������Ա����
				for(i = 0; i < listPersonCount; i++)
				{
					send(sockRecpet, listPerson[i].name, 100, 0);	//����					
					send(sockRecpet, listPerson[i].nick, 100, 0);	//���					
					send(sockRecpet, listPerson[i].sex, 100, 0);	//�Ա�					
					send(sockRecpet, listPerson[i].birth, 100, 0);	//����						
					send(sockRecpet, listPerson[i].habit, 100, 0);	//����				
					memset(sendBuff, 0 ,100);
					sprintf(sendBuff, "%d", listPerson[i].relation);
					send(sockRecpet, sendBuff, 4, 0);				//��ϵ					
					send(sockRecpet, listPerson[i].qq, 100, 0);		//QQ
					send(sockRecpet, listPerson[i].phone, 100, 0);	//�绰
					send(sockRecpet, listPerson[i].tips, 200, 0);	//��ע
				}
				
				break;
			}
			
			//------------------------------ ����Ա�������������û�
			case 'o':
			{
				printf("/////���������û�\n");
				memset(sqlBuff, 0, 300);
				strcpy(sqlBuff, "select * from UsernamePassword");

				printf("sqlBuff:\n%s\n", sqlBuff);
				i = SQLExecDirect(hstmt, (SQLCHAR *)sqlBuff, strlen(sqlBuff));
				printf("��ȡ��ѯ��Ϣ i = SQLExecDirect = %d\n", i);

				SQLBindCol(hstmt, 1, SQL_C_CHAR, buff, 50, 0);	

				i = 0;
				do
				{
					memset(buff, 0, 100);
					retcode = SQLFetch(hstmt);
					if(retcode == SQL_NO_DATA)
					{
						printf("DATA OVER!\n");
						break;
					}
					strcpy(listPerson[i++].name, buff);
					printf("Username %s\n", buff);

				}while(1);
				listPersonCount = i;
				retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt); 

				sendBuff[0] = listPersonCount;
				send(sockRecpet, sendBuff, 1, 0);		//���͵�ǰ��λ�м�����Ա

				//������Ա����
				for(i = 0; i < listPersonCount; i++)
				{
					send(sockRecpet, listPerson[i].name, 100, 0);	//�û���					
				}
				
				break;
			}
			
			//------------------------------ ����Ա����ɾ���û�
			case 'p':
			{	
				recv(sockRecpet, username, 100, 0);
				printf("/////ɾ���û�: %s\n", username);
			
				memset(sqlBuff, 0, 300);
				strcpy(sqlBuff, "delete from UsernamePassword where Username = '");
				strcat(sqlBuff, username);
				strcat(sqlBuff, "'");
				printf("sqlBuff:\n%s\n", sqlBuff);
				i = SQLExecDirect(hstmt, (SQLCHAR *)sqlBuff, strlen(sqlBuff));
				printf("ɾ��UsernamePassword�����û� i = SQLExecDirect = %d\n", i);
				
				memset(sqlBuff, 0, 300);
				strcpy(sqlBuff, "delete from Persons where Username = '");
				strcat(sqlBuff, username);
				strcat(sqlBuff, "'");
				printf("sqlBuff:\n%s\n", sqlBuff);
				i = SQLExecDirect(hstmt, (SQLCHAR *)sqlBuff, strlen(sqlBuff));
				printf("ɾ��Persons�����û� i = SQLExecDirect = %d\n", i);
				
				memset(sqlBuff, 0, 300);
				strcpy(sqlBuff, "delete from Units where Username = '");
				strcat(sqlBuff, username);
				strcat(sqlBuff, "'");
				printf("sqlBuff:\n%s\n", sqlBuff);
				i = SQLExecDirect(hstmt, (SQLCHAR *)sqlBuff, strlen(sqlBuff));
				printf("ɾ��Units�����û� i = SQLExecDirect = %d\n", i);

				break;
			}
			
			//------------------------------ ����Ա���������û�����
			case 'q':
			{	
				recv(sockRecpet, username, 100, 0);
				recv(sockRecpet, buff, 100, 0);
				printf("/////�û�: %s ��������Ϊ: %s\n", username, buff);
				
				memset(sqlBuff, 0, 300);
				strcpy(sqlBuff, "update UsernamePassword set Password = '");
				strcat(sqlBuff, buff);
				strcat(sqlBuff, "' where Username = '");
				strcat(sqlBuff, username);
				strcat(sqlBuff, "'");
				printf("sqlBuff:\n%s\n", sqlBuff);
				i = SQLExecDirect(hstmt, (SQLCHAR *)sqlBuff, strlen(sqlBuff));
				printf("�������� i = SQLExecDirect = %d\n", i);
				
				break;
			}
			
			//------------------------------ �Ͽ�����
			case 'z':
			{
				printf("�Ͽ�����\n");
				closesocket(socketServer);
				WSACleanup();
				goto again;
				break;
			}
		}
	}
	
	
	//�ͷ������  
	SQLCloseCursor(hstmt);  
	SQLFreeHandle(SQL_HANDLE_STMT, hstmt);  
	//3.�Ͽ�����Դ  
	/* 
	1.�Ͽ�������Դ������. 
	2.�ͷ����Ӿ��. 
	3.�ͷŻ������ (���������Ҫ���������������������) 
	*/  
	SQLDisconnect(hdbc);      
	SQLFreeHandle(SQL_HANDLE_DBC, hdbc);     
	SQLFreeHandle(SQL_HANDLE_ENV, henv);   
	
	getch();
	return 0;
}