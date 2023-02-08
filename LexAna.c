#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
bool letter(char ch)
{
	return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ? true : false;
}
bool digit(char ch)
{
	return ch >= '0' && ch <= '9' ? true : false;
}
bool underline(char ch)
{
	return ch == '_' ? true : false;
}
bool reserve(int i, char token[])
{
	char *j;															//存放首地址
	char str[] = " int void cin cout return float double return "; //关键字前后均为空格---' key '
	token[0] = ' ';
	j = strstr(str, token); //找' key'
	if (j)					//存在，返回首地址，不存在返回NULL
		return *(j + i + 1) == ' ' ? true : false;
	return false;
}
void output(int i, int j, char token[])
{
	while (j <= i)
		printf("%c", token[j++]);
	printf("\n");
}
int main()
{
	FILE *fp;						   //读取文件
	char ch;						   //读入单个词
	char token[20] = "";			   //存放待判断词
	int i = 0, j = 0, c = 0, size = 0; // i来断token有效范围,j通用,c判关键字标识符,size文件大小
	fp = fopen("D:\\CodeStor\\C_VScode\\LexTest.txt", "r");
	if (!fp)
	{
		printf("file open error\n");
		exit(1);
	}
	fseek(fp, 0, SEEK_END); //定位文件指针到文件尾。
	size = ftell(fp);		//获取文件指针偏移量，即文件大小。
	rewind(fp);				//指针指向首地址
	while (size > 0)
	{
		do
		{
			ch = fgetc(fp);
			if (ch == '\n')
				size--;
		} while (--size >= 0 && (ch == ' ' || ch == '\n' || ch == '\t')); //读取结束或为空格(占1个)，换行（占2个）
		if (size < 0)
			break;
		switch (ch)
		{
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		{		   //若是字母或下划线开头
			i = 1; //字母数字下划线专有，为了区别标识符和关键字
			while (letter(ch) || digit(ch) || underline(ch))
			{
				token[i] = ch;
				ch = fgetc(fp);
				size--;
				i++;
			}
			size++;
			fseek(fp, -1, SEEK_CUR); //回退一个位置
			i--;					 // token[0~i]为内容
			c = reserve(i, token);	 //判断是否为关键字,1关键字，0标识符
			if (c)					 //关键字
				printf("关键字:\t");
			else //标识符
				printf("标识符:\t");
			output(i, 1, token);
			break;
		}
		case '0':
		{
			while (digit(ch))
			{
				token[i] = ch;
				ch = fgetc(fp);
				size--;
				i++;
			}
			size++;
			fseek(fp, -1, SEEK_CUR); //回退一个位置
			i--;					 // token[0~i]为内容
			printf("数字0:\t");
			output(i, 0, token);
			break;
		}
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		{
			while (digit(ch))
			{
				token[i] = ch;
				ch = fgetc(fp);
				size--;
				i++;
			}
			size++;
			fseek(fp, -1, SEEK_CUR); //回退一个位置
			i--;					 // token[0~i]为内容
			printf("非0数:\t");
			output(i, 0, token);
			break;
		}
		case '+':
		case '-':
		case '*':
		case '/':
		case '%':
		{
			printf("运算符:\t%c\n", ch);
			break;
		}
		case '(':
		case ')':
		case '{':
		case '}':
		case '[':
		case ']':
		case ',':
		case ':':
		case ';':
		case '"':
		{
			printf("分隔符:\t%c\n", ch);
			break;
		}
		case '=':
		{
			printf("赋值符:\t%c\n", ch);
			break;
		}
		default:
			printf("其它:\t%c\n", ch);
			break;
		}
		i = 0;						 //用i来断token有效范围
		for (int j = 0; j < 10; j++) //初始化
			token[j] = '\0';
	}
	fclose(fp);
	return 0;
}