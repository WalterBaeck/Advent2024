#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_12_5 <.dat file>\n");
    exit(1);
  }

  FILE* InputFile;
  char InputLine[1000];
  int InputLineNr=0;
	char Input[200][200];
	int InputXLen=0, InputYLen=0;
  char* Move;
  int MoveNr=0, NrOfMoves;

  ////////////////////////////////    I n p u t    //////////////////////////////

  if (!(InputFile = fopen(Argument[1], "r")))
  {
    fprintf(stderr, "Could not open %s for reading.\n", Argument[1]);
    exit(2);
  }
  // Get the Grid
	while (fgets(InputLine, 1000, InputFile))
	{
		// Strip terminating newline
		while ((InputLine[strlen(InputLine)-1] == '\n') || (InputLine[strlen(InputLine)-1] == '\r'))
			InputLine[strlen(InputLine)-1] = '\0';
    if (!strlen(InputLine))  break;
		strcpy(Input[InputYLen], InputLine);
		if (!InputXLen)
			InputXLen = strlen(InputLine);
		else if (InputXLen != strlen(InputLine))
		{
			fprintf(stderr, "InputLine #%d has length %d different from established XLen=%d\n",
					InputLineNr, strlen(InputLine), InputXLen);
			exit(3);
		}
		InputYLen++;
    InputLineNr++;
	} /* while (fgets) */
  // Get the Movements
  if (!(Move = (char*)malloc(20000)))
  {
    fprintf(stderr, "Could not malloc 20K chars\n");
    exit(4);
  }
	while (fgets(InputLine, 1000, InputFile))
	{
		// Strip terminating newline
		while ((InputLine[strlen(InputLine)-1] == '\n') || (InputLine[strlen(InputLine)-1] == '\r'))
			InputLine[strlen(InputLine)-1] = '\0';
    if (!strlen(InputLine))  break;
		strcpy(Move+MoveNr, InputLine);
    MoveNr += strlen(InputLine);
    InputLineNr++;
  }
  NrOfMoves = MoveNr;
	fclose(InputFile);
	printf("%d inputlines were read.\n", InputLineNr);

  ////////////////////////////    O p e r a t i o n    //////////////////////////

	int X,Y, x,y, NextX,NextY,  EndX,EndY;
	int Sum=0;

  // Look for the robot's initial position
  for (Y=0; Y<InputYLen; Y++)
  {
    for (X=0; X<InputXLen; X++)
      if (Input[Y][X] == '@')  break;
    if (X<InputXLen)  break;
  }
  // Carry out all the moves in the set
  for (MoveNr=0; MoveNr<NrOfMoves; MoveNr++)
  {
    NextX=X; NextY=Y;  EndX=X; EndY=Y;
    switch (Move[MoveNr])
    {
      case '^': NextY--;
        for (EndY=NextY; Input[EndY][X]=='O'; EndY--);  break;
      case '>': NextX++;
        for (EndX=NextX; Input[Y][EndX]=='O'; EndX++);  break;
      case 'v': NextY++;
        for (EndY=NextY; Input[EndY][X]=='O'; EndY++);  break;
      case '<': NextX--;
        for (EndX=NextX; Input[Y][EndX]=='O'; EndX--);  break;
    }
    // If the row of boxes is being pushed into a wall, nothing moves
    if (Input[EndY][EndX] == '#')  continue;
    // Were there any boxes in the robot's path to begin with ?
    if (Input[NextY][NextX] == 'O')
      Input[EndY][EndX] = 'O';
    // Regardless, the robot occupies the spot where it wanted to go,
    //   and vacates the spot where it came from
    Input[NextY][NextX] = '@';
    Input[Y][X] = '.';
    Y=NextY; X=NextX;
  } /* for (MoveNr) */

  ///////////////////////////////    O u t p u t    /////////////////////////////

  // Compute box positions
  for (Y=0; Y<InputYLen; Y++)
  for (X=0; X<InputXLen; X++)
    if (Input[Y][X] == 'O')
      Sum += (Y*100 + X);
  
  printf("Found Sum = %d\n", Sum);
  return 0;
}
