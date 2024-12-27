#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_19_2 <.dat file>\n");
    exit(1);
  }

  FILE* InputFile;
  char InputLine[3000], *Ptr;
	int InputLineNr=0;
	int PatternNr=0, NrOfPatterns;
	int TargetNr=0, NrOfTargets;
	char Pattern[500][15], Target[500][100];
	int MaxPatternLength=0;

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
			if (strlen(Ptr) > MaxPatternLength)
				MaxPatternLength = strlen(Ptr);
			if (++PatternNr >= 500)
			{
				fprintf(stderr, "More than 500 Patterns found on: %s\n", InputLine);
				exit(4);
			}
		}
		NrOfPatterns = PatternNr;
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
	printf("%d InputLines were read, found %d Patterns (MaxLen=%d) and %d Targets\n",
			InputLineNr, NrOfPatterns, MaxPatternLength, NrOfTargets);

  ////////////////////////////    O p e r a t i o n    //////////////////////////

	// Sort the available Patterns into Fragments of fixed length for quick lookup
	char Frag[10][200][10];
	int FragNr, NrOfFrags[10] = {0};
	int Len, Index, CmpResult;
	for (PatternNr=0; PatternNr<NrOfPatterns; PatternNr++)
	{
		Len = strlen(Pattern[PatternNr]);
		for (FragNr=0; FragNr<NrOfFrags[Len]; FragNr++)
			if (strcmp(Pattern[PatternNr], Frag[Len][FragNr]) < 0)  break;
		// Insertion spot found: shift all further items in array out
		for (Index=NrOfFrags[Len]; Index>FragNr; Index--)
			strcpy(Frag[Len][Index], Frag[Len][Index-1]);
		// Now insert the new pattern in this spot
		strcpy(Frag[Len][Index], Pattern[PatternNr]);
		// And remember that the array has grown by one item
		NrOfFrags[Len]++;
	} /* for (PatternNr) */

#ifdef DEBUG
	for (Len=1; Len<=MaxPatternLength; Len++)
	{
		printf("%2d Frags of Len=%d:", NrOfFrags[Len], Len);
		for (FragNr=0; FragNr<NrOfFrags[Len]; FragNr++)  printf(" %s,", Frag[Len][FragNr]);
		putchar('\n');
	}
