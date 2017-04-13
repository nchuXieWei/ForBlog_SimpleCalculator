#include <stdio.h>
#include "Calculator.h"

int main()
{
	while (1)
	{
		if (get())
		{
			translate();
			double result=calculate();
			printf("Result is %lf\n", result);
		}
		else
			break;
	}
	return 0;
}