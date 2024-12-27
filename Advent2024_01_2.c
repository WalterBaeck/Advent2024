#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_01 <.dat file>\n");
    exit(1);
  }

  FILE* InputFile;
  char InputLine[1000];
  char* InputPtr;
  int InputLineNr=0, InputNr, NrOfInputs;

  if (!(InputFile = fopen(Argument[1], "r")))
  {
    fprintf(stderr, "Could not open %s for reading.\n", Argument[1]);
    exit(2);
  }

  ////////////////////////////////    I n p u t    //////////////////////////////

  // Two arrays of numbers
  int Input[1000][2];
  while (fgets(InputLine, 1000, InputFile))
  {
    for (InputNr=0; InputNr<2; InputNr++)
    {
      InputPtr = strtok((InputNr ? NULL : InputLine), " ");
      if (1 != sscanf(InputPtr, "%d", &Input[InputLineNr][InputNr]))
      {
        fprintf(stderr, "Could not parse LineNr %d field %d : %s as int\n",
            InputLineNr, InputNr, InputPtr);
        exit(2);
      }
    }
    if (++InputLineNr > 1000)
    {
      fprintf(stderr, "More than 1000 input Lines at Input:\n%s",
          InputLine);
      exit(2);
    }
  }
  printf("%d InputLines were read.\n", InputLineNr);
  NrOfInputs = InputLineNr;

  ////////////////////////////    O p e r a t i o n    //////////////////////////

  unsigned int MatchNr, NrOfMatches;
  unsigned int Sim, SimSum=0;

  for (InputNr=0; InputNr<NrOfInputs; InputNr++)
  {
    NrOfMatches = 0;
    for (MatchNr=0; MatchNr<NrOfInputs; MatchNr++)
      if (Input[MatchNr][1] == Input[InputNr][0])  NrOfMatches++;
    if (!NrOfMatches)  continue;
    Sim = Input[InputNr][0] * NrOfMatches;
    // Guard against overflow
    unsigned int OldSimSum = SimSum;
    SimSum += Sim;
    if (OldSimSum > SimSum)
    {
      fprintf(stderr, "Overflow in SimSum after InputNr #%d : %d with Sim=%d (NrOfMatches %d)\n",
          InputNr, Input[InputNr][0], Sim, NrOfMatches);
      fprintf(stderr, "  from OldSimSum %d to NewSimSum %d ??\n",
          OldSimSum, SimSum);
    }
  }

  ///////////////////////////////    O u t p u t    /////////////////////////////

  printf("Sum of %d similarities is %d\n", InputNr, SimSum);
  return 0;
}

