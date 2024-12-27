#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_14_1 <.dat file>\n");
    exit(1);
  }

  FILE* InputFile;
  char InputLine[1000];
  int InputNr=0,InputLineNr = 0;
	int PosX[500],PosY[500],SpeedX[500],SpeedY[500];
	int InputXLen,InputYLen;

  ////////////////////////////////    I n p u t    //////////////////////////////

  if (!strncmp(Argument[1]+strlen(Argument[1])-4, ".dat", 4))
  {
    InputXLen = 101;  InputYLen = 103;
  }
  else
  {
    InputXLen = 11;   InputYLen = 7;
  }
  if (!(InputFile = fopen(Argument[1], "r")))
  {
    fprintf(stderr, "Could not open %s for reading.\n", Argument[1]);
    exit(2);
  }
	while (fgets(InputLine, 1000, InputFile))
	{
    if (4 != sscanf(InputLine, "p=%d,%d v=%d,%d",
            &PosX[InputNr], &PosY[InputNr], &SpeedX[InputNr], &SpeedY[InputNr]))
    {
      fprintf(stderr, "Could not scan 4 numbers from InputLine #%d:\n%s",
          InputLineNr, InputLine);
      exit(3);
    }
#ifdef DEBUG
    printf("Robot[%3d] at Pos[X=%d,Y=%d] and Speed[X=%d,Y=%d]\n",
        InputNr, PosX[InputNr], PosY[InputNr], SpeedX[InputNr], SpeedY[InputNr]);
#endif
    InputNr++;
		if (++InputLineNr > 500)
    {
      fprintf(stderr, "More than 500 InputLines\n");
      exit(4);
    }
	} /* while (fgets) */
	fclose(InputFile);
	printf("%d inputlines were read.\n", InputLineNr);

  ////////////////////////////    O p e r a t i o n    //////////////////////////

  int Quad[4]={0};
  for (InputNr=0; InputNr<InputLineNr; InputNr++)
  {
    // Predict where this robot will be after 100 turns
    PosX[InputNr] += (100 * (SpeedX[InputNr] + InputXLen));   PosX[InputNr] %= InputXLen;
    PosY[InputNr] += (100 * (SpeedY[InputNr] + InputYLen));   PosY[InputNr] %= InputYLen;
    // Determine quadrant where this will be
    if ((PosX[InputNr] < InputXLen/2) && (PosY[InputNr] < InputYLen/2))  Quad[0]++;
    if ((PosX[InputNr] > InputXLen/2) && (PosY[InputNr] < InputYLen/2))  Quad[1]++;
    if ((PosX[InputNr] < InputXLen/2) && (PosY[InputNr] > InputYLen/2))  Quad[2]++;
    if ((PosX[InputNr] > InputXLen/2) && (PosY[InputNr] > InputYLen/2))  Quad[3]++;
#ifdef DEBUG
    printf("Robot[%3d] ends up at [X=%d,Y=%d]\n", InputNr, PosX[InputNr], PosY[InputNr]);
#endif
  } /* for (InputNr) */
  int Mul = Quad[0] * Quad[1] * Quad[2] * Quad[3];

  ///////////////////////////////    O u t p u t    /////////////////////////////

  printf("Found Mul = %d\n", Mul);
  return 0;
}
