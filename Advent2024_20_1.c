#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_20_1 <.dat file>\n");
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

	int Cost[200][200];
	int X,Y, x,y;
#define MAXLEN 10000
	int Choice[MAXLEN], ChoiceNr;
	int Xpath[MAXLEN], Ypath[MAXLEN];
	int Min=0, StartMin;

  // Look for the starting position
  for (Y=0; Y<InputYLen; Y++)
	{
		for (X=0; X<InputXLen; X++)
			if (Input[Y][X] == 'S')  break;
		if (X<InputXLen)  break;
	}

	// From this S, we're gonna do a full tree search to the E
	ChoiceNr = 0;
	Choice[ChoiceNr] = -1;
	// Always try to make a further direction Choice
	for(;;)
		// Possible to choose the next direction at the current ChoiceNr ?
		if (++Choice[ChoiceNr] < 4)
		{
			// Yes: then determine the new coordinates at this ChoiceNr
			Xpath[ChoiceNr] = (ChoiceNr ? Xpath[ChoiceNr-1] : X);
			Ypath[ChoiceNr] = (ChoiceNr ? Ypath[ChoiceNr-1] : Y);
			switch (Choice[ChoiceNr])
			{
				case 0: Ypath[ChoiceNr]--;  break;
				case 1: Xpath[ChoiceNr]++;  break;
				case 2: Ypath[ChoiceNr]++;  break;
				case 3: Xpath[ChoiceNr]--;  break;
			}
			// Possible reasons to reject this choice : into a Wall, or off-Grid
			//if ((Xpath[ChoiceNr] < 0) || (Xpath[ChoiceNr] >= InputXLen))  continue;
			//if ((Ypath[ChoiceNr] < 0) || (Ypath[ChoiceNr] >= InputYLen))  continue;
			if (Input[Ypath[ChoiceNr]][Xpath[ChoiceNr]] == '#')  continue;
			if (Input[Ypath[ChoiceNr]][Xpath[ChoiceNr]] == ',')
				// Revisiting a previously reached position : only when Cost becomes lower
				if (ChoiceNr >= Cost[Ypath[ChoiceNr]][Xpath[ChoiceNr]])
					continue;
			// Have we reached the Target ?
			if (Input[Ypath[ChoiceNr]][Xpath[ChoiceNr]] == 'E')
			{
				if ((!Min) || (Min > ChoiceNr))
					Min = ChoiceNr+1;
			}
			else
			{
				// All conditions met to move to a further node of the tree
				Cost[Ypath[ChoiceNr]][Xpath[ChoiceNr]] = ChoiceNr;
				Input[Ypath[ChoiceNr]][Xpath[ChoiceNr]] = ',';
				Choice[++ChoiceNr] = -1;
				if (ChoiceNr >= MAXLEN)
				{
					fprintf(stderr, "Past the %dth position on route\n", MAXLEN);
					exit(4);
				}
			}
		} /* Choice advance was possible */
		else
		{
			// If all Choices have been exhausted at this node, we need to backtrack
			if (--ChoiceNr < 0)
				// When backtracking goes past the root of the tree, the search is complete
				break; // from forever loop
		}
	StartMin = Min;
  printf("Found StartMin = %d\n", StartMin);

	// // // // //   Repeat this search for every missing piece of wall   // // // // //

	int Xwall,Ywall;
	int GainArray[10000]={0};

  for (Ywall=1; Ywall<InputYLen-1; Ywall++)
	for (Xwall=1; Xwall<InputXLen-1; Xwall++)
	{
		if (Input[Ywall][Xwall] != '#')  continue;
		// Temporarily remove this wall
		Input[Ywall][Xwall] = '.';

	// Clear the commas from previous search
  for (y=0; y<InputYLen; y++)
		for (x=0; x<InputXLen; x++)
			if (Input[y][x] == ',')  Input[y][x] = '.';

	// Full tree search
	Min = 0;
	ChoiceNr = 0;
	Choice[ChoiceNr] = -1;
	// Always try to make a further direction Choice
	for(;;)
		// Possible to choose the next direction at the current ChoiceNr ?
		if (++Choice[ChoiceNr] < 4)
		{
			// Yes: then determine the new coordinates at this ChoiceNr
			Xpath[ChoiceNr] = (ChoiceNr ? Xpath[ChoiceNr-1] : X);
			Ypath[ChoiceNr] = (ChoiceNr ? Ypath[ChoiceNr-1] : Y);
			switch (Choice[ChoiceNr])
			{
				case 0: Ypath[ChoiceNr]--;  break;
				case 1: Xpath[ChoiceNr]++;  break;
				case 2: Ypath[ChoiceNr]++;  break;
				case 3: Xpath[ChoiceNr]--;  break;
			}
			// Possible reasons to reject this choice : into a Wall, or off-Grid
			//if ((Xpath[ChoiceNr] < 0) || (Xpath[ChoiceNr] >= InputXLen))  continue;
			//if ((Ypath[ChoiceNr] < 0) || (Ypath[ChoiceNr] >= InputYLen))  continue;
			if (Input[Ypath[ChoiceNr]][Xpath[ChoiceNr]] == '#')  continue;
			if (Input[Ypath[ChoiceNr]][Xpath[ChoiceNr]] == ',')
				// Revisiting a previously reached position : only when Cost becomes lower
				if (ChoiceNr >= Cost[Ypath[ChoiceNr]][Xpath[ChoiceNr]])
					continue;
			// Have we reached the Target ?
			if (Input[Ypath[ChoiceNr]][Xpath[ChoiceNr]] == 'E')
			{
				if ((!Min) || (Min > ChoiceNr))
					Min = ChoiceNr+1;
			}
			else
			{
				// All conditions met to move to a further node of the tree
				Cost[Ypath[ChoiceNr]][Xpath[ChoiceNr]] = ChoiceNr;
				Input[Ypath[ChoiceNr]][Xpath[ChoiceNr]] = ',';
				Choice[++ChoiceNr] = -1;
				if (ChoiceNr >= MAXLEN)
				{
					fprintf(stderr, "Past the %dth position on route\n", MAXLEN);
					exit(4);
				}
			}
		} /* Choice advance was possible */
		else
		{
			// If all Choices have been exhausted at this node, we need to backtrack
			if (--ChoiceNr < 0)
				// When backtracking goes past the root of the tree, the search is complete
				break; // from forever loop
		}
	if (Min < StartMin)
	{
		printf("Found better Min=%d by removing wall [Y=%d,X=%d]\n", Min, Ywall, Xwall);
		GainArray[StartMin - Min]++;
	}

	// Put the wall piece back
	  Input[Ywall][Xwall] = '#';

	} /* for (Xwall) */

  ///////////////////////////////    O u t p u t    /////////////////////////////

	int Index, Sum=0;
	for (Index=100; Index<10000; Index++)
	{
		Sum += GainArray[Index];
#ifdef DEBUG
		if (GainArray[Index])  printf("Found %3d cheats that save %3d ps\n", GainArray[Index], Index);
#endif
	}

	printf("\nFound Sum %d\n", Sum);
  return 0;
}
