#include <stdio.h>  //get()需要使用到某些输入输出函数
#include <ctype.h>  //get()需要利用其中的类型判断函数
#include "Calculator.h" //自定义头文件，包含结构体声明、函数声明


//两个表达式数组，Infix开头的暂存中缀表达式，Postfix开头的存储用于计算的后缀表达式
Elem InfixExpression[SIZE];
Elem PostfixExpression[SIZE];

//操作符栈，topOper指向操作符栈的栈顶（即栈中最上面那个元素的上面那个空位置）
char StackOper[SIZE];
size_t topOper = 0;

//操作符栈的push
void pushOper(char oper)    
{
	StackOper[topOper++] = oper;
}
//操作符栈的pop
char popOper()
{
	return StackOper[--topOper];
}

//用于计算后缀表达式的操作数栈，topNum指向操作数栈的栈顶
//栈内元素为double是因为int/int有可能得出小数
double StackNum[SIZE];
size_t topNum = 0;
//操作数栈的push
void pushNum(double e)
{
	StackNum[topNum++] = e;
}
//操作数栈的pop
double popNum()
{
	return StackNum[--topNum];
}


//get()函数的定义，get()的用处在Calculator.h头文件中
bool get()
{
	//用于保存用户输入的“字符”（还没有“翻译”称表达式的用户输入）
	char input[SIZE * 10];

	//输出提示信息，如果希望终止本程序则输入'n'
	printf("Please enter expression,end with '='\n(if you want to stop calculator,enter 'n'):\n");
	//通过fgets函数获取用户输入
	fgets(input, sizeof(input) / sizeof(char), stdin);


	//简单判断，如果用户键入的是'n'则返回false，主程序会根据get()返回值决定程序走向
	if (input[0] == 'n')
		return false;

	//若用户没有键入'n'则默认用户键入正确的中缀表达式
	//num用于“转换”用户输入的数字字符，具体用法见下
	int num = 0;

	//遍历整个input数组，当然，我们一般中途就会跳出
	for (size_t i = 0, j = 0;i < SIZE * 10;i++)
	{
		//若当前字符为数字字符，则算出当前数字字符与其“左右”的数字字符一起组成了一个什么数
		if (isdigit(input[i]))
		{
			num = num * 10 + input[i] - '0';  //num表示“当前数字”（初始值为0），所以当“再次”遇到一个数字字符时，显然需要“当前数字”乘以10（进位）再加上新的数字字符对应的数字
		}
		//若当前字符不是数字字符，则我们需要判断其是什么字符
		else
		{
			//若当前字符为'='则表示表达式结束，此时我们需要进行一些特殊判断
			if (input[i] == '=')
			{
				//若表达式'='之前的那个字符不是')'则必然是一个数字字符，因此我们需要获取到那个数字字符与其“左右”数字字符组成的数
				if (input[i - 1] != ')')
				{
					InfixExpression[j].IsNum = true;
					InfixExpression[j++].num = num;
					num = 0;   //获取完数字字符组成的数后，我们要将num重置以用于下一次“转换”数字字符
				}
				//无论'='前一个字符是数字字符还是')'，我们都需要将'='存入中缀表达式数组并跳出对input[]的遍历
				InfixExpression[j].IsNum = false;
				InfixExpression[j++].oper = '=';
				break;
			}
			//'('是输入的又一特例，'('的前一个字符理应为运算符，所以我们不用也不该“获取”num的值
			else if (input[i] == '(')
			{
				InfixExpression[j].IsNum = false;
				InfixExpression[j++].oper = '(';
			}
			else if (input[i] == ')'&& input[i-1] == ')')
			{
				
					InfixExpression[j].IsNum = false;
					InfixExpression[j++].oper = ')';
			
			}
			//除去上述特例，不论是运算符还是')'，其前一个字符理应为数字字符，因此我们需要“获取”num的值，然后将操作符也存起来并重置num
			else
			{
				InfixExpression[j].IsNum = true;
				InfixExpression[j++].num = num;
				num = 0;
				InfixExpression[j].IsNum = false;
				InfixExpression[j++].oper = input[i];
			}
		}
	}


	//以下循环为辅助性的，效果是输出中缀表达式中存储的表达式，理论上屏幕输出应与使用者输入相同
	for (size_t i = 0;i < SIZE;++i)
	{
		if (!InfixExpression[i].IsNum&&InfixExpression[i].oper == '=')
		{
			printf("%c\n", '=');
			break;
		}
		else if (InfixExpression[i].IsNum)
		{
			printf("%d", InfixExpression[i].num);
		}
		else
		{
			printf("%c", InfixExpression[i].oper);
		}

	}
	//返回true告诉主程序用户没有键入'n'
	return true;
}






