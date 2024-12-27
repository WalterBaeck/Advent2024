#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_20_2 <.dat file>\n");
    exit(1);
  }

  FILE* InputFile;
  char InputLine[1000];
	char Input[200][200], Grid[200][200];
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
	Cost[Y][X] = 0;
	memcpy(Grid, Input, 200*200);
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
			if (Grid[Ypath[ChoiceNr]][Xpath[ChoiceNr]] == '#')  continue;
			if (Grid[Ypath[ChoiceNr]][Xpath[ChoiceNr]] == 'S')  continue;
			if (Grid[Ypath[ChoiceNr]][Xpath[ChoiceNr]] == ',')
				// Revisiting a previously reached position : only when Cost becomes lower
				if (ChoiceNr+1 >= Cost[Ypath[ChoiceNr]][Xpath[ChoiceNr]])
					continue;
			// OK so it's acceptable to end up here
			Cost[Ypath[ChoiceNr]][Xpath[ChoiceNr]] = ChoiceNr+1;
			// Have we reached the Target ?
			if (Grid[Ypath[ChoiceNr]][Xpath[ChoiceNr]] == 'E')
			{
				if ((!Min) || (Min > ChoiceNr))
					Min = ChoiceNr+1;
			}
			else
			{
				// All conditions met to move to a further node of the tree
				Grid[Ypath[ChoiceNr]][Xpath[ChoiceNr]] = ',';
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
  printf("Found Min = %d\n", Min);

	int Commas=0;
	for (y=0; y<InputYLen; y++)
	{
#ifdef DEBUG
		printf("%s\n", Grid[y]);
#endif
		for (x=0; x<InputXLen; x++)
			if (Grid[y][x] == ',')  Commas++;
	}
	printf("Found %d commas in Grid\n", Commas);

	// // // // //   Repeat this search for every missing piece of wall   // // // // //

	int Xstart,Ystart, Xend,Yend, Xdist,Ydist, Len;
	int Gain, GainArray[10000]={0};

	// Look for any comma in the Grid
  for (Ystart=1; Ystart<InputYLen-1; Ystart++)
		for (Xstart=1; Xstart<InputXLen-1; Xstart++)
		{
			if ((Grid[Ystart][Xstart] != ',') && (Grid[Ystart][Xstart] != 'S'))  continue;

#ifdef DEBUG
			printf("*Considering starting comma at [Y=%d,X=%d] with Cost:%d*\n",
					Ystart,Xstart, Cost[Ystart][Xstart]);
#endif

			// So this will be our Cheat starting point.
			// Within a diamond-shaped area around this Cheat Start,
			//   look for a Cheat End on a racetrack cell.
			for (Yend=Ystart-20; Yend<=Ystart+20; Yend++)
			{
				if ((Yend < 1) || (Yend > InputYLen-2))  continue;
				Ydist = Yend - Ystart;   if (Ydist < 0)  Ydist = -Ydist;
				Xdist = 20 - Ydist;
				for (Xend=Xstart-Xdist; Xend<=Xstart+Xdist; Xend++)
				{
					if ((Xend < 1) || (Xend > InputXLen-2))  continue;
					if ((Grid[Yend][Xend] != ',') && (Grid[Yend][Xend] != 'E'))  continue;

#ifdef DEBUG
					printf("  .. cheat ends at [Y=%d,X=%d]\n", Yend,Xend);
#endif

					// Suitable Cheat Start+End found: compute the gain
					// First determine the length of the Cheat itself
					if (Yend < Ystart)  Len =Ystart-Yend;  else  Len =Yend-Ystart;
					if (Xend < Xstart)  Len+=Xstart-Xend;  else  Len+=Xend-Xstart;

					Gain = Cost[Yend][Xend] - Cost[Ystart][Xstart] - Len;

					if (Gain > 0)
					{
#ifdef DEBUG
						printf("    Found Gain=%d by Cheat [Y=%d,X=%d] -> [Y=%d,X=%d]",
								Gain, Ystart,Xstart, Yend,Xend);
						printf("  /  Cost[%d - %d] - Len %d\n", Cost[Yend][Xend], Cost[Ystart][Xstart], Len);
#endif
						GainArray[Gain]++;
					} /* if (Gain>0) */
				} /* for (Xend) */
			} /* for (Yend) */
		} /* for (Xstart) */

  ///////////////////////////////    O u t p u t    /////////////////////////////

	int Sum=0;
	for (Gain=50; Gain<10000; Gain++)
	{
		if (Gain >= 100)  Sum += GainArray[Gain];
		if (GainArray[Gain])  printf("Found %3d cheats that save %3d ps\n", GainArray[Gain], Gain);
	}

	printf("\nFound Sum %d\n", Sum);
  return 0;
}
