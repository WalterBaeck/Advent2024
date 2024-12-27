#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_22_1 <.dat file>\n");
    exit(1);
  }

  FILE* InputFile;
  char InputLine[1000];
  int InputLineNr=0;
  int Seed[3000];
  int SeedNr, NrOfSeeds;
  unsigned Step, Secret;
	unsigned long long Sum=0;

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

  ////////////////////////////    O p e r a t i o n    //////////////////////////

  for (SeedNr=0; SeedNr<NrOfSeeds; SeedNr++)
  {
    Secret = Seed[SeedNr];
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
#ifdef DEBUG
    if (Step < 10)
      printf("Seed[%d]=%d after Step[%d]: %u\n", SeedNr, Seed[SeedNr], Step, Secret);
#endif
    } /* for (Step) */
    Sum += Secret;
  } /* for (SeedNr) */

  ///////////////////////////////    O u t p u t    /////////////////////////////

	printf("\nFound Sum %llu\n", Sum);
  return 0;
}
