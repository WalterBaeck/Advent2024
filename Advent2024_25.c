#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_25 <.dat file>\n");
    exit(1);
  }

  FILE* InputFile;
  char InputLine[10];
  int InputLineNr=0;
	char Input[10][10];
	int InputX,InputY=0, InputXLen=0, InputYLen=0;

  int LockNr, NrOfLocks=0, KeyNr, NrOfKeys=0;
  int Lock[1000][5], Key[1000][5];

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
    if (strlen(InputLine))
    {
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
      continue;
    }
    // Encountered a blank line : process this Grid
    if (!InputYLen)
      InputYLen = InputY;
    else if (InputYLen != InputY)
    {
      fprintf(stderr, "Found Grid height %d after InputLine #%d, established was %d\n",
          InputY, InputLineNr, InputYLen);
      exit(3);
    }
    if ((!strcmp(Input[0], "#####")) && (!strcmp(Input[6], ".....")))
    {
      // Process this Lock
      for (InputX=0; InputX<5; InputX++)
        // sample downwards from top row
        for (Lock[NrOfLocks][InputX]=0; Lock[NrOfLocks][InputX]<5; Lock[NrOfLocks][InputX]++)
          if (Input[Lock[NrOfLocks][InputX]+1][InputX] != '#')  break;
      NrOfLocks++;
    }
    else if ((!strcmp(Input[0], ".....")) && (!strcmp(Input[6], "#####")))
    {
      // Process this Key
      for (InputX=0; InputX<5; InputX++)
        // sample upwards from bottom row
        for (Key[NrOfKeys][InputX]=0; Key[NrOfKeys][InputX]<5; Key[NrOfKeys][InputX]++)
          if (Input[5-Key[NrOfKeys][InputX]][InputX] != '#')  break;
      NrOfKeys++;
    }
    else
    {
      fprintf(stderr, "Unrecognized Grid after InputLine #%d\n", InputLineNr);
      exit(3);
    }
    // Prepare for a new Grid
    InputY = 0;
	} /* while (fgets) */
	fclose(InputFile);
	printf("%d inputlines were read of height %d x width %d.\n", InputLineNr, InputYLen, InputXLen);
  printf("Found %d Locks and %d Keys\n", NrOfLocks, NrOfKeys);

  ////////////////////////////    O p e r a t i o n    //////////////////////////

  int Sum=0;
  for (LockNr=0; LockNr<NrOfLocks; LockNr++)
    for (KeyNr=0; KeyNr<NrOfKeys; KeyNr++)
    {
      for (InputX=0; InputX<5; InputX++)
        if (Lock[LockNr][InputX] + Key[KeyNr][InputX] > 5)  break;
      if (InputX >= 5)  Sum++;
#ifdef DEBUG
      if (InputX >= 5)
        printf("Matched Lock[%d]={%d,%d,%d,%d,%d} with Key[%d]={%d,%d,%d,%d,%d}\n",
            LockNr, Lock[LockNr][0], Lock[LockNr][1], Lock[LockNr][2],
            Lock[LockNr][3], Lock[LockNr][4],
            KeyNr, Key[KeyNr][0], Key[KeyNr][1], Key[KeyNr][2], Key[KeyNr][3],
            Key[KeyNr][4]);
      else
        printf("Misfit Lock[%d]={%d,%d,%d,%d,%d} with Key[%d]={%d,%d,%d,%d,%d}\n",
            LockNr, Lock[LockNr][0], Lock[LockNr][1], Lock[LockNr][2],
            Lock[LockNr][3], Lock[LockNr][4],
            KeyNr, Key[KeyNr][0], Key[KeyNr][1], Key[KeyNr][2], Key[KeyNr][3],
            Key[KeyNr][4]);
#endif
    }

  ///////////////////////////////    O u t p u t    /////////////////////////////

	printf("Found Sum %d\n", Sum);
  return 0;
}
