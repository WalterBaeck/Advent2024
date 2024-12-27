#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_03_1 <.dat file>\n");
    exit(1);
  }

  FILE* InputFile;
  char Input[20000];
  char *InputPtr, *FirstPtr, *SecondPtr;
  int InputLen,Operand[2];

  ////////////////////////////////    I n p u t    //////////////////////////////

  if (!(InputFile = fopen(Argument[1], "r")))
  {
    fprintf(stderr, "Could not open %s for reading.\n", Argument[1]);
    exit(2);
  }
	if (!(InputLen = fread(Input, 1, 20000, InputFile)))
	{
		fprintf(stderr, "Could not read from %s\n", Argument[1]);
		exit(2);
	}
	else
		printf("%d characters read from %s.\n", InputLen, Argument[1]);

  ////////////////////////////    O p e r a t i o n    //////////////////////////

	int Mul, MulSum=0;
	int Field, Digit;
	char OpStr[4];
	int Enabled=1;

	for (InputPtr=Input; InputPtr<Input+InputLen; InputPtr++)
	{
		// Handling the enablers
		if (!strncmp(InputPtr, "do()", 4))
		{
			Enabled = 1;
			InputPtr += 3;
			continue;
		}
		else if (!strncmp(InputPtr, "don\'t()", 7))
		{
			Enabled = 0;
			InputPtr += 6;
			continue;
		}

		if (strncmp(InputPtr, "mul(", 4))   continue;
		InputPtr += 4;
		FirstPtr = InputPtr;
		for (Field=0; Field<2; Field++)
		{
			for (Digit=0; Digit<3; Digit++)
				if ((*InputPtr < '0') || (*InputPtr > '9'))
					break;
				else
					InputPtr++;
			if (!Digit) break;
			memset(OpStr, '\0', 4);
			if (!Field)
			{
				if (*InputPtr != ',')  break;
				SecondPtr = ++InputPtr;
				strncpy(OpStr, FirstPtr, Digit);
			}
			else
			{
				if (*InputPtr != ')')  break;
				strncpy(OpStr, SecondPtr, Digit);
			}
			sscanf(OpStr, "%d", &(Operand[Field]));
		}
		if (Field >= 2)
		{
			Mul = Operand[0] * Operand[1];
			MulSum += (Enabled * Mul);
#ifdef DEBUG
			printf("%sPos %5d: %3d x %3d = %5d\n",
					(Enabled ? "" : "NOT "),
					InputPtr-Input, Operand[0], Operand[1], Mul);
#endif
		}

	} /* for (InputPtr) */

  ///////////////////////////////    O u t p u t    /////////////////////////////

  printf("Found MulSum = %d\n", MulSum);
  return 0;
}

