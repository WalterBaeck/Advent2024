#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_21_1 <.dat file>\n");
    exit(1);
  }

  FILE* InputFile;
  char InputLine[100];
  int InputLineNr=0;
  char Seq[4][100], ThisSeq[100], NextSeq[100];
  int SeqNr, ThisPos, ThisLen, NextPos;
  int MulSum = 0;

  int X,Y, Num;
  int cNumPadX[11], cNumPadY[11];
  int cDirPadX[5], cDirPadY[5];

  // List NumPad coordinates
  for (Y=0; Y<3; Y++)
    for (X=0; X<3; X++)
    {
      Num = 3*(2-Y)+X+1;
      if ((Num>0) && (Num<10))
      {
        cNumPadX[Num] = X;  cNumPadY[Num] = Y;
      }
    }
  // Manual adjustments to this scheme : NumPad 0 and A
  cNumPadX[ 0] = 1;  cNumPadY[ 0] = 3;
  cNumPadX[10] = 2;  cNumPadY[10] = 3;

  // List DirPad coordinates
  cDirPadX[0] = 1;  cDirPadY[0] = 0;
  cDirPadX[1] = 2;  cDirPadY[1] = 1;
  cDirPadX[2] = 1;  cDirPadY[2] = 1;
  cDirPadX[3] = 0;  cDirPadY[3] = 1;
  cDirPadX[4] = 2;  cDirPadY[4] = 0;

  ////////////////////////////////    I n p u t    //////////////////////////////

  if (!(InputFile = fopen(Argument[1], "r")))
  {
    fprintf(stderr, "Could not open %s for reading.\n", Argument[1]);
    exit(2);
  }
  // Get each Sequence
	while (fgets(InputLine, 1000, InputFile))
	{
    InputLineNr++;
		// Strip terminating newline
		while ((InputLine[strlen(InputLine)-1] == '\n') || (InputLine[strlen(InputLine)-1] == '\r'))
			InputLine[strlen(InputLine)-1] = '\0';
    strcpy(Seq[0], strtok(InputLine, ": "));

  ////////////////////////////    O p e r a t i o n    //////////////////////////

    int DiffX,DiffY, IncrX,IncrY, Step, Target;
    int Dir, Yfirst;
    int Numeric;

    for (SeqNr=0; SeqNr<3; SeqNr++)
    {
      // Express this sequence in terms of the next Pad
      strcpy(ThisSeq, Seq[SeqNr]);
      ThisLen = strlen(ThisSeq);
      NextPos = 0;
      // Always start at the A button
      if (SeqNr)
      {  X = cDirPadX[ 4];   Y = cDirPadY[ 4]; }
      else
      {  X = cNumPadX[10];   Y = cNumPadY[10]; }

#ifdef DEBUG
      printf("ThisSeq[%d]=%s starting from [Y=%d,X=%d]\n", SeqNr, ThisSeq, Y,X);
#endif

      for (ThisPos=0; ThisPos<ThisLen; ThisPos++)
      {
        // Determine which key we'd like to press
        if (SeqNr)
          switch (ThisSeq[ThisPos])
          {
            case '^': Target = 0;  break;
            case '>': Target = 1;  break;
            case 'v': Target = 2;  break;
            case '<': Target = 3;  break;
            case 'A': Target = 4;  break;
          }
        else /* !SeqNr */
        {
          Target = ThisSeq[ThisPos] - '0';
          if (ThisSeq[ThisPos] == 'A')
            Target = 10;
        }
        // Determine how to get there
        if (SeqNr)
        { DiffX = cDirPadX[Target]-X;  DiffY = cDirPadY[Target]-Y; }
        else
        { DiffX = cNumPadX[Target]-X;  DiffY = cNumPadY[Target]-Y; }
        IncrX = (DiffX < 0 ? -1 : 1);  IncrY = (DiffY < 0 ? -1 : 1);

#ifdef DEBUG
        printf("  Need to move DiffX=%d over IncrX=%d | DiffY=%d over IncrY=%d\n",
            DiffX, IncrX, DiffY, IncrY);
#endif

        // Determine in which order the directions will be traversed.
        Yfirst = 0;
        // Override: the one empty space on the keypad must be avoided
        if ((!SeqNr) && (Y==3) && (X+DiffX == 0))  Yfirst=1;
        if ( (SeqNr) && (Y==0) && (X+DiffX == 0))  Yfirst=1;

        for (Dir=0; Dir<2; Dir++)
          if (((Yfirst) && (!Dir)) || ((!Yfirst) && (Dir)))
            for (Step=0; Step!=DiffY; Step+=IncrY)
              NextSeq[NextPos++] = (IncrY<0 ? '^' : 'v');
          else
            for (Step=0; Step!=DiffX; Step+=IncrX)
              NextSeq[NextPos++] = (IncrX<0 ? '<' : '>');

        // So, we've gotten there now
        X += DiffX;  Y += DiffY;

#ifdef DEBUG
        printf("    Arrived at [Y=%d,X=%d] with NextPos=%d\n", Y,X, NextPos);
#endif

        // And finally, push this key
        NextSeq[NextPos++] = 'A';
      } /* for (ThisPos) */

      // Close the NextSeq string ..
      NextSeq[NextPos] = '\0';
      // .. and copy it into its place
      strcpy(Seq[SeqNr+1], NextSeq);

#ifdef DEBUG
      printf("From ThisSeq[%d]=%s to NextSeq[%d]=%s\n", SeqNr, ThisSeq, SeqNr+1, NextSeq);
#endif
    } /* for (SeqNr) */

    // Blank out the last A from the task sequence
    Seq[0][strlen(Seq[0])-1] = '\0';
    if (1 != sscanf(Seq[0], "%d", &Numeric))
    {
      fprintf(stderr, "Could not scan Numeric from %s\n", Seq[0]);
      exit(3);
    }
    printf("Manual sequence length %d to obtain number %d gives Mul = %d\n",
        strlen(Seq[3]), Numeric, strlen(Seq[3])*Numeric);
    MulSum += strlen(Seq[3])*Numeric;

	} /* while (fgets) */
	fclose(InputFile);
	printf("%d inputlines were read.\n", InputLineNr);

  ///////////////////////////////    O u t p u t    /////////////////////////////

  printf("Found MulSum = %d\n", MulSum);
  return 0;
}
