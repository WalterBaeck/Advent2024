#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_13_1 <.dat file>\n");
    exit(1);
  }

  FILE* InputFile;
  char InputLine[1000];
	int InputLineNr=0;
	int InputX[2],InputY[2];
	int TargetX,TargetY;
	int Sum=0;

  ////////////////////////////////    I n p u t    //////////////////////////////

  if (!(InputFile = fopen(Argument[1], "r")))
  {
    fprintf(stderr, "Could not open %s for reading.\n", Argument[1]);
    exit(2);
  }
	while (fgets(InputLine, 1000, InputFile))
	{
		InputLineNr++;
		// Strip terminating newline
		while ((InputLine[strlen(InputLine)-1] == '\n') || (InputLine[strlen(InputLine)-1] == '\r'))
			InputLine[strlen(InputLine)-1] = '\0';
		if (!strlen(InputLine))  continue;
		if (!strncmp(InputLine, "Button A: ", 10))
		{
			if (2 != sscanf(InputLine+10, "X+%d, Y+%d", &InputX[0], &InputY[0]))
			{
				fprintf(stderr, "Could not scan Button A numbers on Line #%d:\n%s\n", InputLineNr, InputLine);
				exit(3);
			}
			continue;
		}
		if (!strncmp(InputLine, "Button B: ", 10))
		{
			if (2 != sscanf(InputLine+10, "X+%d, Y+%d", &InputX[1], &InputY[1]))
			{
				fprintf(stderr, "Could not scan Button B numbers on Line #%d:\n%s\n", InputLineNr, InputLine);
				exit(3);
			}
			continue;
		}
		if (!strncmp(InputLine, "Prize: ", 7))
		{
			if (2 != sscanf(InputLine+7, "X=%d, Y=%d", &TargetX, &TargetY))
			{
				fprintf(stderr, "Could not scan Target numbers on Line #%d:\n%s\n", InputLineNr, InputLine);
				exit(3);
			}

  ////////////////////////////    O p e r a t i o n    //////////////////////////

			int A,B, MinA,MinB, Cost,MinCost=0;
			int GoalX,GoalY;

			for (A=0; A<=100; A++)
			{
				GoalX = TargetX - A*InputX[0];
				GoalY = TargetY - A*InputY[0];
				B = GoalX/InputX[1];
				if (GoalX != B*InputX[1])  continue;
				if (GoalY != B*InputY[1])  continue;
				// Solution found - how does the Cost compare to the Minimum ?
				Cost = 3*A + B;
				if (!MinCost)  MinCost = Cost;
				else
					printf("Amazingly, another Cost=%d found besides MinCost=%d!!\n", Cost, MinCost);
			} /* for (A) */

#ifdef DEBUG
			if (!MinCost)
				printf("Target[X=%d,Y=%d] cannot be reached with A:[X=%d,Y=%d] B:[X=%d,Y=%d]\n",
						TargetX,TargetY, InputX[0],InputY[0], InputX[1],InputY[1]);
			else
				printf("Target[X=%d,Y=%d] can be reached with A:[X=%d,Y=%d] B:[X=%d,Y=%d] at MinCost=%d\n",
						TargetX,TargetY, InputX[0],InputY[0], InputX[1],InputY[1], MinCost);
#endif

			Sum += MinCost;
		} /* if (!strncmp("Prize")) */
	} /* while (fgets) */
	fclose(InputFile);
	printf("%d inputlines were read.\n", InputLineNr);

  ///////////////////////////////    O u t p u t    /////////////////////////////

  printf("Found Sum = %d\n", Sum);
  return 0;
}
