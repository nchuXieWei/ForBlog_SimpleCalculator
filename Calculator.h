#ifndef CALCULATE   //用于防止头文件被多次包含产生的多次定义问题
#define CALCULATE

//SIZE用于多个场合，如栈的大小、表达式数组的大小
#define SIZE 1000

//表达式的单个元素所使用的结构体
typedef struct elem {
	int num = 0;
	char oper = '=';
	bool IsNum = false;
}Elem;

//get()用于获取用户输入，将用户输入转换为中缀表达式存储在calculate.cpp的中缀表达式数组中
bool get();
//translate()用于将中缀表达式数组中的中缀表达式转换为后缀表达式并存储进后缀表达式数组中
void translate();
//calculate()将利用后缀表达式数组中的后缀表达式计算出表达式结果并返回
double calculate();
#endif

