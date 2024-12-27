#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_04_1 <.dat file>\n");
    exit(1);
  }

  FILE* InputFile;
  char InputLine[1000];
	char Input[200][200];
	int InputX,InputY=0, InputXLen=0, InputYLen;

  ////////////////////////////////    I n p u t    //////////////////////////////

  if (!(InputFile = fopen(Argument[1], "r")))
  {
    fprintf(stderr, "Could not open %s for reading.\n", Argument[1]);
    exit(2);
  }
	while (fgets(InputLine, 1000, InputFile))
	{
		// Strip terminating newline
		while ((InputLine[strlen(InputLine)-1] == '\n') || (InputLine[strlen(InputLine)-1] == '\r'))
			InputLine[strlen(InputLine)-1] = '\0';
		strcpy(Input[InputY], InputLine);
		if (!InputXLen)
			InputXLen = strlen(InputLine);
		else if (InputXLen != strlen(InputLine))
		{
			fprintf(stderr, "InputLine #%d has length %d different from established XLen=%d\n",
					InputY, strlen(InputLine), InputXLen);
			exit(3);
		}
		InputY++;
	} /* while (fgets) */
	fclose(InputFile);
	InputYLen = InputY;
	printf("%d inputlines were read of width %d.\n", InputYLen, InputXLen);

  ////////////////////////////    O p e r a t i o n    //////////////////////////

	int Sum=0;
	const char* TargetString = "XMAS";
	int Dir, Pos, X,Y;

	for (InputY=0; InputY<InputYLen; InputY++)
	for (InputX=0; InputX<InputXLen; InputX++)
	{
		// At every grid position, search in 8 directions to make the 4-letter-word
		for (Dir=0; Dir<8; Dir++)
		{
			X = InputX;  Y = InputY;
			// Starting letter match ?
			if (Input[Y][X] != TargetString[0])  continue;
			for (Pos=1; Pos<4; Pos++)
			{
				switch (Dir)
				{
					case 0: Y--;         break;
					case 1: Y--;  X++;   break;
					case 2:       X++;   break;
					case 3: Y++;  X++;   break;
					case 4: Y++;         break;
					case 5: Y++;  X--;   break;
					case 6:       X--;   break;
					case 7: Y--;  X--;   break;
				}
				// Bounds check
				if ((X<0) || (X>=InputXLen))  break;
				if ((Y<0) || (Y>=InputYLen))  break;

#ifdef DEBUG
				printf("InputX=%d InputY=%d  Dir=%d Pos=%d  X=%d Y=%d  match(%c,%c)\n",
						InputX,InputY, Dir,Pos, X,Y, Input[Y][X], TargetString[Pos]);
#endif

				// Further letter match ?
				if (Input[Y][X] != TargetString[Pos])  break;
			} /* for (Pos) */

			// Have we concluded the Pos loop in this Dir ?
			if (Pos >= 4)   Sum++;
		} /* for (Dir) */
	}

  ///////////////////////////////    O u t p u t    /////////////////////////////

  printf("Found Sum = %d\n", Sum);
  return 0;
}
