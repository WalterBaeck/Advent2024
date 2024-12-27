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

  // Two unsorted arrays of numbers
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

  // Sorted arrays
  int Sorted[1000][2];
  int SortNr, NrOfSorted, ShiftNr;

  for (InputNr=0; InputNr<2; InputNr++)
    for (NrOfSorted=0; NrOfSorted<NrOfInputs; NrOfSorted++)
    {
      // Go through already Sorted array to find right insertion spot for new number
      for (SortNr=0; SortNr<NrOfSorted; SortNr++)
        if (Input[NrOfSorted][InputNr] < Sorted[SortNr][InputNr])  break;
      // Shift all Sorted elements beyond this position, up by one spot
      for (ShiftNr=NrOfSorted; ShiftNr>SortNr; ShiftNr--)
        Sorted[ShiftNr][InputNr] = Sorted[ShiftNr-1][InputNr];
      // Now insert the new number at the proper spot
      Sorted[SortNr][InputNr] = Input[NrOfSorted][InputNr];
    }

#ifdef DEBUG
for (InputNr=0; InputNr<2; InputNr++)
{
  for (SortNr=0; SortNr<NrOfInputs; SortNr++)
    printf("#%d: %d\n", SortNr, Sorted[SortNr][InputNr]);
  printf("-----\n");
}
#endif

  // Sum all the differences
  int Diff,DiffSum = 0;
  for (InputNr=0; InputNr<NrOfInputs; InputNr++)
  {
    Diff = Sorted[InputNr][1] - Sorted[InputNr][0];
    DiffSum += (Diff < 0 ? -Diff : Diff);
  }

  ///////////////////////////////    O u t p u t    /////////////////////////////

  printf("Sum of %d differences is %d\n", InputNr, DiffSum);
  return 0;
}

