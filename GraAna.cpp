// 语法分析 规定e空符号串，并约定写在文法最后
// E->E+T|T T->T*F|F    F->(E)|i------------------------------1
// E->TE'   T->FT'  F->(E)|i  E'->+TE'|e  T'->*FT'|e----------2
// E->TA    T->FB   F->(E)|i  A->+TA|e    B->*FB|e------------3
#include <bits/stdc++.h>
using namespace std;
char *tntarray;                            // 终结符
char *M[5][6], *G[10], *FST[10], *FLW[10]; // M[5][6]分析表,G[10]文法表达式
char out[20];
int Recursion(int G_num) // 消除左递归,返回最终文法个数
{
    char newTN = 'A';
    int newGp, m, n, l;
    n = G_num;                      // n用来记录新的G文法位置
    for (int j = 0; j < G_num; j++) // 依次判断已有文法
        if (*G[j] == *(G[j] + 3))   // 若是则为左递归
        {
            newGp = 3;                    // 新文法位置
            for (m = 4; *(G[j] + m); m++) // 旧文法后面，‘|’前面所有内容填到新文法处
                if (*(G[j] + m) == '|')   // 找到第一个‘|’
                {
                    m++;   // 旧文法‘|’之后内容掩盖前面旧文法
                    l = 3; // 从旧文法下标为3处开始掩盖
                    do
                    {
                        *(G[j] + l) = *(G[j] + m); // 改写旧文法
                        m++;
                        l++;
                    } while (*(G[j] + m)); // 直到结束
                    *(G[j] + l) = newTN++; // 增加新非终结符
                    *(G[j] + l + 1) = 0;   // 添加结束符,旧文法结束
                    // 补全新文法
                    *(G[n]) = *(G[j] + l);
                    *(G[n] + 1) = '-';
                    *(G[n] + 2) = '>';
                    *(G[n] + newGp) = *(G[j] + l);
                    *(G[n] + newGp + 1) = '|';
                    *(G[n] + newGp + 2) = 'e';
                    *(G[n] + newGp + 3) = '\000';
                    n++; // 下一个新文法位置加一
                    break;
                }
                else
                {
                    *(G[n] + newGp) = *(G[j] + m); // 新文法填充内容ing
                    newGp++;
                }
        }
    return n;
}
void First_FstAdd(map<char, int> mp, char former, char latter) // 后者非空FIRST集存入前者FIRST集中
{
    int j;
    for (int i = 0; *(FST[mp[latter]] + i); i++) // 后者某个FIRST集字符
    {
        for (j = 0; *(FST[mp[former]] + j) && *(FST[mp[former]] + j) != *(FST[mp[latter]] + i); j++)
            ;
        if (!*(FST[mp[former]] + j))
        {
            if (*(FST[mp[latter]] + i) != 'e')
                *(FST[mp[former]] + j) = *(FST[mp[latter]] + i);
        }
    }
}
bool First_Single(map<char, int> mp, char TN) // 正在求FIRST集的非终结符
{
    bool flag = false;
    int m = 2, i, fst;
    do
    {
        m++;                                                    // m=3 *(G[mp[TN]] + m)指向E->TA中T(例子)
        if (*(G[mp[TN]] + m) >= 'A' && *(G[mp[TN]] + m) <= 'Z') // 非终结符，添加该非终结符FIRST集
        {
            do
            {
                flag = First_Single(mp, *(G[mp[TN]] + m)); // 是否有空字符，有为true
                First_FstAdd(mp, TN, *(G[mp[TN]] + m));    // 有，加入非空字符
                m++;                                       // 考虑下一个字符
            } while (*(G[mp[TN]] + m) && flag);
            if (flag) // 添加空字符
            {
                for (fst = 0; *(FST[mp[TN]] + fst) && *(FST[mp[TN]] + fst) != 'e'; fst++)
                    ;
                if (!*(FST[mp[TN]] + fst))
                    *(FST[i] + fst) = 'e';
            }
        }
        else // 终结符，添加到FIRST集,并考虑空符号串
        {
            for (fst = 0; *(FST[mp[TN]] + fst) && *(FST[mp[TN]] + fst) != *(G[mp[TN]] + m); fst++)
                ;
            if (!*(FST[mp[TN]] + fst))
                *(FST[mp[TN]] + fst) = *(G[mp[TN]] + m);
        }
        // 寻找该非终结符下一个产生式
        for (m++; *(G[mp[TN]] + m) && *(G[mp[TN]] + m) != '|'; m++) // 存在且不为'|'
            ;                                                       // 空操作
    } while (*(G[mp[TN]] + m));
    return flag;
}
void First(map<char, int> mp, int G_num) // 0~G_num-1均为文法
{
    for (int i = 0; i < G_num; i++) // 依次求每个非终结符FIRST集
        First_Single(mp, *G[i]);
}
void Follow_FlwAdd(map<char, int> mp, char former, char latter) // 后者FOLLOW集存入前者FOLLOW集中
{
    int i, j;
    for (i = 0; *(FLW[mp[latter]] + i); i++) // 后者某个FOLLOW集字符
    {
        for (j = 0; *(FLW[mp[former]] + j) && *(FLW[mp[former]] + j) != *(FLW[mp[latter]] + i); j++)
            ;
        if (!*(FLW[mp[former]] + j))
        {
            *(FLW[mp[former]] + j) = *(FLW[mp[latter]] + i);
        }
    }
}
bool Follow_FstAdd(map<char, int> mp, char former, char latter) // 后者非空FIRST集存入前者FOLLOW集中，返回1，表明有'e'，返回0，表明无
{
    int i, j;
    bool have_e = false;
    for (i = 0; *(FST[mp[latter]] + i); i++) // 后者某个FIRST集字符
    {
        for (j = 0; *(FLW[mp[former]] + j) && *(FLW[mp[former]] + j) != *(FST[mp[latter]] + i); j++)
            ;
        if (!*(FLW[mp[former]] + j))
        {
            if (*(FST[mp[latter]] + i) == 'e')
                have_e = true;
            else
                *(FLW[mp[former]] + j) = *(FST[mp[latter]] + i);
        }
    }
    return have_e;
}
void Follow_Single(map<char, int> mp, char flw) // 求flw的FOLLOW集
{
    int i, j, k, p;
    bool have_e;
    for (i = 0; *G[i]; i++) // 查找每一个文法，i负责大循环
    {
        for (j = 3; *(G[i] + j); j++) // j负责小遍历
        {
            if (*(G[i] + j) == flw)
            {
                // 情况1 后面是终结符（非空符号串），其存入FOLLOW集
                // 情况2 是非终结符，存入其非空FIRST集，结束；有空符号串，考虑下一个非终结符FIRST集
                // 情况3 若后面无内容，该文法规则非终结符的FOLLOW集加入其中
                if (*(G[i] + j + 1) && !(*(G[i] + j + 1) >= 'A' && *(G[i] + j + 1) <= 'Z') && *(G[i] + j + 1) != 'e' && *(G[i] + j + 1) != '|') // 存在且是终结符，非空符号串
                {
                    for (k = 0; *(FLW[mp[flw]] + k) && *(FLW[mp[flw]] + k) != *(G[i] + j + 1); k++)
                        ;
                    if (!*(FLW[mp[flw]] + k))
                        *(FLW[mp[flw]] + k) = *(G[i] + j + 1); // 情况1
                }
                else if (!*(G[i] + j + 1)) // 情况3
                {
                    if (*G[i] != flw)
                    {
                        if (!*FLW[mp[*G[i]]])
                            Follow_Single(mp, *G[i]);
                        Follow_FlwAdd(mp, flw, *G[i]);
                    }
                }
                else if ((*(G[i] + j + 1) >= 'A' && *(G[i] + j + 1) <= 'Z')) // 情况2 前面
                {
                    have_e = Follow_FstAdd(mp, flw, *(G[i] + j + 1));
                    p = 2;
                    while (*(G[i] + j + p) && *(G[i] + j + p) != '|' && have_e)
                    {
                        have_e = Follow_FstAdd(mp, flw, *(G[i] + j + p));
                        p++;
                    }
                    if (!*(G[i] + j + p) || *(G[i] + j + p) == '|')
                    {
                        if (*G[i] != flw)
                        {
                            if (!*FLW[mp[*G[i]]])
                                Follow_Single(mp, *G[i]);
                            Follow_FlwAdd(mp, flw, *G[i]);
                        }
                    }
                }
                break;
            }
        }
    }
}
void Follow(map<char, int> mp)
{
    // 开始符号，'#'存入
    *FLW[mp[*G[0]]] = '#';
    for (int i = 0; *G[i]; i++)
        Follow_Single(mp, *G[i]); // 待求FOLLOW集的字符
}
void AnaTable(map<char, int> mp)
{
    int i, j, k, p, q;
    bool flag;
    for (i = 0; *G[i]; i++) // 遍历每一个非终结符
    {
        // FIRST集非空存放相应文法
        for (j = 0; *(FST[i] + j) && *(FST[i] + j) != 'e'; j++)
        {
            // 只要没‘|’,放置整个文法
            // 若有‘|’,单个放
            for (q = 0; *(G[i] + q); q++)
                if (*(G[i] + q) == '|')
                    flag = true;
            if (flag)
            {
                *(M[mp[*G[i]]][mp[*(FST[i] + j)]] + 0) = *G[i];
                *(M[mp[*G[i]]][mp[*(FST[i] + j)]] + 1) = '-';
                *(M[mp[*G[i]]][mp[*(FST[i] + j)]] + 2) = '>';
                for (k = 3; *(G[i] + k) != *(FST[i] + j); k++)
                    ;
                for (p = 3, k; *(G[i] + k) && *(G[i] + k) != '|'; p++, k++)
                {
                    *(M[mp[*G[i]]][mp[*(FST[i] + j)]] + p) = *(G[i] + k);
                }
            }
            else
            {
                k = 0;
                while (*(G[i] + k))
                {
                    *(M[mp[*G[i]]][mp[*(FST[i] + j)]] + k) = *(G[i] + k);
                    k++;
                }
            }
        }
        // 有空，其FOLLOW集处存放产生空符号串的相应文法
        if (*(FST[i] + j))
        {
            for (k = 0; *(FLW[i] + k); k++)
            {
                *(M[mp[*G[i]]][mp[*(FLW[i] + k)]] + 0) = *G[i];
                *(M[mp[*G[i]]][mp[*(FLW[i] + k)]] + 1) = '-';
                *(M[mp[*G[i]]][mp[*(FLW[i] + k)]] + 2) = '>';
                *(M[mp[*G[i]]][mp[*(FLW[i] + k)]] + 3) = 'e';
            }
        }
    }
}
bool GraAna(map<char, int> mp, char Stack[], char Input[], int p, int q, char ch)
{
    int i, j, o, u, oo, t;
    char x;
    bool FLAG = true;
    while (FLAG)
    {
        x = Stack[p];
        if (strstr(tntarray, &x)) // tntarray存放终结符 前大后小
        {
            if (x == ch)
            {
                q++;
                ch = Input[q];
                p--;
            }
            else
                break;
        }
        else if (x == '#')
        {
            if (x == ch)
                FLAG = false;
            else
                break;
        }
        else if ((strstr(tntarray, &ch) || ch == '#') && *M[mp[x]][mp[ch]]) // 判断是否有转换,不为空表明存在转换符'->',两个字符
        {
            j = 0;
            char tmp; // 不为e空符号串，逆序入栈，为空符号串不进栈
            tmp = *(M[mp[x]][mp[ch]] + 3);
            if (tmp == 'e')
            {
                p--;
                for (o = 0; out[o] != *M[mp[x]][mp[ch]]; o++)
                    ; // 从前往后找到待移动处
                for (o++; out[o]; o++)
                    out[o - 1] = out[o]; // 前移
                out[o - 1] = 0;
                cout << "->" << out;
                continue;
            }
            else
            {
                j++;
                if (!out[0]) // 初始状态，直接输出。否则逐步替换，产生式右侧：空符号串，后续前移，并原位置清零；非空，后续后退
                {
                    cout << "分析过程：" << endl;
                    cout << M[mp[x]][mp[ch]];
                    for (o = 3; *(M[mp[x]][mp[ch]] + o); o++)
                        out[o - 3] = *(M[mp[x]][mp[ch]] + o);
                }
                else
                {
                    for (t = 3, u = 0; *(M[mp[x]][mp[ch]] + t); t++)
                        u++; // 共需移动u-1下
                    for (oo = 19; !out[oo]; oo--)
                        ; // 从后往前找到非零处
                    for (o = oo; o >= 0 && out[o] != *M[mp[x]][mp[ch]]; o--)
                        out[o + u - 1] = out[o]; // 从后往前后移，并判断结束点
                    for (u = 3; *(M[mp[x]][mp[ch]] + u); u++, o++)
                        out[o] = *(M[mp[x]][mp[ch]] + u); // 替换
                    cout << "->" << out;
                }
            }
            i = 3;
            do
            {
                i++;
                j++;
                tmp = *(M[mp[x]][mp[ch]] + i); // E->F
            } while (tmp != '\0');
            i--;                    // i此时指向最后一个非结束符
            j--;                    // j个字符待添加
            for (; j > 0; j--, i--) // j起到一个控制作用
            {
                Stack[p++] = *(M[mp[x]][mp[ch]] + i);
            }
            p--;
        }
        else
            break;
    }
    return FLAG;
}
void Init()
{
    int i, j;
    for (i = 0; i < 5; i++)
        for (j = 0; j < 6; j++)
        {
            M[i][j] = (char *)malloc(20);
            memset(M[i][j], 0, 20);
        }
    for (j = 0; j < 10; j++)
    {
        G[j] = (char *)malloc(20);
        FST[j] = (char *)malloc(20);
        FLW[j] = (char *)malloc(20);
        memset(G[j], 0, 20);
        memset(FST[j], 0, 20);
        memset(FLW[j], 0, 20);
    }
    tntarray = (char *)malloc(20);
    memset(tntarray, 0, 20);
}
int TNT(int G_num)
{
    int i, j, k = 0;
    // 寻找非终结符，非空字符，非'|'
    for (i = 0; i < G_num; i++)
        for (j = 3; *(G[i] + j); j++)
            if (!(*(G[i] + j) >= 'A' && *(G[i] + j) <= 'Z') && *(G[i] + j) != 'e' && *(G[i] + j) != '|')
            {
                if (!strstr(tntarray, G[i] + j))
                {
                    *(tntarray + k) = *(G[i] + j);
                    k++;
                }
            }
    return k;
}
int main()
{
    bool FLAG;
    int i, j, G_num, tnt, m, n;    // 符号栈和输入串指针
    char Stack[20], Input[20], ch; // 符号栈-输入串-栈顶元素-读取字符
    map<char, int> mp;             // 分析表对应键值
    mp['E'] = 0;
    mp['T'] = 1;
    mp['F'] = 2;
    mp['A'] = 3;
    mp['B'] = 4;
    mp['('] = 0;
    mp[')'] = 1;
    mp['+'] = 2;
    mp['*'] = 3;
    mp['i'] = 4;
    mp['#'] = 5;
    map<int, char> mpp; // 分析表对应键值
    mpp[0] = '(';
    mpp[1] = ')';
    mpp[2] = '+';
    mpp[3] = '*';
    mpp[4] = 'i';
    mpp[5] = '#';
    Init(); // 动态分配各字符指针并初始化
    // 若有左递归则消除左递归-----(简单的--不需要归并化简，直接左递归)从1变成3
    cout << "输入文法个数:" << endl;
    cin >> G_num; // G_num为文法个数
    for (i = 0; i < G_num; i++)
        cin >> G[i];          // 0~i均有文法
    G_num = Recursion(G_num); // 返回最终文法个数
    tnt = TNT(G_num);         // 统计终结符个数
    cout << "去掉左递归之后文法:" << endl;
    for (i = 0; i < G_num; i++) // 输出最终文法
        cout << G[i] << endl;
    First(mp, G_num); // FIRST集  E->TA    T->FB   F->(E)|i  A->+TA|e    B->*FB|e------------3
    cout << "各文法FIRST集:" << endl;
    for (i = 0; i < G_num; i++) // 输出FIRST集
        cout << *G[i] << ':' << FST[i] << endl;
    Follow(mp); // FOLLOW集
    cout << "各文法FOLLOW集:" << endl;
    for (i = 0; i < G_num; i++) // 输出FOLLOW集
        cout << *G[i] << ':' << FLW[i] << endl;
    // 分析表
    AnaTable(mp);
    cout << "输出分析表:" << endl;
    for (i = 0; i < G_num; i++)
        for (j = 0; j < 6; j++) // 输出分析表，6是终结符符号个数+1，‘#’
            if (*M[i][j])
                cout << *G[i] << ',' << mpp[j] << ':' << M[i][j] << endl;
    // LL(1)
    cout << "请输入待判断符号串个数:" << endl;
    cin >> n;
    m = 1;
    while (n--)
    {
        int p = 0, q = 0;
        for (int k = 0; k < 20; k++)
        {
            Stack[k] = '\000';
            Input[k] = '\000';
        }
        cout << "请输入待判断符号串" << m++ << ":" << endl;
        cin >> Input;
        for (i = 0; i < 20; i++)
        {
            if (!Input[i])
            {
                Input[i] = '#';
                break;
            }
        }
        Stack[p++] = '#';
        Stack[p] = *G[0]; // 文法开始符号，#和开始符号依次进栈
        ch = Input[q];    // 读取输入串首符号
        FLAG = GraAna(mp, Stack, Input, p, q, ch);
        if (FLAG)
            cout << "\n结果:\n错误" << endl;
        else
            cout << "\n结果:\n正确" << endl;
        for (int tt = 0; out[tt]; tt++)
            out[tt] = 0;
    }
    return 0;
}