#endif

	int Choice[120], ChoiceNr;
	int Pos, MaxLen;
	unsigned long long Sum=0, PrevSum, SumAt[120];
	long long Solutions[120];

	for (TargetNr=0; TargetNr<NrOfTargets; TargetNr++)
	{
		Len = strlen(Target[TargetNr]);
#ifdef DEBUG
		printf("= = = = =\nTarget[%d] : %s of length %d\n= = = = =\n", TargetNr, Target[TargetNr], Len);
#endif

		// Feasibility check : around each character in the Target string,
		//   verify	that *any* Fragment exists that includes this letter.
		for (Pos=MaxPatternLength-1; Pos<Len; Pos++)
		{
			CmpResult = 1;
			// Try to match with a Fragment of length=ChoiceNr
			for (ChoiceNr=1; ChoiceNr<=MaxPatternLength; ChoiceNr++)
			{
				// Start from position=Pos+1-Index
				for (Index=ChoiceNr; Index; Index--)
				{
					// Do we have an appropriate Fragment available of this chosen length ?
					for (FragNr=0; FragNr<NrOfFrags[ChoiceNr]; FragNr++)
					{
						CmpResult = strncmp(Frag[ChoiceNr][FragNr], Target[TargetNr]+Pos+1-Index, ChoiceNr);
						if (CmpResult >= 0)  break;
					}
					if (!CmpResult)  break;
				}
				if (!CmpResult)  break;
			} /* for (ChoiceNr) */
			if (CmpResult)  break;
#ifdef DEBUG
			printf("Target[%d]=%c can be covered with Frag[%d][%d]=%s from Pos=%d\n",
					Pos, Target[TargetNr][Pos], ChoiceNr, FragNr, Frag[ChoiceNr][FragNr], Pos+1-Index);
#endif
		} /* for (Pos) */
		if (Pos<Len)
		{
#ifdef DEBUG
			printf("UNFEASIBLE Target[%d]=%c cannot be covered by any Fragment\n", Pos, Target[TargetNr][Pos]);
#endif
			continue;
		}

		// Reset the cache
		for (Pos=0; Pos<120; Pos++)  Solutions[Pos] = -1;
		PrevSum = Sum;
		// The tree search happens by deciding the Fragment Length that will be used
		//   at the next incomplete part of the Target string.
		Pos = 0;
		ChoiceNr = 0;
		Choice[0] = 0;
		// Full tree search
		for(;;)
		{
			MaxLen = MaxPatternLength;
			if (Pos + MaxLen > Len)
				MaxLen = Len - Pos;
			// Is increasing the Choice at the current node still possible ?
			if (++Choice[ChoiceNr] <= MaxLen)
			{
				// Do we have an appropriate Fragment available of this chosen length ?
				for (FragNr=0; FragNr<NrOfFrags[Choice[ChoiceNr]]; FragNr++)
				{
					CmpResult = strncmp(Frag[Choice[ChoiceNr]][FragNr], Target[TargetNr]+Pos, Choice[ChoiceNr]);
					if (CmpResult >= 0)  break;
				}
				if (CmpResult)  continue;
				// If successfull, adopt this Fragment and move on the Position
				Pos += Choice[ChoiceNr];
				// Have we reached the full Target ?
				if (Pos >= Len)
				{
#ifdef DEBUG
					printf("Found Target[%d]=%s as %d Fragments:", TargetNr, Target[TargetNr], ChoiceNr+1);
					Pos = 0;
					for (FragNr=0; FragNr<=ChoiceNr; FragNr++)
					{
						printf(" %s", (FragNr ? "+ " : ""));
						for (Index=0; Index<Choice[FragNr]; Index++,Pos++)  putchar(Target[TargetNr][Pos]);
					}
					putchar('\n');
#endif
					Pos -= Choice[ChoiceNr];
					Sum++;
					continue;
				}
				// We are advancing in the search tree.
				// Perhaps we're arriving at a Position that already has a cached amount of solutions.
				if (Solutions[Pos] >= 0)
				{
					// In that case, we needn't bother with working all the way to the end of the Target string
					Sum += Solutions[Pos];
#ifdef DEBUG
					printf("Advancing to Pos=%d with known Solutions=%lld so add to Sum=%llu and recede to Pos=%d\n",
							Pos, Solutions[Pos], Sum, Pos-Choice[ChoiceNr]);
#endif
					Pos -= Choice[ChoiceNr];
					continue;
				}
				// Since we're going to explore from the Position to the end, register where we're at now
#ifdef DEBUG
				printf("Advancing to Pos=%d with unknown Solutions so we note SumAt=%llu\n", Pos, Sum);
#endif
				SumAt[Pos] = Sum;
				Choice[++ChoiceNr] = 0;
			}
			else
			{
				// No further Choice increase possible at this node : need to backtrack
				if (--ChoiceNr < 0)
					break;  /* from (for;;) */
				// Since all possibilities have been exhausted from this Position,
				//   we can store their amount in the cache
				Solutions[Pos] = Sum - SumAt[Pos];
#ifdef DEBUG
				printf("Backtracking to Pos=%d, we find Solutions[Pos=%d] = Sum %llu - SumAt %llu = %lld\n",
						Pos-Choice[ChoiceNr], Pos, Sum, SumAt[Pos], Solutions[Pos]);
#endif
				Pos -= Choice[ChoiceNr];
			}
		} /* for(;;) */

		printf("Target[%d] has %llu solutions\n", TargetNr, Sum-PrevSum);
	} /* for (TargetNr) */

  ///////////////////////////////    O u t p u t    /////////////////////////////

	printf("\nFound Sum %llu\n", Sum);
	return 0;
}
