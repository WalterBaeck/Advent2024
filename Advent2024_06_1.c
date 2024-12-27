#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_06_1 <.dat file>\n");
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

	int Dir, X,Y, OldX,OldY;
	char* Pos;

	// First establish the starting position
	for (InputY=0; InputY<InputYLen; InputY++)
		if (Pos = strchr(Input[InputY], '^'))
		{
			Y = InputY;
			X = Pos - Input[InputY];
			Dir = 0;
			printf("Starting at [Y=%d,X=%d] in direction %d\n", Y,X, Dir);
			// Also mark this spot as visited
			Input[Y][X] = 'X';
			break;
		}

	// Then keep following the route forever
	for(;;)
	{
		// Keep in mind where we're coming from
		OldX = X;   OldY = Y;
		// Tentative next coordinates
		switch (Dir)
		{
			case 0: Y--;         break;
			case 1:       X++;   break;
			case 2: Y++;         break;
			case 3:       X--;   break;
		}
		// Bounds check : leave the forever loop when going outside of arena
		if ((X<0) || (X>=InputXLen))  break;
		if ((Y<0) || (Y>=InputYLen))  break;
		// Obstacle check : when bumping, stay in place but turn right
		if (Input[Y][X] == '#')
		{
			X = OldX;   Y = OldY;
			Dir++;  Dir%=4;
		}
		else
			// We arrived at a new spot of the route, mark it
			Input[Y][X] = 'X';
	} /* for(;;) */
	printf("Leaving the grid at [Y=%d, X=%d] in direction %d\n", Y,X, Dir);

#ifdef DEBUG
	for (InputY=0; InputY<InputYLen; InputY++)
		printf("%s\n", Input[InputY]);
#endif

	// Finally, count the number of spots marked with an X
	int Sum=0;
	for (InputY=0; InputY<InputYLen; InputY++)
		for (InputX=0; InputX<InputXLen; InputX++)
			if (Input[InputY][InputX] == 'X')  Sum++;

  ///////////////////////////////    O u t p u t    /////////////////////////////

  printf("Found Sum = %d\n", Sum);
  return 0;
}
