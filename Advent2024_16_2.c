#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_16_2 <.dat file>\n");
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

	int X,Y;
#define MAXLEN 10000
	int Choice[MAXLEN], ChoiceNr;
	int Xpath[MAXLEN], Ypath[MAXLEN], Xend,Yend;
	int Turn, NewCost, Min=0, MinLength;
  // Because turning is so expensive, the Cost of passing a certain gridpoint
  //   depends greatly on the *orientation* we arrive in.
  // Register this in 4 separate numbers for every gridpoint.
	int Cost[200][200][4];
  int Orient;

	for (Y=0; Y<InputYLen; Y++)
		for (X=0; X<InputXLen; X++)
      for (Orient=0; Orient<4; Orient++)
        Cost[Y][X][Orient] = -1;

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
	memset(Cost[Y][X], 0, 4 * sizeof(int));
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
			// What's the Turn cost associated with this direction choice ?
			Turn = Choice[ChoiceNr] - (ChoiceNr ? Choice[ChoiceNr-1] : 1);
			if (Turn<0) Turn=-Turn;   if (Turn==3)  Turn=1;
			NewCost = 1000*Turn + 1 + (ChoiceNr ? Cost[Ypath[ChoiceNr-1]][Xpath[ChoiceNr-1]][Choice[ChoiceNr-1]] : 0);
			// Possible reasons to reject this choice : into a Wall, or off-Grid
			//if ((Xpath[ChoiceNr] < 0) || (Xpath[ChoiceNr] >= InputXLen))  continue;
			//if ((Ypath[ChoiceNr] < 0) || (Ypath[ChoiceNr] >= InputYLen))  continue;
			if (Input[Ypath[ChoiceNr]][Xpath[ChoiceNr]] == '#')  continue;
			if (Cost[Ypath[ChoiceNr]][Xpath[ChoiceNr]][Choice[ChoiceNr]] >= 0)
				// Revisiting a previously reached position : only when Cost becomes lower
				if (NewCost >= Cost[Ypath[ChoiceNr]][Xpath[ChoiceNr]][Choice[ChoiceNr]])
					continue;
			// OK so it's acceptable to end up here
      Cost[Ypath[ChoiceNr]][Xpath[ChoiceNr]][Choice[ChoiceNr]] = NewCost;
			// Have we reached the Target ?
			if (Input[Ypath[ChoiceNr]][Xpath[ChoiceNr]] == 'E')
			{
				if ((!Min) || (Min > NewCost))
				{
					Min = NewCost;
					MinLength = ChoiceNr;
				}
        Yend = Ypath[ChoiceNr];  Xend = Xpath[ChoiceNr];
			}
			else
			{
				// All conditions met to move to a further node of the tree
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

	printf("Found MinCost from S to E: %d  in %d steps\n", Min, MinLength+1);
#ifdef DEBUG
  printf("Cost[] numbers at E[Y=%d,X=%d]:", Yend,Xend);
  for (Orient=0; Orient<4; Orient++)  printf("%c%d", Orient ? ',' : ' ', Cost[Yend][Xend][Orient]);
  putchar('\n');
#endif

	// // // // // //      Do the whole thing once more !     // // // // // //

  // We're reusing the Cost[Y][X][Orient] numbers that were registered in the first run.
	char Copy[200][200];
	memcpy(Copy, Input, 200*200);

	// From this S, we're gonna do a full tree search to the E
	ChoiceNr = 0;
	Choice[ChoiceNr] = -1;
	// Always try to make a further direction Choice
	for(;;)
	{
#ifdef DEBUG
		printf("Entering with Choice[%d]=%d from [Y=%d,X=%d]\n", ChoiceNr, Choice[ChoiceNr],
				(ChoiceNr ? Ypath[ChoiceNr-1] : Y), (ChoiceNr ? Xpath[ChoiceNr-1] : X));
#endif
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
			// What's the Turn cost associated with this direction choice ?
			Turn = Choice[ChoiceNr] - (ChoiceNr ? Choice[ChoiceNr-1] : 1);
			if (Turn<0) Turn=-Turn;   if (Turn==3)  Turn=1;
			NewCost = 1000*Turn + 1 + (ChoiceNr ? Cost[Ypath[ChoiceNr-1]][Xpath[ChoiceNr-1]][Choice[ChoiceNr-1]] : 0);
			// Possible reasons to reject this choice : into a Wall, off-Grid, or non-optimal
			//if ((Xpath[ChoiceNr] < 0) || (Xpath[ChoiceNr] >= InputXLen))  continue;
			//if ((Ypath[ChoiceNr] < 0) || (Ypath[ChoiceNr] >= InputYLen))  continue;
			if (Input[Ypath[ChoiceNr]][Xpath[ChoiceNr]] == '#')  continue;
      // Revisiting a previously reached position : only allowed at optimal Cost
      if (NewCost > Cost[Ypath[ChoiceNr]][Xpath[ChoiceNr]][Choice[ChoiceNr]])
        continue;
			// Have we reached the Target ?
			if (Input[Ypath[ChoiceNr]][Xpath[ChoiceNr]] == 'E')
			{
#ifdef DEBUG
				printf("Arriving at E with MinCost=%d  and Len=%d...\n", NewCost, ChoiceNr);
#endif
        // It doesn't matter in which orientation we arrive at E since we're not going further.
        // So this path only counts as optimal, if it is not outperformed by the Cost number
        //  for gridpoint E in ANY orientation
        for (Orient=0; Orient<4; Orient++)
          if ((Cost[Ypath[ChoiceNr]][Xpath[ChoiceNr]][Orient] >= 0) &&
              (NewCost > Cost[Ypath[ChoiceNr]][Xpath[ChoiceNr]][Orient]))  break;
        if (Orient < 4)  continue;   // Better Cost found in another Orientation
        // Mark all positions in between S and E
        int Index;
        for (Index=0; Index<ChoiceNr; Index++)
          Copy[Ypath[Index]][Xpath[Index]] = 'O';
        printf("Found and marked an optimal route\n");
			}
			else
			{
				// All conditions met to move to a further node of the tree
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
#ifdef DEBUG
			Input[Ypath[ChoiceNr]][Xpath[ChoiceNr]] = '+';
  printf("= Backtracking ChoiceNr=%d [Y=%d,X=%d] =\n", ChoiceNr, Ypath[ChoiceNr], Xpath[ChoiceNr]);
	int y;  for (y=0; y<InputYLen; y++)  printf("%s\n", Input[y]);
#endif
		}
	}

	// Count all the marked positions
	int Sum = 0;
  for (Y=0; Y<InputYLen; Y++)
	for (X=0; X<InputXLen; X++)
		if ((Copy[Y][X] == 'S') || (Copy[Y][X] == 'E') || (Copy[Y][X] == 'O'))
			Sum++;
  for (Y=0; Y<InputYLen; Y++)  printf("%s\n", Copy[Y]);

  ///////////////////////////////    O u t p u t    /////////////////////////////

  printf("Found Sum = %d\n", Sum);
  return 0;
}
