#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NR_ROBOTS 8
#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_21_2b <.dat file>\n");
    exit(1);
  }

  FILE* InputFile;
  char InputLine[100];
  int InputLineNr=0;
  char Digit[5];
  int DigitNr;
  char Seq0[2][10], NextSeq[10];
  int SeqNr, NextPos, ThisPos, ThisLen;
  unsigned long long MulSum = 0;

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

  typedef struct {
    char Str[7];
    int Score;
    int Len;
  } tAct;

  typedef struct {
    int Act[2];
    int NrOfAct;
  } tChain;

  typedef struct {
    tChain* Chain;
    unsigned long long NrOfChain;
    unsigned long long Score;
    unsigned long long Len;
  } tSolution;

  tAct Act[100];  // Limited pool of possible Acts
  tSolution EvolveAct[100];
  int ActNr, NrOfAct=0, NewActNr, NrOfEvolve;

  for (NrOfEvolve=0; NrOfEvolve<100; NrOfEvolve++)
    EvolveAct[NrOfEvolve].NrOfChain = 0;
  NrOfEvolve = 0;

  tAct *ActPtr;
  tChain* ChainPtr;
  unsigned long long ChainNr;
  tSolution *Current=NULL, *Next=NULL;
  tSolution *CurrentPtr, *NextPtr;
  int CurrentNr, NrOfCurrent=0, NextNr, NrOfNext=0;

  // // // // // Helper functions // // // // //
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

  int AddAct(char* String)
  {
    int Nr;
    for (Nr=0; Nr<NrOfAct; Nr++)
      if (!strcmp(Act[Nr].Str, String))
        return Nr;
    int Pos,Len,Score;
    strcpy(Act[NrOfAct].Str, String);
    Len = strlen(String);
    Act[NrOfAct].Len = Len;
    for (Pos=0,Score=0; Pos<Len; Pos++)
      Score += ScoreFromTo(Pos ? String[Pos-1] : 'A', String[Pos]);
    Act[NrOfAct].Score = Score;
    if (NrOfAct++ >= 100)
    {
      fprintf(stderr, "Needed to create more than 100 Acts for new Act %s\n", String);
      exit(5);
    }
  } /* int AddAct() */

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

  int NextSeqNr, PrevSeqNr;
  int Target;
  int StartX,StartY, EndX,EndY, DiffX,DiffY, IncrX,IncrY, PrevX,PrevY;
  int Yfirst, Tactic, Dir, Step;
  int Score0, MinScore0;
  unsigned long long MinScore, MinLength, TotalLength;

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
    MinScore0 = 0;
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
        if (!strcmp(Seq0[PrevSeqNr], NextSeq))  break;
      if (PrevSeqNr<NextSeqNr)  continue;
      for (ThisPos=0,Score0=0; ThisPos<strlen(NextSeq); ThisPos++)
        Score0 += ScoreFromTo(ThisPos ? NextSeq[ThisPos-1] : 'A', NextSeq[ThisPos]);
      if ((MinScore0 == 0) || (Score0 <= MinScore0))
      {
        strcpy(Seq0[NextSeqNr++], NextSeq);
        MinScore0 = Score0;
      }
    } /* for (Yfirst) */

    // // // // // Start out with {Current Solutions} = Seq[0] // // // // //
    NrOfCurrent = 0;  NrOfNext = 0;

#ifdef DEBUG
    printf(" +Digit %c can be reached with %d level-0 sequences:+\n", Digit[DigitNr], NextSeqNr);
