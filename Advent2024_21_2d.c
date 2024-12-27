#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NR_ROBOTS 26
//#define NR_ROBOTS 12
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

  // Dynamic Programming = Recursion + Memoization(=Caching)
  unsigned long long Cache[100][NR_ROBOTS] = {0}, BackupCache[100][NR_ROBOTS];
  // Note that this function assumes that all EvolveActs have been flattened :
  //   it uses Act[0] from every Chainlink in an EvolveAct.
  unsigned long long LenAfterGen(int ActNr, int SeqNr)
  {
    if      (SeqNr == 0)  return       Act[ActNr].Len;
    else if (SeqNr == 1)  return EvolveAct[ActNr].Len;
    else if (Cache[ActNr][SeqNr])  return Cache[ActNr][SeqNr];
    // We need to compute this Gen's length by evolving into next Gen's EvolveAct,
    //   and then recursing the function call on each of that EvolveAct's Acts.
    unsigned long long RetVal = 0, AddVal;
    // Note that EVERY variable used in a recursive function needs to be declared locally
    tSolution* NextPtr = EvolveAct + ActNr;
    unsigned long long ChainNr;
    tChain* ChainPtr;
#ifdef DEBUG
    printf("  Computing LenAfterGen(%2d,%2d) %s with NrOfChain %d ..\n",
        ActNr, SeqNr, Act[ActNr].Str, NextPtr->NrOfChain);
#endif
    for (ChainNr=0,ChainPtr=NextPtr->Chain; ChainNr<NextPtr->NrOfChain; ChainNr++,ChainPtr++)
    {
      AddVal = LenAfterGen(ChainPtr->Act[0], SeqNr-1);
      RetVal += AddVal;
#ifdef DEBUG
      printf("  .. added LenAfterGen(%2d,%2d) %s %llu\n",
          ChainPtr->Act[0], SeqNr-1, Act[ChainPtr->Act[0]].Str, AddVal);
#endif
    }
#ifdef DEBUG
    printf("    total LenAfterGen(%2d,%2d) %llu\n", ActNr, SeqNr, RetVal);
#endif
    // Don't forget to store this computation result into the Cache
    Cache[ActNr][SeqNr] = RetVal;
    return RetVal;
  } /* ull LenAfterGen() */

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
  unsigned long long MinLength, TotalLength;

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

    // // // // // Keep exploring how each known Act evolves // // // // //

    for (;;)
    {
      int EvolvedMoreActs = 0;
      for (ActNr=0; ActNr<NrOfAct; ActNr++)
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
          printf("   Act[%d]=%s has length %d\n", ActNr, ActPtr->Str, ThisLen);
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
          EvolvedMoreActs = 1;
        } /* if (!EvolveAct) */
      if (!EvolvedMoreActs)  break;
    } /* for(;;) */

    // // // // // With a full tree of Acts evolved to each other, clean up each Act // // // // //

    // An easy decision comes from EvolveAct[Act].Score on any Chainlink with 2 competing Acts.
    // This has the equivalent predicting power of looking 2 generations ahead.
    for (ActNr=0; ActNr<NrOfAct; ActNr++)
    {
      NextPtr = EvolveAct + ActNr;
      for (ChainNr=0,ChainPtr=NextPtr->Chain; ChainNr<NextPtr->NrOfChain; ChainNr++,ChainPtr++)
        // Quick choice between both Acts by considering how they'll evolve
        if ((ChainPtr->NrOfAct > 1) &&
            (EvolveAct[ChainPtr->Act[0]].Score != EvolveAct[ChainPtr->Act[1]].Score))
        {
          int Prefer0 = (EvolveAct[ChainPtr->Act[0]].Score < EvolveAct[ChainPtr->Act[1]].Score);
#ifdef DEBUG
          printf("  Quick choice Chain[%d][%llu] to Act[%2d] %s iso Act[%2d] %s\n",
              ActNr, ChainNr,
              (!Prefer0) ? ChainPtr->Act[1] : ChainPtr->Act[0],
              (!Prefer0) ? Act[ChainPtr->Act[1]].Str : Act[ChainPtr->Act[0]].Str,
              (!Prefer0) ? ChainPtr->Act[0] : ChainPtr->Act[1],
              (!Prefer0) ? Act[ChainPtr->Act[0]].Str : Act[ChainPtr->Act[1]].Str);
          printf("   because EvolveAct[%d] has {%d} and EvolveAct[%d] has {%d}\n",
              ChainPtr->Act[0], EvolveAct[ChainPtr->Act[0]].Score,
              ChainPtr->Act[1], EvolveAct[ChainPtr->Act[1]].Score);
#endif
          if (!Prefer0)
            ChainPtr->Act[0] = ChainPtr->Act[1];
          ChainPtr->NrOfAct = 1;
        } /* if (NrOfAct > 1) */
    } /* for (ActNr) */

    // Next, for every Chainlink that remains undecided with 2 possible Act choices,
    //   look a few more steps ahead, **while using Act[0] of all other EvolveActs**
    // This approach can look ahead arbitrarily far, but it is inaccurate.
    for (;;)
    {
      int FlattenedMoreChains = 0;
    for (ActNr=0; ActNr<NrOfAct; ActNr++)
    {
      NextPtr = EvolveAct + ActNr;
      for (ChainNr=0,ChainPtr=NextPtr->Chain; ChainNr<NextPtr->NrOfChain; ChainNr++,ChainPtr++)
        if (ChainPtr->NrOfAct > 1)
        {
#ifdef DEBUG
              printf("    Deciding bloated Chain[%d][%llu]  Act[%2d] %s vs Act[%2d] %s\n",
                  ActNr, ChainNr, ChainPtr->Act[0], Act[ChainPtr->Act[0]].Str,
                  ChainPtr->Act[1], Act[ChainPtr->Act[1]].Str);
#endif
          // Force a choice between both Acts by considering how they'll evolve
          unsigned long long Len[2];
          int Ch, BackupAct0;
          for (Step=0; Step<5; Step++)
          {
            for (Ch=0; Ch<2; Ch++)
            {
              // Take a backup of the dilemma situation first
              BackupAct0 = ChainPtr->Act[0];
              memcpy(BackupCache, Cache, 100 * NR_ROBOTS * sizeof(unsigned long long));
              // Assume Act[Ch] of this Chainlink prevails
              if (Ch == 1)
                ChainPtr->Act[0] = ChainPtr->Act[1];
              ChainPtr->NrOfAct = 1;
              // See where this leads after #Step generations
              Len[Ch] = LenAfterGen(ChainPtr->Act[Ch], Step);
              // Undo Act[Ch] Choice
              ChainPtr->Act[0] = BackupAct0;
              ChainPtr->NrOfAct = 2;
              memcpy(Cache, BackupCache, 100 * NR_ROBOTS * sizeof(unsigned long long));
            } /* for (Ch) */
#ifdef DEBUG
            printf("      Step %d: Len[0]=%llu  Len[1]=%llu\n", Step+1, Len[0], Len[1]);
#endif
            if (Len[0] != Len[1])
            {
#ifdef DEBUG
              printf("    Reduced bloated Chain[%d][%llu] to Act[%2d] %s iso Act[%2d] %s\n",
                  ActNr, ChainNr,
                  (Len[0] > Len[1]) ? ChainPtr->Act[1] : ChainPtr->Act[0],
                  (Len[0] > Len[1]) ? Act[ChainPtr->Act[1]].Str : Act[ChainPtr->Act[0]].Str,
                  (Len[0] > Len[1]) ? ChainPtr->Act[0] : ChainPtr->Act[1],
                  (Len[0] > Len[1]) ? Act[ChainPtr->Act[0]].Str : Act[ChainPtr->Act[1]].Str);
#endif
              if (Len[0] > Len[1])
                ChainPtr->Act[0] = ChainPtr->Act[1];
              ChainPtr->NrOfAct = 1;
              FlattenedMoreChains = 1;
              break;
            } /* if (Len[0] != Len[1]) */
          } /* for (Step) */
          if (Step >= 5)
          {
            fprintf(stderr, "Stuck on dilemma Chain[%d][%llu] to Act[%2d] %s vs Act[%2d] %s\n",
                ActNr, ChainNr, ChainPtr->Act[1], Act[ChainPtr->Act[1]].Str,
                ChainPtr->Act[0], Act[ChainPtr->Act[0]].Str);
            exit(6);
          }
        } /* if (NrOfAct > 1) */
    } /* for (ActNr) */
    if (!FlattenedMoreChains)  break;
    } /* for(;;) */

#ifdef DEBUG
    printf("************* All %d Acts Evolved into flat Chains **************\n", NrOfAct);
#endif

    // // // // // Go through NR_ROBOTS-1 layers of DirPad possibilities for this Digit // // // // //

    // Note that Seq[0] start array of up to 2 Current Solutions can NOT be pruned
    //   by looking at EvolveAct[].Score of each Act :-(

    MinLength = 0;
    for (CurrentNr=0,CurrentPtr=Current; CurrentNr<NrOfCurrent; CurrentNr++,CurrentPtr++)
    {
      unsigned long long Length = LenAfterGen(CurrentPtr->Chain->Act[0], NR_ROBOTS-1);
      if ((!MinLength) || (Length < MinLength))
        MinLength = Length;
    }
    printf("Digit[%d]=%c can be pressed with MinLength=%llu\n", DigitNr, Digit[DigitNr], MinLength);

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
