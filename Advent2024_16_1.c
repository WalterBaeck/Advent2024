#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_16_1 <.dat file>\n");
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
	int Turn, NewCost, Min=0;

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
			// What's the Turn cost associated with this direction choice ?
			Turn = Choice[ChoiceNr] - (ChoiceNr ? Choice[ChoiceNr-1] : 1);
			if (Turn<0) Turn=-Turn;   if (Turn==3)  Turn=1;
			NewCost = 1000*Turn + 1 + (ChoiceNr ? Cost[Ypath[ChoiceNr-1]][Xpath[ChoiceNr-1]] : 0);
			// Possible reasons to reject this choice : into a Wall, or off-Grid
			//if ((Xpath[ChoiceNr] < 0) || (Xpath[ChoiceNr] >= InputXLen))  continue;
			//if ((Ypath[ChoiceNr] < 0) || (Ypath[ChoiceNr] >= InputYLen))  continue;
			if (Input[Ypath[ChoiceNr]][Xpath[ChoiceNr]] == '#')  continue;
			if (Input[Ypath[ChoiceNr]][Xpath[ChoiceNr]] == ',')
				// Revisiting a previously reached position : only when Cost becomes lower
				if (NewCost >= Cost[Ypath[ChoiceNr]][Xpath[ChoiceNr]])
					continue;
			// Have we reached the Target ?
			if (Input[Ypath[ChoiceNr]][Xpath[ChoiceNr]] == 'E')
			{
				if ((!Min) || (Min > NewCost))
					Min = NewCost;
			}
			else
			{
				// All conditions met to move to a further node of the tree
				Cost[Ypath[ChoiceNr]][Xpath[ChoiceNr]] = NewCost;
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

  ///////////////////////////////    O u t p u t    /////////////////////////////

  printf("Found Min = %d\n", Min);
  return 0;
}
