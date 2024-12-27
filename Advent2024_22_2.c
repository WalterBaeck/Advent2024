#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_22_2 <.dat file>\n");
    exit(1);
  }

  FILE* InputFile;
  char InputLine[1000];
  int InputLineNr=0;
  int Seed[3000];
  int SeedNr, NrOfSeeds;
  unsigned Step, Secret;
  char *Digit, *Diff, PrevDigit;
  char Seq[4], MaxSeq[4];
	int Sum,MaxSum=0;

  ////////////////////////////////    I n p u t    //////////////////////////////

  if (!(InputFile = fopen(Argument[1], "r")))
  {
    fprintf(stderr, "Could not open %s for reading.\n", Argument[1]);
    exit(2);
  }
	while (fgets(InputLine, 1000, InputFile))
    if (1 != sscanf(InputLine, "%d", &Seed[InputLineNr++]))
    {
      fprintf(stderr, "Could not parse Seed on InputLine #%d: %s\n",
          InputLineNr, InputLine);
      exit(3);
    }
	fclose(InputFile);
	printf("%d inputlines were read.\n", InputLineNr);
  NrOfSeeds = InputLineNr;

  if (!(Digit = (char*)malloc(3000 * 2000)))
  {
    fprintf(stderr, "Could not allocate 3000 * 2000 chars for Digit\n");
    exit(4);
  }
  if (!(Diff = (char*)malloc(3000 * 2000)))
  {
    fprintf(stderr, "Could not allocate 3000 * 2000 chars for Diff\n");
    exit(4);
  }

  ////////////////////////////    O p e r a t i o n    //////////////////////////

  // Prepare the tables of Digits and Diffs: an array of 2000 for every Step
  for (SeedNr=0; SeedNr<NrOfSeeds; SeedNr++)
  {
    Secret = Seed[SeedNr];
    PrevDigit = Secret%10;
    for (Step=0; Step<2000; Step++)
    {
      // First part : multiplication by 64
      Secret ^= (Secret << 6);
      Secret &= 0xFFFFFF;
      // Second part : division by 32
      Secret ^= (Secret >> 5);
      Secret &= 0xFFFFFF;
      // Third part : multiplication by 2048
      Secret ^= (Secret << 11);
      Secret &= 0xFFFFFF;

      // We now know a new Digit
      Digit[SeedNr*2000 + Step] = Secret%10;
      Diff[SeedNr*2000 + Step] = Digit[SeedNr*2000 + Step] - PrevDigit;
      PrevDigit = Digit[SeedNr*2000 + Step];
    } /* for (Step) */
  } /* for (SeedNr) */

  // Consider all possible sequences of 4 diffs over these tables
  Seq[0]=-9; Seq[1]=-9; Seq[2]=-9; Seq[3]=-9;
  char DebugSeq[4];
  DebugSeq[0]=-2;DebugSeq[1]=1;DebugSeq[2]=-1;DebugSeq[3]=3;
  for(;;)
  {
    int SeqNr;
    // Try to increase a Seq[] choice at the far end
    for (SeqNr=3; SeqNr>=0; SeqNr--)
      if (++Seq[SeqNr] <= 9)  break;  else  Seq[SeqNr] = -9;
    if (SeqNr < 0) // search exhausted
      break;  /* from for(;;) */
    // Sanity check: weed out Sequence that will yield impossible Digit
    char Start=9;
    for (SeqNr=0; SeqNr<4; SeqNr++)
      if ((Start += Seq[SeqNr]) < -9)  break;
    if (SeqNr<4)  continue;
    Start=-9;
    for (SeqNr=0; SeqNr<4; SeqNr++)
      if ((Start += Seq[SeqNr]) > 9)  break;
    if (SeqNr<4)  continue;

    // With this Sequence, consider the price obtained for every Seed
    Sum = 0;
    for (SeedNr=0; SeedNr<NrOfSeeds; SeedNr++)
      for (Step=0; Step<=2000-4; Step++)
        if (!memcmp(Seq, Diff + SeedNr*2000 + Step, 4*sizeof(char)))
        {
#ifdef DEBUG
          if (!memcmp(Seq,DebugSeq,4))
            printf("Found Digit[%d][%d] %hhd and Diff %hhd with Seq[%hhd,%hhd,%hhd,%hhd]\n",
                SeedNr, Step, Digit[SeedNr*2000 + Step+3], Diff[SeedNr*2000 + Step+3],
                Seq[0],Seq[1],Seq[2],Seq[3]);
#endif
          Sum += Digit[SeedNr*2000 + Step+3];
          break;
        }

#ifdef DEBUG
    if ((Sum) && (!memcmp(Seq,DebugSeq,4)))
      printf("Found Sum %d with Seq[%hhd,%hhd,%hhd,%hhd]\n", Sum, Seq[0],Seq[1],Seq[2],Seq[3]);
#endif

    // Now that this Sequence generates a known Sum, compare it
    if (Sum > MaxSum)
    {
      MaxSum = Sum;
      memcpy(MaxSeq, Seq, 4*sizeof(char));
      printf("Found better Sum %d with Seq[%hhd,%hhd,%hhd,%hhd]\n", Sum, Seq[0],Seq[1],Seq[2],Seq[3]);
    }
  } /* for(;;) */

  ///////////////////////////////    O u t p u t    /////////////////////////////

	printf("Found MaxSum %d with Seq[%hhd,%hhd,%hhd,%hhd]\n", MaxSum,
      MaxSeq[0],MaxSeq[1],MaxSeq[2],MaxSeq[3]);
  return 0;
}
