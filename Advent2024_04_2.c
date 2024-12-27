#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_04_2 <.dat file>\n");
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

	int Sum, XSum=0;
	const char* TargetString = "MAS";
	int Dir, Pos, X,Y;

	// Avoid the need for bounds-check by examining suitable 'A' beyond outer border
	for (InputY=1; InputY<InputYLen-1; InputY++)
	for (InputX=1; InputX<InputXLen-1; InputX++)
	{
		// Middle letter match ?
		if (Input[InputY][InputX] != TargetString[1])  continue;
		// Search in 4 directions to make the 3-letter-word
		Sum = 0;
		for (Dir=0; Dir<4; Dir++)
		{
			for (Pos=-1; Pos<=1; Pos+=2)
			{
				X = InputX;  Y = InputY;
				switch (Dir)
				{
					case 0: Y-=Pos;  X+=Pos;   break;
					case 1: Y+=Pos;  X+=Pos;   break;
					case 2: Y+=Pos;  X-=Pos;   break;
					case 3: Y-=Pos;  X-=Pos;   break;
				}

#ifdef DEBUG
				printf("InputX=%d InputY=%d  Dir=%d Pos=%2d  X=%d Y=%d  match(%c,%c)\n",
						InputX,InputY, Dir,Pos, X,Y, Input[Y][X], TargetString[1+Pos]);
#endif

				// Further letter match ?
				if (Input[Y][X] != TargetString[1+Pos])  break;
			} /* for (Pos) */

			// Have we concluded the Pos loop in this Dir ?
			if (Pos > 1)   Sum++;
		} /* for (Dir) */
		
		// An X is formed when two Directions have MAS
		if (Sum == 2)   XSum++;
	}

  ///////////////////////////////    O u t p u t    /////////////////////////////

  printf("Found XSum = %d\n", XSum);
  return 0;
}
