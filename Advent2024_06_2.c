#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_06_2 <.dat file>\n");
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

	int Dir, X,Y, OldX,OldY, StartX,StartY;
	char Orig[200][200];
	char *Pos, Marker;

	// First establish the starting position and direction
	for (InputY=0; InputY<InputYLen; InputY++)
		if (Pos = strchr(Input[InputY], '^'))
		{
			StartY = InputY;
			StartX = Pos - Input[InputY];
			Dir = 0;
			printf("Starting at [Y=%d,X=%d] in direction %d\n", Y,X, Dir);
			break;
		}
	// Preserve a copy of the pristine grid
	for (Y=0; Y<InputYLen; Y++)
		strcpy(Orig[Y], Input[Y]);

	// Try all possible obstacle insertion positions
	int Sum=0;
	for (InputY=0; InputY<InputYLen; InputY++)
		for (InputX=0; InputX<InputXLen; InputX++)
		{
			// If the pristine grid had an obstacle here,
			//   this is not a viable insertion position
			if ((Orig[InputY][InputX]=='#') || (Orig[InputY][InputX]=='^'))
				continue;
			// Start from the pristine grid ..
			for (Y=0; Y<InputYLen; Y++)
				strcpy(Input[Y], Orig[Y]);
			// .. and insert the obstacle
			Input[InputY][InputX] = '#';

			// Place ourselves at the original starting position ..
			X = StartX;   Y = StartY;   Dir = 0;
			// .. and keep following the route forever
			for(;;)
			{
				// Keep in mind where we're coming from
				OldX = X;   OldY = Y;
				// Tentative next coordinates
				switch (Dir)
				{
					case 0: Marker='^'; Y--;         break;
					case 1: Marker='>';       X++;   break;
					case 2: Marker='v'; Y++;         break;
					case 3: Marker='<';       X--;   break;
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
				else if (Input[Y][X] == Marker)
				{
					// We seem to be retracing our steps, so the route has entered a loop
					Sum++;

#ifdef DEBUG
  printf("\nOption %d: loop established by obstacle [Y=%d,X=%d]\n", Sum, InputY,InputX);
	for (Y=0; Y<InputYLen; Y++)  printf("%s\n", Input[Y]);
#endif

					break;
				}
				else
					// We arrived at a new spot of the route, mark it
					Input[Y][X] = Marker;
			} /* for(;;) */
		} /* for (InputX) */

  ///////////////////////////////    O u t p u t    /////////////////////////////

  printf("Found Sum = %d\n", Sum);
  return 0;
}
