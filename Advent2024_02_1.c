#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_02_1 <.dat file>\n");
    exit(1);
  }

  FILE* InputFile;
  char InputLine[1000];
  char* InputPtr;
  int InputLineNr=0, InputNr, NrOfInputs;

  ////////////////////////////////    I n p u t    //////////////////////////////

  int Input[10];
	int NrOfSafe = 0;

  if (!(InputFile = fopen(Argument[1], "r")))
  {
    fprintf(stderr, "Could not open %s for reading.\n", Argument[1]);
    exit(2);
  }
  while (fgets(InputLine, 1000, InputFile))
  {
		InputNr = 0;
		while (InputPtr = strtok((InputNr ? NULL : InputLine), " "))
		{
      if (1 != sscanf(InputPtr, "%d", &Input[InputNr]))
      {
        fprintf(stderr, "Could not parse LineNr %d field %d : %s as int\n",
            InputLineNr, InputNr, InputPtr);
        exit(2);
      }
			if (++InputNr > 10)
			{
				fprintf(stderr, "More than 10 input numbers on Line #%d:\n%s",
						InputLineNr, InputLine);
				exit(3);
			}
    } /* while (strtok) */
		NrOfInputs = InputNr;
    ++InputLineNr;

  ////////////////////////////    O p e r a t i o n    //////////////////////////

		int Trend=0, Diff;
		for (InputNr=1; InputNr<NrOfInputs; InputNr++)
		{
			Diff = Input[InputNr] - Input[InputNr-1];
			if ((!Diff) || (Diff < -3) || (Diff > 3))  break;
			if (!Trend)
				Trend = (Diff>0 ? 1 : -1);
			else if (Trend * Diff < 0)
				break;
		}
		// Did we not exit early from the loop of InputNrs on an InputLine ?
		if (InputNr >= NrOfInputs)
			NrOfSafe++;

  }
  printf("%d InputLines were read.\n", InputLineNr);

  ///////////////////////////////    O u t p u t    /////////////////////////////

  printf("Found %d safe inputlines\n", NrOfSafe);
  return 0;
}

