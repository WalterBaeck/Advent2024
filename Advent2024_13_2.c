#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_13_2 <.dat file>\n");
    exit(1);
  }

  FILE* InputFile;
  char InputLine[1000];
	int InputLineNr=0;
	long long InputX[2],InputY[2];
	long long TargetX,TargetY;
	unsigned long long Sum=0;

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
			if (2 != sscanf(InputLine+10, "X+%llu, Y+%llu", &InputX[0], &InputY[0]))
			{
				fprintf(stderr, "Could not scan Button A numbers on Line #%d:\n%s\n", InputLineNr, InputLine);
				exit(3);
			}
			continue;
		}
		if (!strncmp(InputLine, "Button B: ", 10))
		{
			if (2 != sscanf(InputLine+10, "X+%llu, Y+%llu", &InputX[1], &InputY[1]))
			{
				fprintf(stderr, "Could not scan Button B numbers on Line #%d:\n%s\n", InputLineNr, InputLine);
				exit(3);
			}
			continue;
		}
		if (!strncmp(InputLine, "Prize: ", 7))
		{
			if (2 != sscanf(InputLine+7, "X=%llu, Y=%llu", &TargetX, &TargetY))
			{
				fprintf(stderr, "Could not scan Target numbers on Line #%d:\n%s\n", InputLineNr, InputLine);
				exit(3);
			}

  ////////////////////////////    O p e r a t i o n    //////////////////////////

			long long A,B, Cost;
			long long Numerator, Denominator, X;

			// We're trying to convert the [X,Y] grid into an [A,B] grid.
			// Within that [A,B] grid, if the Target lies on integer coordinates, a solution is found.
			//
			// While matrix conversion formulas exist for affine transformations,
			// this conversion can also be represented as projecting the Target point
			// parallelly to the (slanted) B axis, onto the (slanted) A axis.
			//
			// This parallel projection actually boils down to finding the intersection point
			// of two slanted lines : one line is the slanted A axis itself, going through the origin,
			// and the other slanted line is parallel to slanted B axis, and going through the Target.
			//
			// So, the [X,Y] coordinates of this intersection point satisfy both linear equations:
			// 1.  (Y/X) = (Ya/Xa)    <= so =>      Y = (Ya/Xa) * X
			// 2.  (TargY - Y)/(TargX - X) = (Yb/Xb)    <= so =>      Y = (Yb/Xb)*(X - TargX) + TargY
			//
			// This set of two linear equations can be solved as
			//     [(Ya/Xa) - (Yb/Xb)]*X = TargY - (Yb/Xb)*TargX
			//                        <= so =>
			//    X = [TargY - (Yb/Xb)*TargX]/[(Ya/Xa) - (Yb/Xb)]
			//                        <= so =>
			//    X = Xa*(Xb*TargY - Yb*TargX)/(Ya*Xb - Yb*Xa)
			//
			// From this X coordinate of the intersection point, the A coordinate is found as X/Xa
			// and then the B coordinate can be deduced as (TargX - A*Xa)/Xb

#if 1
			TargetX += (long long)1e13;  TargetY += (long long)1e13;
#endif

			Numerator = InputX[1]*TargetY - InputY[1]*TargetX;
			Denominator = InputY[0]*InputX[1] - InputY[1]*InputX[0];

#ifdef DEBUG
      printf("Target[X=%lld,Y=%lld] to reach with A:[X=%lld,Y=%lld] B:[X=%lld,Y=%lld]\n",
					TargetX,TargetY, InputX[0],InputY[0], InputX[1],InputY[1]);
			printf("Numerator=%lld   Denominator=%lld\n", Numerator, Denominator);
#endif

			X = (Numerator * InputX[0]) / Denominator;
			// Is this a perfect division ?
			if (X * Denominator != Numerator * InputX[0])  continue;
			A = X/InputX[0];
			// Is this a perfect division ?
			if (A * InputX[0] != X)  continue;
			B = (TargetX - A*InputX[0])/InputX[1];
			// Is this a perfect division ?
			if (B * InputX[1] != TargetX - A*InputX[0])  continue;
			Cost = 3*A + B;

			printf("Target[X=%lld,Y=%lld] can be reached with A:[X=%lld,Y=%lld] B:[X=%lld,Y=%lld] at Cost=%lld\n",
					TargetX,TargetY, InputX[0],InputY[0], InputX[1],InputY[1], Cost);

			Sum += (unsigned long long)Cost;
		} /* if (!strncmp("Prize")) */
	} /* while (fgets) */
	fclose(InputFile);
	printf("%d inputlines were read.\n", InputLineNr);

  ///////////////////////////////    O u t p u t    /////////////////////////////

  printf("Found Sum = %llu\n", Sum);
  return 0;
}
