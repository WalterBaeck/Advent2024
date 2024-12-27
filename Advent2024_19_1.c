#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_19_1 <.dat file>\n");
    exit(1);
  }

  FILE* InputFile;
  char InputLine[3000], *Ptr;
	int InputLineNr=0;
	int PatternNr=0, NrOfPatterns;
	int TargetNr=0, NrOfTargets;
	char Pattern[500][15], Target[500][100];
	int PatternLength[500];

  ////////////////////////////////    I n p u t    //////////////////////////////

  if (!(InputFile = fopen(Argument[1], "r")))
  {
    fprintf(stderr, "Could not open %s for reading.\n", Argument[1]);
    exit(2);
  }
	// Get available Patterns
	if (fgets(InputLine, 3000, InputFile))
	{
		InputLineNr++;
		// Strip terminating newline
		while ((InputLine[strlen(InputLine)-1] == '\n') || (InputLine[strlen(InputLine)-1] == '\r'))
			InputLine[strlen(InputLine)-1] = '\0';
		while (Ptr = strtok(PatternNr ? NULL : InputLine, ", "))
		{
			strcpy(Pattern[PatternNr], Ptr);
			PatternLength[PatternNr] = strlen(Ptr);
			if (++PatternNr >= 500)
			{
				fprintf(stderr, "More than 500 Patterns found on: %s\n", InputLine);
				exit(4);
			}
		}
		NrOfPatterns = PatternNr;
#ifdef DEBUG
  	printf("Found %d Patterns\n", NrOfPatterns);
#endif
	}
	if (fgets(InputLine, 3000, InputFile))
	{
		InputLineNr++;
		// Strip terminating newline
		while ((InputLine[strlen(InputLine)-1] == '\n') || (InputLine[strlen(InputLine)-1] == '\r'))
			InputLine[strlen(InputLine)-1] = '\0';
		if (strlen(InputLine))
		{
			fprintf(stderr, "Expecting blank separator line between Patterns and Targets: %s\n", InputLine);
			exit(3);
		}
	}
	// Get Register values
	while (fgets(InputLine, 3000, InputFile))
	{
		InputLineNr++;
		// Strip terminating newline
		while ((InputLine[strlen(InputLine)-1] == '\n') || (InputLine[strlen(InputLine)-1] == '\r'))
			InputLine[strlen(InputLine)-1] = '\0';
		strcpy(Target[TargetNr], InputLine);
		if (++TargetNr >= 500)
		{
			fprintf(stderr, "More than 500 Targets found at InputLine #%d: %s\n", InputLineNr, InputLine);
			exit(4);
		}
	}
	NrOfTargets = TargetNr;
	fclose(InputFile);
	printf("%d InputLines were read, found %d Patterns and %d Targets\n", InputLineNr, NrOfPatterns, NrOfTargets);

  ////////////////////////////    O p e r a t i o n    //////////////////////////

	int Choice[500], ChoiceNr;
	int Pos, Len, Sum=0;
	for (TargetNr=0; TargetNr<NrOfTargets; TargetNr++)
	{
		Pos = 0;
		Len = strlen(Target[TargetNr]);
		ChoiceNr=0;
		Choice[0]=-1;
		// Full tree search
		for(;;)
			// Is increasing the Choice at the current node still possible ?
			if (++Choice[ChoiceNr] < NrOfPatterns)
			{
				// Is this Pattern an appropriate fit at this Position of the Target ?
				if (strncmp(Pattern[Choice[ChoiceNr]], Target[TargetNr]+Pos, PatternLength[Choice[ChoiceNr]]))
					continue;
				// If successfull, adopt this Pattern and move on the Position
				Pos += PatternLength[Choice[ChoiceNr]];
				// Have we reached the full Target ?
				if (Pos >= Len)
				{
					// As soon as this Target can be formed in one way, stop searching
#ifdef DEBUG
					printf("Found Target[%d]=%s as %d Patterns:", TargetNr, Target[TargetNr], ChoiceNr+1);
					int Index;
					for (Index=0; Index<=ChoiceNr; Index++)  printf(" %s%s", (Index?"+ ":""), Pattern[Choice[Index]]);
					putchar('\n');
#endif
					Sum++;  break; /* from for(;;) */
				}
				Choice[++ChoiceNr] = -1;
			}
			else
			{
				// No further Choice increase possible at this node : need to backtrack
				if (--ChoiceNr < 0)
				{
#ifdef DEBUG
					printf("IMPOSSIBLE: Target[%d]=%s\n", TargetNr, Target[TargetNr]);
#endif
					break;  /* from (for;;) */
				}
				Pos -= PatternLength[Choice[ChoiceNr]];
			}
	} /* for (TargetNr) */

  ///////////////////////////////    O u t p u t    /////////////////////////////

	printf("Found Sum %d\n", Sum);

	return 0;
}