//用于translate()的一些函数，负责栈操作
//将中缀转换为后缀时，如果遇到操作符，那么我们需要对操作符进行判断然后决定相应的（栈）操作
//下面这些函数就是当遇到不同操作符时调用的不同函数，用如其名
void IsAdd(size_t *j);
void IsSub(size_t *j);
void IsMulti();
void IsDiv();
void IsLeft();
void IsRight(size_t *j);
void IsEqual(size_t *j);


//translate()函数的定义，其用途说明在Calculator.h中
void translate()
{
	//遍历中缀表达式数组，将其中存储的中缀表达式转换为后缀表达式并存入后缀表达式数组
	//i为中缀表达式数组的“当前下标”（当前所判断的元素），j为后缀表达式数组的“当前下标”（输出到后缀的新元素应放入的位置），切记两者并不同步
	for (size_t i = 0, j = 0;i < SIZE;++i)
	{
		//若当前中缀（中缀表达式的简称）元素为数字则我们直接将其“输出”到后缀表达式
		if (InfixExpression[i].IsNum)
		{
			PostfixExpression[j].IsNum = true;
			PostfixExpression[j++].num = InfixExpression[i].num;
		}
		//若当前中缀元素不是数字，则我们需要根据其“值”决定应选择的栈操作，这里也是中缀下标i和后缀下标j不同步的原因产生之处
		else
		{
			switch (InfixExpression[i].oper)
			{
			case '(':
				IsLeft();  //当前元素为'('时，我们调用IsLeft()，因为'('必然是直接入栈，所以我们的j不会发生变化
				break;
			case ')':
				IsRight(&j); //当前元素为')'时调用IsRight()，因为')'可能导致输出元素至后缀表达式，所以需要知道后缀的下标j，并且j可能会发生变化，我们将j的地址传递过去
				break;
			case '+':
				IsAdd(&j);  //当前元素为'+'时调用IsAdd()，因为'+'可能导致输出元素至后缀表达式，所以需要知道后缀的下标j，并且j可能会发生变化，我们将j的地址传递过去
				break;
			case '-':
				IsSub(&j);  //当前元素为'-'时调用IsSub()，因为'-'可能导致输出元素至后缀表达式，所以需要知道后缀的下标j，并且j可能会发生变化，我们将j的地址传递过去
				break;
			case '*':
				IsMulti();  //当前元素为'*'时调用IsMult()，因为'*'直接入栈，所以j不会发生变化，不需要传递
				break;
			case '/':
				IsDiv();  //当前元素为'/'时调用IsDiv()，因为'/'直接入栈，所以j不会发生变化，不需要传递
				break;
			case '=':   //当前元素为'='时调用IsEqual()，因为'='会导致输出元素至后缀表达式，所以需要知道j，理论上只需要j的值因为表达式要结束了，但我们还是传递了j的地址，你可以自行修改╮(╯_╰)╭
				IsEqual(&j);
				return;
			}
		}
	}
}

//如果是'('则直接pushOper()
void IsLeft()
{
	pushOper('(');
}

