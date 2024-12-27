#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_09_1 <.dat file>\n");
    exit(1);
  }

  FILE* InputFile;
  char Input[20000];
  int InputLen;

  ////////////////////////////////    I n p u t    //////////////////////////////

  if (!(InputFile = fopen(Argument[1], "r")))
  {
    fprintf(stderr, "Could not open %s for reading.\n", Argument[1]);
    exit(2);
  }
	if (!(InputLen = fread(Input, 1, 20000, InputFile)))
	{
		fprintf(stderr, "Could not read from %s\n", Argument[1]);
		exit(2);
	}
	else
		printf("%d characters read from %s.\n", InputLen, Argument[1]);

  ////////////////////////////    O p e r a t i o n    //////////////////////////

	int *Disk, File=0, InputPos, DiskPos=0, DiskLen;
	int Blank=0, Index, Len;

	// First parse the Input into a Disk image
	if (!(Disk = malloc(10 * InputLen * sizeof(int))))
	{
		fprintf(stderr, "Could not allocate Disk of %d integers\n", 10 * InputLen);
		exit(3);
	}
	for (InputPos=0; InputPos<InputLen; InputPos++)
	{
		if ((Input[InputPos] == '\r') || (Input[InputPos] == '\n'))  break;
		if (Input[InputPos] == '\0')                                 break;
		if ((Input[InputPos] < '0') || (Input[InputPos] > '9'))
		{
			fprintf(stderr, "Unrecognised digit %c at InputPos %d\n", Input[InputPos], InputPos);
			exit(4);
		}
		Len = Input[InputPos] - '0';
		for (Index=0; Index<Len; Index++)
			Disk[DiskPos++] = (Blank ? -1 : File);
		Blank = !Blank;
		if (Blank)  File++;
	}
	DiskLen = DiskPos;

#ifdef DEBUG
	printf("+++ Initial situation +++\n");
  for (DiskPos=0; DiskPos<DiskLen; DiskPos++)
		if (Disk[DiskPos] < 0)
			putchar('.');
		else
			putchar('0' + Disk[DiskPos]);
	putchar('\n');
#endif

	// Then perform a repeated transfer from the far end to the firstmost blank position
	int FreePos=0, EndPos=DiskLen-1;
	for (;;)
	{
		while ((FreePos < DiskLen) && (Disk[FreePos] >= 0))  FreePos++;
		while ((EndPos >= 0) && (Disk[EndPos] < 0))  EndPos--;
		if (FreePos >= EndPos)  break;
		// Perform the transfer
		Disk[FreePos] = Disk[EndPos];
		Disk[EndPos] = -1;
	}

#ifdef DEBUG
	printf("+++ After defragmentation +++\n");
  for (DiskPos=0; DiskPos<DiskLen; DiskPos++)
		if (Disk[DiskPos] < 0)
			putchar('.');
		else
			putchar('0' + Disk[DiskPos]);
	putchar('\n');
#endif

	// Finally, compute the checksum
	unsigned long long MulSum=0, Mul;
  for (DiskPos=0; DiskPos<DiskLen; DiskPos++)
	{
		if (Disk[DiskPos] < 0)  break;
		Mul = (unsigned long long)DiskPos * Disk[DiskPos];
		MulSum += Mul;
	}

  ///////////////////////////////    O u t p u t    /////////////////////////////

  printf("Found MulSum = %llu\n", MulSum);
  return 0;
}