#endif
    for (NrOfCurrent=0; NrOfCurrent<NextSeqNr; NrOfCurrent++)
    {
      // Create a new Solution for this Seq[0] member
      if (!(Current = (tSolution*)realloc(Current, (NrOfCurrent+1)*sizeof(tSolution))))
      {
        fprintf(stderr, "Could not get %d Solutions realloced\n", NrOfCurrent+1);
        exit(5);
      }
      CurrentPtr = Current + NrOfCurrent;
      // New Solution starts out with just one Chainlink, create it
      CurrentPtr->NrOfChain = 1;
      if (!(CurrentPtr->Chain = (tChain*)malloc(sizeof(tChain))))
      {
        fprintf(stderr, "Could not malloc 1 Chain for Current[%d]\n", NrOfCurrent);
        exit(5);
      }
      ChainPtr = CurrentPtr->Chain;
      // This Chainlink has just one Act, create it in the pool
      ChainPtr->NrOfAct = 1;
      ChainPtr->Act[0] = AddAct(Seq0[NrOfCurrent]);
      ActPtr = Act + ChainPtr->Act[0];
      // This new Chainlink is now complete, copy its qualities into this new Solution
      CurrentPtr->Score = ActPtr->Score;
      CurrentPtr->Len   = ActPtr->Len;
#ifdef DEBUG
      printf("  {%d} %s = Act[%2d]\n", ActPtr->Score, ActPtr->Str, ChainPtr->Act[0]);
#endif
    }

    // // // // // Go through NR_ROBOTS-1 layers of DirPad possibilities for this Digit // // // // //

    for (SeqNr=0; SeqNr<NR_ROBOTS-1; SeqNr++)
    {
      // We're coming in with an array of Current Solutions that have all been flattened:
      //   each Chainlink has only 1 Act.
      // From each Current Solution, we're deriving *one* Next Solution (at SeqNr+1)
      //   which can be bloated: some of its Chainlinks may have 2 Acts.
      // When the Next solution doesn't meet the MinScore, we don't store it.
      // As each character at SeqNr evolves into a full Act at SeqNr+1,
      //   the Next Solution will always have NrOfChain equal to Current->Len
      MinScore = 0;
      NrOfNext = 0;
      Next = (tSolution*)realloc(Next, sizeof(tSolution));
      NextPtr = Next;
      NextPtr->NrOfChain = Current[0].Len;
      NextPtr->Chain = (tChain*)malloc(NextPtr->NrOfChain * sizeof(tChain));

      for (CurrentNr=0,CurrentPtr=Current; CurrentNr<NrOfCurrent; CurrentNr++,CurrentPtr++)
      {

#ifdef DEBUG
        printf("  From SeqNr[%d] Current[%d] {%llu} with Len %llu NrOfChain %llu\n",
            SeqNr, CurrentNr, CurrentPtr->Score, CurrentPtr->Len, CurrentPtr->NrOfChain);
#endif

        // Start building a candidate Next Solution
        tChain* NextChainPtr = NextPtr->Chain;
        NextPtr->Score = 0;   NextPtr->Len = 0;
      for (ChainNr=0,ChainPtr=CurrentPtr->Chain; ChainNr<CurrentPtr->NrOfChain; ChainNr++,ChainPtr++)
      {
        // Within this particular Chain of the Current Solution, we're looking at a single Act.
        ActNr = ChainPtr->Act[0];
        // When this Act has no known EvolveAct yet, we need to evolve it now
        if (!EvolveAct[ActNr].NrOfChain)
        {
          ActPtr = Act + ActNr;
          ThisLen = ActPtr->Len;
          EvolveAct[ActNr].NrOfChain = ThisLen;
          EvolveAct[ActNr].Chain = (tChain*)malloc(ThisLen * sizeof(tChain));
          EvolveAct[ActNr].Score = 0;
          EvolveAct[ActNr].Len   = 0;
#ifdef DEBUG
          printf("   Seq[%d][%d][%llu]=%s has length %d\n",
              SeqNr, CurrentNr, ChainNr, ActPtr->Str, ThisLen);
#endif
          // Express this Act in a series of evolved Acts
          // Always start at the A button
          StartX = cDirPadX[4];   StartY = cDirPadY[4];
          for (ThisPos=0; ThisPos<ThisLen; ThisPos++)
          {
            EvolveAct[ActNr].Chain[ThisPos].NrOfAct = 0;
            // Determine which key we'd like to press
            switch (ActPtr->Str[ThisPos])
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
            for (Yfirst=0; Yfirst<2; Yfirst++)
            {
              if ((Yfirst && (!DiffY) && DiffX) || ((!Yfirst) && (!DiffX)))  continue;
              X = StartX;   Y = StartY;
              NextPos = 0;
              for (Dir=0; Dir<2; Dir++)
                if (((Yfirst) && (!Dir)) || ((!Yfirst) && (Dir)))
                {
                  for (Step=0; Step!=DiffY; Step+=IncrY)
                  {
                    NextSeq[NextPos++] = (IncrY<0 ? '^' : 'v');
                    Y += IncrY;
                    if ((Y==0) && (X==0))  break;  // Don't fall into the Void
                  }
                  if (Step!=DiffY)  break;  // Voidfall
                } /* Y-travel */
                else
                {
                  for (Step=0; Step!=DiffX; Step+=IncrX)
                  {
                    NextSeq[NextPos++] = (IncrX<0 ? '<' : '>');
                    X += IncrX;
                    if ((Y==0) && (X==0))  break;  // Don't fall into the Void
                  }
                  if (Step!=DiffX)  break;  // Voidfall
                } /* X-travel */
              if (Dir<2)
                // This Yfirst choice has made us fall into the Void
                continue;
              // After travel in both Directions, push this key
              NextSeq[NextPos++] = 'A';
              NextSeq[NextPos] = '\0';
              // NextSeq is now a valid Act to store within the evolve Chainlink
              NewActNr = AddAct(NextSeq);

#ifdef DEBUG
              printf("    ThisPos[%d] found NewAct[%2d] %s {%d}\n",
                  ThisPos, NewActNr, Act[NewActNr].Str, Act[NewActNr].Score);
              if (EvolveAct[ActNr].Chain[ThisPos].NrOfAct)
              {
                printf("     Chain[%d] had NrOfAct=%d", ThisPos,
                    EvolveAct[ActNr].Chain[ThisPos].NrOfAct);
                printf(" with [0]=%2d %s {%d}\n", EvolveAct[ActNr].Chain[ThisPos].Act[0],
                    Act[EvolveAct[ActNr].Chain[ThisPos].Act[0]].Str,
                    Act[EvolveAct[ActNr].Chain[ThisPos].Act[0]].Score);
              }
#endif

              // Don't store if it's weaker than existing Act in this Chainlink
              if ((EvolveAct[ActNr].Chain[ThisPos].NrOfAct) &&
                  (Act[EvolveAct[ActNr].Chain[ThisPos].Act[0]].Score < Act[NewActNr].Score))  break;
              // Overwrite the previous Act in this Chainlink if this is better
              if ((EvolveAct[ActNr].Chain[ThisPos].NrOfAct) &&
                  (Act[EvolveAct[ActNr].Chain[ThisPos].Act[0]].Score > Act[NewActNr].Score))
                EvolveAct[ActNr].Chain[ThisPos].NrOfAct = 0;
              EvolveAct[ActNr].Chain[ThisPos].Act[EvolveAct[ActNr].Chain[ThisPos].NrOfAct++] = NewActNr;
            } /* for (Yfirst) */
            EvolveAct[ActNr].Score += Act[EvolveAct[ActNr].Chain[ThisPos].Act[0]].Score;
            EvolveAct[ActNr].Len   += Act[EvolveAct[ActNr].Chain[ThisPos].Act[0]].Len;
            // So, we've gotten there now - on to the next char in the Act to be evolved
            StartX = EndX;   StartY = EndY;
          } /* for (ThisPos) */
        } /* if (!EvolveAct) */

#ifdef DEBUG
        printf("    Copying EvolveAct[%2d] with NrOfChain=%llu {%d} Len=%llu into Next[%d] at Chain[%llu]\n",
            ActNr, EvolveAct[ActNr].NrOfChain, EvolveAct[ActNr].Score, EvolveAct[ActNr].Len,
            NextPtr-Next, NextChainPtr-NextPtr->Chain);
#endif

        // This Act out of the Current Solution has an EvolveAct,
        //   that needs to be copied into the Next Solution now.
        int NextChainNr;
        for (NextChainNr=0; NextChainNr<EvolveAct[ActNr].NrOfChain; NextChainNr++)
          memcpy(NextChainPtr++, EvolveAct[ActNr].Chain+NextChainNr, sizeof(tChain));
        NextPtr->Score += EvolveAct[ActNr].Score;
        NextPtr->Len   += EvolveAct[ActNr].Len;
      } /* for (ChainNr) */

#ifdef DEBUG
      printf("   Next[%d] {%llu} len %llu NrOfChain %llu\n",
          NextPtr-Next, NextPtr->Score, NextPtr->Len, NextPtr->NrOfChain);
#endif

        // All Chainlinks of the Current Solution have been evolved.
        // This means that the candidate Next Solution is complete.
        // However, this candidate may perform different from known Next Solutions :
        if ((MinScore == 0) || (NextPtr->Score < MinScore))
        {
          // This candidate becomes the only one in the Next array
          if (Next != NextPtr)
          {
            free(Next->Chain);
            memcpy(Next, NextPtr, sizeof(tSolution));
            for (NextNr=1,NextPtr=Next+1; NextNr<NrOfNext; NextNr++,NextPtr++)
              free(NextPtr->Chain);
          }
          NrOfNext = 1;
          MinScore = NextPtr->Score;
        }
        else if (NextPtr->Score == MinScore)
          // Store this candidate equally behind others in the Next array
          NrOfNext++;
        else  // NextPtr->Score > MinScore
          continue;
        // The Next array needs to be adjusted to a new size
        Next = (tSolution*)realloc(Next, (NrOfNext+1)*sizeof(tSolution));
        NextPtr = Next + NrOfNext;
        NextPtr->NrOfChain = Current[0].Len;
        NextPtr->Chain = (tChain*)malloc(NextPtr->NrOfChain * sizeof(tChain));
      } /* for (CurrentNr) */

      // Each flattened Current Solution has now evolved into a bloated Next Solution.
      //   As we don't need the Current Solution array anymore, disband it
      for (CurrentNr=0,CurrentPtr=Current; CurrentNr<NrOfCurrent; CurrentNr++,CurrentPtr++)
        free(CurrentPtr->Chain);
      NrOfCurrent=0;
      // Now copy the Next array over into the Current array, but flatten it
      for (NextNr=0,NextPtr=Next; NextNr<NrOfNext; NextNr++,NextPtr++)
      {
        int SplitChainNr,NrOfSplitChains=0;
        for (ChainNr=0,ChainPtr=NextPtr->Chain; ChainNr<NextPtr->NrOfChain; ChainNr++,ChainPtr++)
        {
          if ((ChainPtr->NrOfAct > 1) &&
              (EvolveAct[ChainPtr->Act[0]].NrOfChain) && (EvolveAct[ChainPtr->Act[1]].NrOfChain))
          {
            // Force a choice between both Acts by considering how they'll evolve
            if (EvolveAct[ChainPtr->Act[0]].Score > EvolveAct[ChainPtr->Act[1]].Score)
              ChainPtr->Act[0] = ChainPtr->Act[1];
            ChainPtr->NrOfAct = 1;
#ifdef DEBUG
            if (ChainPtr->NrOfAct == 1)
              printf("    Reduced bloated Chain[%d][%d][%llu] to Act[%2d] %s\n",
                  SeqNr, NextNr, ChainNr, ChainPtr->Act[0], Act[ChainPtr->Act[0]].Str);
#endif
          }
          NrOfSplitChains += (ChainPtr->NrOfAct-1);
        }

#ifdef DEBUG
        printf("   Next[%d] NrOfChain %llu NrOfSplitChains %d\n",
            NextNr, NextPtr->NrOfChain, NrOfSplitChains);
#endif

        for (Tactic=0; Tactic<(1<<NrOfSplitChains); Tactic++)
        {
          SplitChainNr = 0;
          Current = (tSolution*)realloc(Current, (++NrOfCurrent)*sizeof(tSolution));
          CurrentPtr = Current + NrOfCurrent-1;
          CurrentPtr->Chain = (tChain*)malloc((NextPtr->NrOfChain) * sizeof(tChain));
          CurrentPtr->NrOfChain = NextPtr->NrOfChain;
          CurrentPtr->Score     = NextPtr->Score;
          CurrentPtr->Len       = NextPtr->Len;
          for (ChainNr=0,ChainPtr=CurrentPtr->Chain; ChainNr<NextPtr->NrOfChain; ChainNr++,ChainPtr++)
          {
            ChainPtr->NrOfAct = 1;
            if (NextPtr->Chain[ChainNr].NrOfAct == 1)
              ChainPtr->Act[0] = NextPtr->Chain[ChainNr].Act[0];
            else
              ChainPtr->Act[0] = NextPtr->Chain[ChainNr].Act[(Tactic>>(SplitChainNr++))&1];
          } /* for (ChainNr) */
        } /* for (Tactic) */
      } /* for (NextNr) */
      // And now the Next array can be cleared
      for (NextNr=0,NextPtr=Next; NextNr<NrOfNext; NextNr++,NextPtr++)
        free(NextPtr->Chain);

#ifdef DEBUG
      printf("  -Found %d best level-%d sequences:-\n", NrOfCurrent, SeqNr+1);
      for (CurrentNr=0,CurrentPtr=Current; CurrentNr<NrOfCurrent; CurrentNr++,CurrentPtr++)
      {
        printf("  {%3llu} ", CurrentPtr->Score);
        for (ChainNr=0,ChainPtr=CurrentPtr->Chain; ChainNr<CurrentPtr->NrOfChain; ChainNr++,ChainPtr++)
          printf("%s", Act[ChainPtr->Act[0]].Str);
        putchar('\n');
      }
#endif

    } /* for (SeqNr) */
    MinLength = 0;
    for (CurrentNr=0,CurrentPtr=Current; CurrentNr<NrOfCurrent; CurrentNr++,CurrentPtr++)
      if ((!MinLength) || (CurrentPtr->Len < MinLength))
        MinLength = CurrentPtr->Len;

#ifdef DEBUG
    printf("Digit[%d]=%c can be pressed with MinLength=%llu\n", DigitNr, Digit[DigitNr], MinLength);
#endif

    TotalLength += MinLength;
  } /* for (DigitNr) */

  ///////////////////////////////    O u t p u t    /////////////////////////////

  int Numeric = 100*(Digit[0]-'0') + 10*(Digit[1]-'0') + Digit[2]-'0';
  printf("Manual sequence length %llu to obtain number %d gives Mul = %llu\n",
      TotalLength, Numeric, TotalLength*Numeric);
  MulSum += TotalLength*Numeric;

	} /* while (fgets) */
	fclose(InputFile);
	printf("%d inputlines were read.\n", InputLineNr);

  printf("Found MulSum = %llu\n", MulSum);
  return 0;
}
