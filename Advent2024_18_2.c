#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_18_2 <.dat file>\n");
    exit(1);
  }

  FILE* InputFile;
  char InputLine[1000];
	int InputLineNr=0;
	char Input[200][200];
	int InputX,InputY=0, InputXLen=71, InputYLen=71;
	int X,Y;

  ////////////////////////////////    I n p u t    //////////////////////////////

	for (Y=0; Y<InputYLen; Y++)
	for (X=0; X<InputXLen; X++)
		Input[Y][X] = '.';
  if (!(InputFile = fopen(Argument[1], "r")))
  {
    fprintf(stderr, "Could not open %s for reading.\n", Argument[1]);
    exit(2);
  }
	while (fgets(InputLine, 1000, InputFile))
	{
		if (2 != sscanf(InputLine, "%d,%d", &InputX, &InputY))
		{
			fprintf(stderr, "Could not scan X,Y from InputLine #%d: %s", InputLineNr, InputLine);
			exit(3);
		}
		Input[InputY][InputX] = '#';
		if (++InputLineNr <= 1024)  continue;

  ////////////////////////////    O p e r a t i o n    //////////////////////////

	int Cost[200][200];
#define MAXLEN 10000
	int Choice[MAXLEN], ChoiceNr;
	int Xpath[MAXLEN], Ypath[MAXLEN];
	int NewCost, Min=0;

	// From [0,0], we're gonna do a full tree search to [70,70]
	X=Y=0;
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
			// What's the cost associated with this direction choice ?
			NewCost = 1 + (ChoiceNr ? Cost[Ypath[ChoiceNr-1]][Xpath[ChoiceNr-1]] : 0);
			// Possible reasons to reject this choice : into a Wall, or off-Grid
			if ((Xpath[ChoiceNr] < 0) || (Xpath[ChoiceNr] >= InputXLen))  continue;
			if ((Ypath[ChoiceNr] < 0) || (Ypath[ChoiceNr] >= InputYLen))  continue;
			if (Input[Ypath[ChoiceNr]][Xpath[ChoiceNr]] == '#')  continue;
			if (Input[Ypath[ChoiceNr]][Xpath[ChoiceNr]] == ',')
				// Revisiting a previously reached position : only when Cost becomes lower
				if (NewCost >= Cost[Ypath[ChoiceNr]][Xpath[ChoiceNr]])
					continue;
			// Have we reached the Target ?
			if ((Ypath[ChoiceNr] == InputYLen-1) && (Xpath[ChoiceNr] == InputXLen-1))
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

	for (Y=0; Y<InputYLen; Y++)
	for (X=0; X<InputXLen; X++)
		if (Input[Y][X] == ',')
			Input[Y][X] = '.';

  ///////////////////////////////    O u t p u t    /////////////////////////////

		if (!Min)
		{
			printf("Found no more path after fall of [X=%d,Y=%d]\n", InputX, InputY);
			break; /* from while(fgets) */
		}
	} /* while (fgets) */
	fclose(InputFile);
	printf("%d inputlines were read.\n", InputLineNr);

  return 0;
}
