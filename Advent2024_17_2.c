#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_17_2 <.dat file>\n");
    exit(1);
  }

  FILE* InputFile;
  char InputLine[1000], *Ptr;
	int InputLineNr=0;
	int CodeNr=0, NrOfCodes;
	unsigned char Code[20], RegName;
	unsigned long long A,B,C, Operand;

  ////////////////////////////////    I n p u t    //////////////////////////////

  if (!(InputFile = fopen(Argument[1], "r")))
  {
    fprintf(stderr, "Could not open %s for reading.\n", Argument[1]);
    exit(2);
  }
	// Get Register values
	while (fgets(InputLine, 1000, InputFile))
	{
		InputLineNr++;
		// Strip terminating newline
		while ((InputLine[strlen(InputLine)-1] == '\n') || (InputLine[strlen(InputLine)-1] == '\r'))
			InputLine[strlen(InputLine)-1] = '\0';
		if (!strlen(InputLine))  break;
		if (2 != sscanf(InputLine, "Register %c: %llu", &RegName, &Operand))
		{
			fprintf(stderr, "Could not scan RegName and Value on InputLine #%d: %s\n", InputLineNr, InputLine);
			exit(3);
		}
		switch (RegName)
		{
			case 'A': A = Operand;  break;
			case 'B': B = Operand;  break;
			case 'C': C = Operand;  break;
		}
	}
	// Get Opcodes
	if (fgets(InputLine, 1000, InputFile))
	{
		InputLineNr++;
		// Strip terminating newline
		while ((InputLine[strlen(InputLine)-1] == '\n') || (InputLine[strlen(InputLine)-1] == '\r'))
			InputLine[strlen(InputLine)-1] = '\0';
		if (strncmp(InputLine, "Program: ", 9))
		{
			fprintf(stderr, "Opcode line does not start with Program: %s\n", InputLine);
			exit(4);
		}
		while (Ptr = strtok(CodeNr ? NULL : InputLine+9, ","))
		{
			if (1 != sscanf(Ptr, "%hhu", &Code[CodeNr]))
			{
				fprintf(stderr, "Could not read OpCode #%d from string %s\n", CodeNr, Ptr);
				exit(4);
			}
			if (++CodeNr > 20)
			{
				fprintf(stderr, "More than 20 OpCodes found on: %s\n", InputLine);
				exit(4);
			}
		}
		NrOfCodes = CodeNr;
	}
	fclose(InputFile);
	printf("%d InputLines were read, found %d OpCodes\n", InputLineNr, NrOfCodes);

  ////////////////////////////    O p e r a t i o n    //////////////////////////

	typedef struct {
		unsigned long long Value,Mask;
	} tConstraint;

	// For each of the 8 possible output digits, we have up to 8 possible ways to obtain them
	tConstraint Maker[8][8];
	int NrOfMakers[8] = {0};

	// Helper function
	void ValueMaskMakes(unsigned long long Value, unsigned long long Mask, int Output)
	{
		tConstraint theConstraint;
		theConstraint.Value = Value;
		theConstraint.Mask = Mask;
		Maker[Output][NrOfMakers[Output]++] = theConstraint;
#ifdef DEBUG
		printf("Value %llu Mask 0x%X makes %d, which now has %d makers\n", Value, Mask, Output, NrOfMakers[Output]);
#endif
	}

	// Construct a table of Makers that was obtained from pen-and-paper analysis
	unsigned long long Value, Mask;
	int Output;
	ValueMaskMakes(1,7,5);
	ValueMaskMakes(0,0xF,5);
	ValueMaskMakes(8,0xF,1);
	for (Value=3,Output=6; Value<3+32; Value+=8,Output-=2)
		ValueMaskMakes(Value,0x1F,Output);
	for (Value=2,Output=7; Value<2+64; Value+=8,Output--)
		ValueMaskMakes(Value,0x3F,Output);
	for (Value=5,Output=0; Value<5+128; Value+=16,Output++)
		ValueMaskMakes(Value,0x77,Output);
	for (Value=4,Output=0; Value<4+256; Value+=32,Output++)
		ValueMaskMakes(Value,0xE7,Output^1);
	for (Value=7,Output=0; Value<7+512; Value+=64,Output++)
		ValueMaskMakes(Value,0x1C7,Output^2);
	for (Value=6,Output=0; Value<6+1024; Value+=128,Output++)
		ValueMaskMakes(Value,0x387,Output^3);

	// Tree search for all possible ways to make each required output digit
	int Choice[20], ChoiceNr;
	tConstraint Constraint[20], theMaker;
	unsigned long long Min=0;

	ChoiceNr = 0;
	Choice[ChoiceNr] = -1;
	// Always try to make a further Choice
	for(;;)
		// Possible to pick next Choice at the current ChoiceNr ?
		if (++Choice[ChoiceNr] < NrOfMakers[Code[ChoiceNr]])
		{
			// Yes: then determine if this Maker is compatible with previous constraints
			theMaker = Maker[Code[ChoiceNr]][Choice[ChoiceNr]];
			Value = theMaker.Value << (3*ChoiceNr);
			Mask  = theMaker.Mask  << (3*ChoiceNr);
			if (ChoiceNr)
			{
				if (Mask & Constraint[ChoiceNr-1].Mask)
				{
#ifdef DEBUG
	printf("Choice[%d]=%d Value=%llx Mask=%llx verifying compatibility with previous Value=%llx Mask=%llx\n",
			ChoiceNr, Choice[ChoiceNr], Value, Mask, Constraint[ChoiceNr-1].Value, Constraint[ChoiceNr-1].Mask);
#endif
					if ((Value & Mask & Constraint[ChoiceNr-1].Mask) !=
							(Constraint[ChoiceNr-1].Value & Mask & Constraint[ChoiceNr-1].Mask))
						continue;
				}
				Constraint[ChoiceNr].Value = Constraint[ChoiceNr-1].Value | Value;
				Constraint[ChoiceNr].Mask  = Constraint[ChoiceNr-1].Mask  | Mask;
			}
			else
			{
				Constraint[0].Value = Value;
				Constraint[0].Mask  = Mask;
			}
#ifdef DEBUG
	printf("OK Choice[%d]=%d makes Value=%llx Mask=%llx\n",
			ChoiceNr, Choice[ChoiceNr], Constraint[ChoiceNr].Value, Constraint[ChoiceNr].Mask);
#endif
			// Have we reached the Target ?
			if (ChoiceNr == NrOfCodes-1)
			{
				if (!((Constraint[ChoiceNr].Value >> (3*NrOfCodes))&0xFF))
				{
					printf("Found solution %llu = 0x%llX\n", Constraint[ChoiceNr].Value, Constraint[ChoiceNr].Value);
					if ((!Min) || (Min > Constraint[ChoiceNr].Value))
						Min = Constraint[ChoiceNr].Value;
				}
				continue;
			}
			else
				// All conditions met to move to a further node of the tree
				Choice[++ChoiceNr] = -1;
		} /* Choice advance was possible */
		else
		{
			// If all Choices have been exhausted at this node, we need to backtrack
			if (--ChoiceNr < 0)
				// When backtracking goes past the root of the tree, the search is complete
				break; // from forever loop
		}

  ///////////////////////////////    O u t p u t    /////////////////////////////

	printf("\nMinimum solution found:  %llu = 0x%llX\n", Min, Min);

	return 0;
}