//如果是')'则弹出栈顶元素直至栈顶元素为'('，当栈顶元素为'('时弹出并丢弃
void IsRight(size_t *j)
{
	char oper;
	//如果是正确的表达式，则遇到)时栈内一定有(，此时循环条件其实没作用
	while (topOper > 0)
	{
		//获取栈顶元素
		oper = popOper();

		//如果是'('则返回，因为'('被丢弃所以可以不理睬
		if (oper == '(')
			return;
		//如果不是'('则将该操作符“输出”至后缀表达式
		else
		{
			PostfixExpression[(*j)].IsNum = false;
			PostfixExpression[(*j)++].oper = oper;
		}
	}
}

void IsAdd(size_t *j)
{
	char oper;
	while (topOper > 0)
	{
		oper = popOper();
		if (oper == '(')
		{
			pushOper(oper);
			break;
		}
		else
		{
			PostfixExpression[(*j)].IsNum = false;
			PostfixExpression[(*j)++].oper = oper;
		}
	}

	pushOper('+');
}

void IsSub(size_t *j)
{
	char oper;
	while (topOper > 0)
	{
		oper = popOper();
		if (oper == '(')
		{
			pushOper(oper);
			break;
		}
		else
		{
			PostfixExpression[(*j)].IsNum = false;
			PostfixExpression[(*j)++].oper = oper;
		}

	}

	pushOper('-');
}

void IsMulti()
{
	pushOper('*');
}

void IsDiv()
{
	pushOper('/');
}

void IsEqual(size_t *j)
{
	char oper;
	while (topOper > 0)
	{
		oper = popOper();
		PostfixExpression[(*j)].IsNum = false;
		PostfixExpression[(*j)++].oper = oper;
	}
	PostfixExpression[(*j)].IsNum = false;
	PostfixExpression[(*j)++].oper = '=';
}


//虽然我们的结构体中使用的是int，但是int/int也有可能出现小数，所以我们返回值使用double
double calculate()
{
	//遍历后缀表达式数组，输出后缀表达式，没有特殊目的，只是方便我们“检查”一下后缀表达式
	for (size_t i = 0;i < SIZE;++i)
	{
		if (!PostfixExpression[i].IsNum && PostfixExpression[i].oper == '=')
		{
			printf("%c\n", '=');
			break;
		}
		else if (PostfixExpression[i].IsNum)
		{
			printf("%d", PostfixExpression[i].num);
		}
		else
		{
			printf("%c", PostfixExpression[i].oper);
		}
	}

	//后缀表达式的计算过程，遍历整个后缀表达式数组，理论上我们中途会遇到'='跳出遍历，如果没有，好吧，程序要出错了
	for (size_t i = 0;i < SIZE;++i)
	{
		//如果当前元素不是数字且其oper=='='，则说明表达式到末尾了，此时我们弹出栈内元素（理应为唯一一个）作为计算结果返回
		if (!PostfixExpression[i].IsNum&&PostfixExpression[i].oper == '=')
		{
			return popNum();
		}
		//如果当前元素为数字，则我们将其转换为double型并入栈
		else if (PostfixExpression[i].IsNum)
		{
			pushNum((double)(PostfixExpression[i].num));
		}
		//如果当前元素不是数字也不符合oper=='='就说明其是一个运算符（不会是括号，后缀表达式没有括号）
		//此时我们按计算方式pop出栈顶两元素进行计算并将结果重新压入栈
		else
		{
			//temp用于暂存栈顶两元素的计算结果
			double temp = 0.0;
			//注意，第一次popNum得到的应作为右操作数，第二次popNum得到的作为左操作数，我们分别记为rear和front
			double rear = popNum();
			double front = popNum();

			//根据当前元素选择应进行的运算，并将结果入栈
			switch (PostfixExpression[i].oper)
			{
			case '+':
				temp = front + rear;
				pushNum(temp);
				break;
			case '-':
				temp = front - rear;
				pushNum(temp);
				break;
			case '*':
				temp = front*rear;
				pushNum(temp);
				break;
			case '/':
				temp = front / rear;
				pushNum(temp);
				break;
			}
		}
	}

	return 0;
}