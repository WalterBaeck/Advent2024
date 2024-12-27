#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_08_2 <.dat file>\n");
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

	int X,Y, Index;
  int XPos[64][20];
  int YPos[64][20];
  int NrOfPos[64]={0};

  // Start by cataloging all antennas within the Input grid
  for (Y=0; Y<InputYLen; Y++)
    for (X=0; X<InputXLen; X++)
    {
      if (Input[Y][X] == '.')  continue;
      if      ((Input[Y][X] >= '0') && (Input[Y][X] <= '9'))  Index=Input[Y][X]-'0';
      else if ((Input[Y][X] >= 'A') && (Input[Y][X] <= 'Z'))  Index=Input[Y][X]-'A'+10;
      else if ((Input[Y][X] >= 'a') && (Input[Y][X] <= 'z'))  Index=Input[Y][X]-'a'+36;
      else
      {
        fprintf(stderr, "Unrecognized character %c at [Y=%d,X=%d]\n",
            Input[Y][X], Y,X);
        exit(4);
      }
      XPos[Index][NrOfPos[Index]] = X;  YPos[Index][NrOfPos[Index]] = Y;
      if (++NrOfPos[Index] >= 20)
      {
        fprintf(stderr, "More than 20 occurrences of %c after [Y=%d,X=%d]\n",
            Input[Y][X], Y,X);
        exit(4);
      }
    }

#ifdef DEBUG
  printf("Found %d times 0 and %d times A\n", NrOfPos[0], NrOfPos[10]);
#endif

  // Then go over every antenna type
  for (Index=0; Index<62; Index++)
  {
    // Combine all antennas of this frequency into every possible pair
    int FirstPos,SecondPos;
    int FirstX,FirstY, SecondX, SecondY;
    for (FirstPos=0; FirstPos<NrOfPos[Index]-1; FirstPos++)
      for (SecondPos=FirstPos+1; SecondPos<NrOfPos[Index]; SecondPos++)
      {
        FirstX = XPos[Index][FirstPos]; FirstY = YPos[Index][FirstPos];
        SecondX = XPos[Index][SecondPos]; SecondY = YPos[Index][SecondPos];
        // For this pair of antennas, determine the series of antinodes
        int DiffX,DiffY;
        DiffX = FirstX-SecondX;  DiffY = FirstY-SecondY;
        // All antinodes at the side of the First antenna
        X = FirstX;  Y = FirstY;
        while ((X>=0) && (X<InputXLen) && (Y>=0) && (Y<InputYLen))
        {
          Input[Y][X]='#';
          X += DiffX;   Y += DiffY;
        }
        // Antinode beyond the Second antenna
        X = SecondX;  Y = SecondY;
        while ((X>=0) && (X<InputXLen) && (Y>=0) && (Y<InputYLen))
        {
          Input[Y][X]='#';
          X -= DiffX;   Y -= DiffY;
        }
      }
  } /* for (Index) */

  // Finally, go over the grid again, to count all the # markers
  int Sum=0;
  for (Y=0; Y<InputYLen; Y++)
    for (X=0; X<InputXLen; X++)
      if (Input[Y][X] == '#')  Sum++;

  ///////////////////////////////    O u t p u t    /////////////////////////////

  printf("Found Sum = %d\n", Sum);
  return 0;
}
