#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_07_1 <.dat file>\n");
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

  unsigned long long Sum=0;
  unsigned long long Target, Input[20];
  while (fgets(InputLine, 1000, InputFile))
  {
    // First parse the Target number
    InputPtr = strtok(InputLine, ":");
    if (1 != sscanf(InputPtr, "%llu", &Target))
    {
      fprintf(stderr, "Could not parse LineNr %d field #0 : %s as long\n",
          InputLineNr, InputPtr);
      exit(2);
    }
    // Then get all the Operand numbers
    InputNr = 0;
    while (InputPtr = strtok(NULL, " "))
    {
      if (1 != sscanf(InputPtr, "%llu", &Input[InputNr]))
      {
        fprintf(stderr, "Could not parse LineNr %d field %d : %s as long\n",
            InputLineNr, InputNr, InputPtr);
        exit(2);
      }
      if (++InputNr >= 20)
      {
        fprintf(stderr, "More than 10 operands on Line #%d:\n%s",
            InputLineNr, InputLine);
        exit(3);
      }
    }
    NrOfInputs = InputNr;
    InputLineNr++;

#ifdef DEBUG
    printf("\n--------------- Line %3d -----------\n", InputLineNr);
    printf("    with Sum = %llu\n", Sum);
    printf("Target=%llu  ", Target);
    for (InputNr=0; InputNr<NrOfInputs; InputNr++)
      printf(" %llu", Input[InputNr]);
    printf("\n= = = = = = = = = = = = = = = = = = =\n");
#endif

  ////////////////////////////    O p e r a t i o n    //////////////////////////

    unsigned long long Result[20];
    int Choice[20], ChoiceNr;
    unsigned long long LeftHandSide;

    // Initialize the tree search
    ChoiceNr = 0;
    Choice[ChoiceNr] = -1;
    // Always try to make a further Operator Choice
    for(;;)
    {
      // Possible to choose the next Operator at the current ChoiceNr ?
      if (++Choice[ChoiceNr] < 2)
      {
        // Yes: then update the Result at this ChoiceNr
        LeftHandSide = (ChoiceNr ? Result[ChoiceNr-1] : Input[0]);
        switch (Choice[ChoiceNr])
        {
          case 0: Result[ChoiceNr] = LeftHandSide + Input[ChoiceNr+1]; break;
          case 1: Result[ChoiceNr] = LeftHandSide * Input[ChoiceNr+1]; break;
        }
#ifdef DEBUG
        printf("Choice[%d]=%d LeftHandSide=%llu RightHandSide=%llu Result=%llu\n",
            ChoiceNr, Choice[ChoiceNr], LeftHandSide, Input[ChoiceNr+1], Result[ChoiceNr]);
#endif
        // Need to move on to the next ChoiceNr, but is that possible ?
        if (ChoiceNr >= NrOfInputs-2)
        {
          // Last Choice has been made; have we reached the Target ?
          if (Result[ChoiceNr] == Target)
          {
            Sum += Target;
            break; // from forever loop
          }
        }
        // Perhaps we have already exceeded the Target, we'll never get lower
        else if (Result[ChoiceNr] <= Target)
          // All conditions met to move to a further node of the tree
          Choice[++ChoiceNr] = -1;
        // Regardless if we're stuck at the same node or advancing: next Choice !
        continue;
      } /* Choice advance was possible */
      else
      {
        // If all Choices have been exhausted at this node, we need to backtrack
        if (--ChoiceNr < 0)
          // When backtracking goes past the root of the tree, the search is complete
          break; // from forever loop
      }
    } /* for(;;) */

  } /* while (fgets) */
  printf("%d InputLines were read.\n", InputLineNr);

  ///////////////////////////////    O u t p u t    /////////////////////////////

  printf("Found Sum = %llu\n", Sum);
  return 0;
}
