#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_15_2 <.dat file>\n");
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

	int X,Y, NextX,NextY,  EndX;
  int IncrX,IncrY;
  int QueueX[2000],QueueY[2000], ReadPtr,WritePtr,Index;

  // Grid duplication
	char Grid[200][200];
	int GridXLen=InputXLen*2, GridYLen=InputYLen;
  for (Y=0; Y<InputYLen; Y++)
    for (X=0; X<InputXLen; X++)
      switch (Input[Y][X])
      {
        case '#' : strcpy(Grid[Y]+X*2, "##");  break;
        case '.' : strcpy(Grid[Y]+X*2, "..");  break;
        case '@' : strcpy(Grid[Y]+X*2, "@.");  break;
        case 'O' : strcpy(Grid[Y]+X*2, "[]");  break;
      }

  // Helper function
  void AddToQueue(int argX, int argY)
  {
    // First, make sure that the new element isn't already in the queue
    for (Index=0; Index<WritePtr; Index++)
      if ((QueueX[Index] == argX) && (QueueY[Index] == argY))  return;
    QueueX[WritePtr] = argX;
    QueueY[WritePtr] = argY;
    WritePtr++;
  }

  // Look for the robot's initial position
  for (Y=0; Y<GridYLen; Y++)
  {
    for (X=0; X<GridXLen; X++)
      if (Grid[Y][X] == '@')  break;
    if (X<GridXLen)  break;
  }
  // Carry out all the moves in the set
  for (MoveNr=0; MoveNr<NrOfMoves; MoveNr++)
  {

#ifdef DEBUG
    printf("===== Before move #%2d: %c ======\n", MoveNr, Move[MoveNr]);
    int y;
    for (y=0; y<GridYLen; y++)  printf("%s\n", Grid[y]);
#endif

    // Handle trivial cases first : move into wall or into empty space
    NextX=X; NextY=Y;
    switch (Move[MoveNr])
    {
      case '^': NextY--;  break;
      case '>': NextX++;  break;
      case 'v': NextY++;  break;
      case '<': NextX--;  break;
    }
    if (Grid[NextY][NextX] == '#')  continue;
    // Were there any boxes in the robot's path to begin with ?
    if (Grid[NextY][NextX] == '.')
    {
      // The robot occupies the spot where it wanted to go,
      //   and vacates the spot where it came from
      Grid[NextY][NextX] = '@';
      Grid[Y][X] = '.';
      Y=NextY; X=NextX;
      continue;
    }
    // For non-trivial cases, NextX and NextY have now been computed anyways

    // Non-trivial cases easy : horizontal moves
    if ((Move[MoveNr] == '>') || (Move[MoveNr] == '<'))
    {
      EndX=X;
      switch (Move[MoveNr])
      {
        case '>': IncrX = +1;  break;
        case '<': IncrX = -1;  break;
      }
      for (EndX=NextX; ((Grid[Y][EndX]=='[') || (Grid[Y][EndX]==']')); EndX+=IncrX);
      // If the row of boxes is being pushed into a wall, nothing moves
      if (Grid[Y][EndX] == '#')  continue;
      // The whole row of boxes moves by one position
      while (EndX != NextX)
      {
        Grid[Y][EndX] = Grid[Y][EndX-IncrX];
        EndX-=IncrX;
      }
      // The robot occupies the spot where it wanted to go,
      //   and vacates the spot where it came from
      Grid[Y][NextX] = '@';
      Grid[Y][X] = '.';
      X=NextX;
      continue;
    }

    // Non-trivial cases hard : vertical moves
    switch (Move[MoveNr])
    {
      case '^': IncrY = -1;  break;
      case 'v': IncrY = +1;  break;
    }
    // Compile a tree of boxes that will be affected by this push.
    // Start the queue with the one box immediately adjacent to the robot.
    QueueX[0] = (Grid[NextY][X]=='[' ? X : X-1);  QueueY[0] = NextY;
    ReadPtr = 0;  WritePtr = 1;
    int WallDetected = 0;
    // Keep expanding the tree
    while (ReadPtr < WritePtr)
    {
      NextX=QueueX[ReadPtr]; NextY=QueueY[ReadPtr]+IncrY;
      // A box could push against boxes in 3 positions within the next row
      // It could also be locked against a wall in 2 positions in next row
      if ((Grid[NextY][NextX] == '#') || (Grid[NextY][NextX+1] == '#'))
      {
        WallDetected = 1;
        break;
      }
      if (Grid[NextY][NextX] == '[')
        AddToQueue(NextX, NextY);
      else if (Grid[NextY][NextX] == ']')
        AddToQueue(NextX-1, NextY);
      if (Grid[NextY][NextX+1] == '[')
        AddToQueue(NextX+1, NextY);
      ReadPtr++;
    }

#ifdef DEBUG
    printf("[X=%d,Y=%d] %s ReadPtr=%d WritePtr=%d", X,Y, (WallDetected?"*WALL*":""), ReadPtr,WritePtr);
    for (Index=0; Index<WritePtr; Index++)  printf("  [X=%d,Y=%d]", QueueX[Index], QueueY[Index]);
    putchar('\n');
#endif

    // Was a wall detected ?
    if (WallDetected)  continue;
    // If not, move all boxes within the tree list, from last back to first
    while (WritePtr)
    {
      NextX=QueueX[WritePtr-1]; NextY=QueueY[WritePtr-1];
      // Occupy a new box position
      Grid[NextY+IncrY][NextX] = '[';  Grid[NextY+IncrY][NextX+1] = ']';
      // Vacate the existing box position
      Grid[NextY][NextX] = '.';  Grid[NextY][NextX+1] = '.';
      WritePtr--;
    }
    // And then move the robot itself
    Grid[Y][X] = '.';
    Y += IncrY;
    Grid[Y][X] = '@';
  } /* for (MoveNr) */

  ///////////////////////////////    O u t p u t    /////////////////////////////

  // Compute box positions
	int Sum=0;
  for (Y=0; Y<GridYLen; Y++)
  for (X=0; X<GridXLen; X++)
    if (Grid[Y][X] == '[')
      Sum += (Y*100 + X);
  
  printf("Found Sum = %d\n", Sum);
  return 0;
}
