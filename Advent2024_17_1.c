#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_17_1 <.dat file>\n");
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

	int FirstOutput=1;
	for (CodeNr=0; CodeNr<NrOfCodes; CodeNr+=2)
	{
		// Parse Operand
		if ((Code[CodeNr]==1) || (Code[CodeNr]==3) || (Code[CodeNr]==4))
			Operand = (unsigned long long)Code[CodeNr+1];
		else
			switch (Code[CodeNr+1])
			{
				case 4: Operand = A;  break;
				case 5: Operand = B;  break;
				case 6: Operand = C;  break;
				case 7:
					fprintf(stderr, "Illegal operand 7 in Code #%d\n", CodeNr+1);
					exit(5);
				default: Operand = (unsigned long long)Code[CodeNr+1];
			}
		// Parse OpCode
		switch (Code[CodeNr])
		{
			case 0: //adv
				A >>= Operand;  break;
			case 1: //bxl
				B ^= Operand;   break;
			case 2: //bst
				B = Operand&7;  break;
			case 3: //jnz
				if (A) CodeNr=(int)Operand-2;  break;
			case 4: //bxc
				B ^= C;         break;
			case 5:
				if (FirstOutput)  FirstOutput=0;  else putchar(',');
				printf("%hhu", Operand&7);     break;
			case 6: //bdv
				B = A>>Operand; break;
			case 7: //cdv
				C = A>>Operand; break;
		}
	}

	return 0;
}
