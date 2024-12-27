#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NR_ROBOTS 4
#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_21_2 <.dat file>\n");
    exit(1);
  }

  FILE* InputFile;
  char InputLine[100];
  int InputLineNr=0;
  char Digit[5];
  int DigitNr;
  char *Seq[NR_ROBOTS][200], ThisSeq[10000], NextSeq[10000];
  int SeqNr, NrOfSeq[NR_ROBOTS], ThisPos, ThisLen, NextPos;
  int MulSum = 0;

  for (SeqNr=0; SeqNr<NR_ROBOTS; SeqNr++)
    for (DigitNr=0; DigitNr<200; DigitNr++)
      if (!(Seq[SeqNr][DigitNr] = (char*)malloc(10000)))
      {
        fprintf(stderr, "Could not allocate 10K chars for Seq[%d][%d]\n", SeqNr, DigitNr);
        exit(5);
      }

  int X,Y, Num;
  int cNumPadX[11], cNumPadY[11];
  int cDirPadX[ 5], cDirPadY[ 5];

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

  // // // // // Helper function // // // // //
  int ScoreFromTo(char From, char To)
  {
    int FromX,FromY, ToX,ToY;
    switch (From)
    {
      case '^': FromX = cDirPadX[0];  FromY = cDirPadY[0];  break;
      case '>': FromX = cDirPadX[1];  FromY = cDirPadY[1];  break;
      case 'v': FromX = cDirPadX[2];  FromY = cDirPadY[2];  break;
      case '<': FromX = cDirPadX[3];  FromY = cDirPadY[3];  break;
      case 'A': FromX = cDirPadX[4];  FromY = cDirPadY[4];  break;
    }
    switch (To)
    {
      case '^': ToX = cDirPadX[0];  ToY = cDirPadY[0];  break;
      case '>': ToX = cDirPadX[1];  ToY = cDirPadY[1];  break;
      case 'v': ToX = cDirPadX[2];  ToY = cDirPadY[2];  break;
      case '<': ToX = cDirPadX[3];  ToY = cDirPadY[3];  break;
      case 'A': ToX = cDirPadX[4];  ToY = cDirPadY[4];  break;
    }
    return (ToX<FromX ? FromX-ToX : ToX-FromX) + (ToY<FromY ? FromY-ToY : ToY-FromY);
  } /* int ScoreFromTo(); */

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
    strcpy(Digit, strtok(InputLine, ": "));

  ////////////////////////////    O p e r a t i o n    //////////////////////////

  int ThisSeqNr, NextSeqNr, PrevSeqNr;
  int Target;
  int StartX,StartY, EndX,EndY, DiffX,DiffY, IncrX,IncrY, PrevX,PrevY;
  int Yfirst, Tactic, Dir, Step;
  int Score, MinScore, MinLength, TotalLength;

  TotalLength = 0;
  for (DigitNr=0; DigitNr<4; DigitNr++)
  {
    // // // // // Populate Sequence[0][] with up to 2 possible paths over NumPad // // // // //

    // What were the coordinates after previous digit ?
    if (!DigitNr)
    {  StartX = cNumPadX[10];   StartY = cNumPadY[10]; }
    else
    {  StartX = PrevX;          StartY = PrevY; }
    // Determine which key we'd like to press
    Target = Digit[DigitNr] - '0';
    if (Digit[DigitNr] == 'A')
      Target = 10;
    EndX = cNumPadX[Target];    EndY = cNumPadY[Target];
    PrevX = EndX;               PrevY = EndY;
    // So, these are the displacements we'll have to make
    DiffX = EndX - StartX;      DiffY = EndY - StartY;
    IncrX = (DiffX<0 ? -1 : 1); IncrY = (DiffY<0 ? -1 : 1);
    // There's two ways to handle this : Y travel first, or not
    NextSeqNr = 0;
    MinScore = 0;
    for (Yfirst=0; Yfirst<2; Yfirst++)
    {
      X = StartX;   Y = StartY;
      NextPos = 0;
      for (Dir=0; Dir<2; Dir++)
        if (((Yfirst) && (!Dir)) || ((!Yfirst) && (Dir)))
        {
          for (Step=0; Step!=DiffY; Step+=IncrY)
          {
            NextSeq[NextPos++] = (IncrY<0 ? '^' : 'v');
            Y += IncrY;
            if ((Y==3) && (X==0))  break;  // Don't fall into the Void
          }
          if (Step!=DiffY)  break;
        } /* Y-travel */
        else
        {
          for (Step=0; Step!=DiffX; Step+=IncrX)
          {
            NextSeq[NextPos++] = (IncrX<0 ? '<' : '>');
            X += IncrX;
            if ((Y==3) && (X==0))  break;  // Don't fall into the Void
          }
          if (Step!=DiffX)  break;
        } /* X-travel */
      if (Dir<2)
        // This Yfirst choice has made us fall into the Void
        continue;
      // After travel in both Directions, push this key
      NextSeq[NextPos++] = 'A';
      // Close this NextSeq string and save it, if Score is better
      NextSeq[NextPos] = '\0';
      for (PrevSeqNr=0; PrevSeqNr<NextSeqNr; PrevSeqNr++)
        if (!strcmp(Seq[0][PrevSeqNr], NextSeq))  break;
      if (PrevSeqNr<NextSeqNr)  continue;
      for (ThisPos=0,Score=0; ThisPos<strlen(NextSeq); ThisPos++)
        Score += ScoreFromTo(ThisPos ? NextSeq[ThisPos-1] : 'A', NextSeq[ThisPos]);
      if ((MinScore == 0) || (Score <= MinScore))
      {
        strcpy(Seq[0][NextSeqNr++], NextSeq);
        MinScore = Score;
      }
    } /* for (Yfirst) */
    NrOfSeq[0] = NextSeqNr;

#ifdef DEBUG
    printf(" +Digit %c can be reached with %d level-0 sequences:+\n", Digit[DigitNr], NrOfSeq[0]);
    for (NextSeqNr=0; NextSeqNr<NrOfSeq[0]; NextSeqNr++)
    {
      for (ThisPos=0,Score=0; ThisPos<strlen(Seq[0][NextSeqNr]); ThisPos++)
        Score += ScoreFromTo(ThisPos ? Seq[0][NextSeqNr][ThisPos-1] : 'A', Seq[0][NextSeqNr][ThisPos]);
      printf("  {%d} %s\n", Score, Seq[0][NextSeqNr]);
    }
#endif

    // // // // // Go through NR_ROBOTS-1 layers of DirPad possibilities for this Digit // // // // //

    for (SeqNr=0; SeqNr<NR_ROBOTS-1; SeqNr++)
    {
      MinScore = 0;
      NextSeqNr = 0;
      for (ThisSeqNr=0; ThisSeqNr<NrOfSeq[SeqNr]; ThisSeqNr++)
      {
        strcpy(ThisSeq, Seq[SeqNr][ThisSeqNr]);
        ThisLen = strlen(ThisSeq);
#ifdef DEBUG
        printf("   Seq[%d][%d]=%s has length %d\n", SeqNr, ThisSeqNr, ThisSeq, ThisLen);
#endif
        // Express this sequence in all possible ways
        for (Tactic=0; Tactic<(1<<ThisLen); Tactic++)
        {
          // Always start at the A button
          StartX = cDirPadX[4];   StartY = cDirPadY[4];
          NextPos = 0;
          for (ThisPos=0; ThisPos<ThisLen; ThisPos++)
          {
            // Determine which key we'd like to press
            switch (ThisSeq[ThisPos])
            {
              case '^': Target = 0;  break;
              case '>': Target = 1;  break;
              case 'v': Target = 2;  break;
              case '<': Target = 3;  break;
              case 'A': Target = 4;  break;
            }
            EndX = cDirPadX[Target];    EndY = cDirPadY[Target];
            // So, these are the displacements we'll have to make
            DiffX = EndX - StartX;      DiffY = EndY - StartY;
            IncrX = (DiffX<0 ? -1 : 1); IncrY = (DiffY<0 ? -1 : 1);
            // The Tactic has decided whether Y travel comes first at ThisPos
            Yfirst = Tactic & (1<<ThisPos);
            if (((Yfirst && (!DiffY)) || ((!Yfirst) && (!DiffX)))
                && ((DiffY) || (DiffX)))  break;
            X = StartX;   Y = StartY;
            for (Dir=0; Dir<2; Dir++)
              if (((Yfirst) && (!Dir)) || ((!Yfirst) && (Dir)))
              {
                for (Step=0; Step!=DiffY; Step+=IncrY)
                {
                  NextSeq[NextPos++] = (IncrY<0 ? '^' : 'v');
                  Y += IncrY;
                  if ((Y==0) && (X==0))  break;  // Don't fall into the Void
                }
                if (Step!=DiffY)  break;
              } /* Y-travel */
              else
              {
                for (Step=0; Step!=DiffX; Step+=IncrX)
                {
                  NextSeq[NextPos++] = (IncrX<0 ? '<' : '>');
                  X += IncrX;
                  if ((Y==0) && (X==0))  break;  // Don't fall into the Void
                }
                if (Step!=DiffX)  break;
              } /* X-travel */
            if (Dir<2)
              // This Yfirst choice has made us fall into the Void
              break;
            // After travel in both Directions, push this key
            NextSeq[NextPos++] = 'A';
            if (NextPos >= 10000)
            {
              fprintf(stderr, "NextPos>10000 for Digit[%d] Seq[%d]\n", DigitNr, SeqNr);
              exit(4);
            }
            // So, we've gotten there now
            StartX = EndX;   StartY = EndY;
          } /* for (ThisPos) */
          // Was this an aborted Tactic ?
          if (ThisPos<ThisLen)  continue;
          // Close this NextSeq string and save it, if Score is better
          NextSeq[NextPos] = '\0';
          for (PrevSeqNr=0; PrevSeqNr<NextSeqNr; PrevSeqNr++)
            if (!strcmp(Seq[SeqNr+1][PrevSeqNr], NextSeq))  break;
          if (PrevSeqNr<NextSeqNr)  continue;
          // Compute the Score of this candidate
          for (ThisPos=0,Score=0; ThisPos<strlen(NextSeq); ThisPos++)
            Score += ScoreFromTo(ThisPos ? NextSeq[ThisPos-1] : 'A', NextSeq[ThisPos]);
          if ((MinScore == 0) || (Score < MinScore))
          {
            strcpy(Seq[SeqNr+1][0], NextSeq);
            NextSeqNr = 1;
            MinScore = Score;
          }
          else if (Score == MinScore)
          {
            strcpy(Seq[SeqNr+1][NextSeqNr++], NextSeq);
            if (NextSeqNr >= 200)
            {
              fprintf(stderr, "More than 200 NextSeqNr for Digit[%d] Seq[%d]\n", DigitNr, SeqNr);
              exit(4);
            }
          }
        } /* for (Tactic) */
      } /* for (ThisSeqNr) */
      NrOfSeq[SeqNr+1] = NextSeqNr;

#ifdef DEBUG
      printf("  -Found %d best level-%d sequences:-\n", NrOfSeq[SeqNr+1], SeqNr+1);
      for (NextSeqNr=0; NextSeqNr<NrOfSeq[SeqNr+1]; NextSeqNr++)
      {
        // Compute the Score of this candidate
        for (ThisPos=0,Score=0; ThisPos<strlen(Seq[SeqNr+1][NextSeqNr]); ThisPos++)
          Score += ScoreFromTo(ThisPos ? Seq[SeqNr+1][NextSeqNr][ThisPos-1] : 'A',
                                         Seq[SeqNr+1][NextSeqNr][ThisPos]);
        printf("  {%3d} %s\n", Score, Seq[SeqNr+1][NextSeqNr]);
    }
#endif

    } /* for (SeqNr) */
    int MinLength = 0;
    for (NextSeqNr=0; NextSeqNr<NrOfSeq[NR_ROBOTS-1]; NextSeqNr++)  
      if ((!MinLength) || (strlen(Seq[NR_ROBOTS-1][NextSeqNr]) < MinLength))
        MinLength = strlen(Seq[NR_ROBOTS-1][NextSeqNr]);

#ifdef DEBUG
    printf("Digit[%d]=%c can be pressed with MinLength=%d\n", DigitNr, Digit[DigitNr], MinLength);
#endif

    TotalLength += MinLength;
  } /* for (DigitNr) */

  ///////////////////////////////    O u t p u t    /////////////////////////////

  int Numeric = 100*(Digit[0]-'0') + 10*(Digit[1]-'0') + Digit[2]-'0';
  printf("Manual sequence length %d to obtain number %d gives Mul = %d\n",
      TotalLength, Numeric, TotalLength*Numeric);
  MulSum += TotalLength*Numeric;

	} /* while (fgets) */
	fclose(InputFile);
	printf("%d inputlines were read.\n", InputLineNr);

  printf("Found MulSum = %d\n", MulSum);
  return 0;
}
