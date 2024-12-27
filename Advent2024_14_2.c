#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Tiff.h"

//#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_14_2 <.dat file>\n");
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

  FILE* OutputFile;
  char OutputName[100];
  unsigned char* OutputImage = (unsigned char*)malloc(InputXLen * InputYLen * sizeof(unsigned char));
  int Turn,Advance = 1;
  for (Turn=0; Turn<101*103; Turn+=Advance)
  {

  ///////////////////////////////    O u t p u t    /////////////////////////////

    sprintf(OutputName, "Adv_14_2.%04d.tif", Turn);
    if (!(OutputFile = fopen(OutputName, "wb")))
    {
      fprintf(stderr, "Could not open %s for binary writing.\n", OutputName);
      exit(5);
    }
    WriteTiffHeader(OutputFile, InputXLen, InputYLen, 1, TIFF_LZW_COMPRESSION);
    // Populate OutputImage
    memset(OutputImage, 0, InputXLen*InputYLen);
    for (InputNr=0; InputNr<InputLineNr; InputNr++)
      OutputImage[PosY[InputNr]*InputXLen + PosX[InputNr]] = 0xFF;
    WriteTiffCompressed(OutputFile, OutputImage, InputXLen, InputYLen, 1, TIFF_LZW_COMPRESSION);
    fclose(OutputFile);

  ////////////////////////////    O p e r a t i o n    //////////////////////////

    for (InputNr=0; InputNr<InputLineNr; InputNr++)
    {
      // Predict where this robot will be after 100 turns
      PosX[InputNr] += (Advance * (SpeedX[InputNr] + InputXLen));   PosX[InputNr] %= InputXLen;
      PosY[InputNr] += (Advance * (SpeedY[InputNr] + InputYLen));   PosY[InputNr] %= InputYLen;
    } /* for (InputNr) */

  } /* for (Turn) */

  return 0;
}
