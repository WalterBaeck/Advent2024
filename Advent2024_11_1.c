#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_11_1 <.dat file>\n");
    exit(1);
  }

  FILE* InputFile;
  char InputLine[1000];
  char* InputPtr;
  int ItemNr, NrOfItems;

  ////////////////////////////////    I n p u t    //////////////////////////////

	unsigned long long* Item;
	if (!(Item = (unsigned long long*)malloc(1000000 * sizeof(unsigned long long))))
	{
		fprintf(stderr, "Could not allocate 1M unsigned long longs.\n");
		exit(4);
	}
  if (!(InputFile = fopen(Argument[1], "r")))
  {
    fprintf(stderr, "Could not open %s for reading.\n", Argument[1]);
    exit(2);
  }
  if (!fgets(InputLine, 1000, InputFile))
	{
		fprintf(stderr, "Could not read 1 InputLine from %s\n", Argument[1]);
    exit(2);
  }

	ItemNr = 0;
	while (InputPtr = strtok((ItemNr ? NULL : InputLine), " "))
	{
		if (1 != sscanf(InputPtr, "%llu", &Item[ItemNr]))
		{
			fprintf(stderr, "Could not parse field %d : %s as ull\n",
					ItemNr, InputPtr);
			exit(2);
		}
		if (++ItemNr > 10)
		{
			fprintf(stderr, "More than 10 input numbers:\n%s", InputLine);
			exit(3);
		}
	} /* while (strtok) */
	NrOfItems = ItemNr;
  printf("%d Items were read.\n", NrOfItems);

  ////////////////////////////    O p e r a t i o n    //////////////////////////

	int Iteration, Index, Len;
	char Str[30];
	unsigned long long PrevItem;

	for (Iteration=0; Iteration<25; Iteration++)
	{
		for (ItemNr=0; ItemNr<NrOfItems; ItemNr++)
		{
			// Apply the 3 rules in consecutive order
			if (!Item[ItemNr])
			{
				Item[ItemNr] = 1;
				continue;
			}
			PrevItem = Item[ItemNr];
			sprintf(Str, "%llu", Item[ItemNr]);
			Len = strlen(Str);
			if (!(Len & 1))
			{
				// To duplicate one Item into two, need to shift remainder of array out
				for (Index=NrOfItems; Index>ItemNr; Index--)
					Item[Index] = Item[Index-1];
				if (++NrOfItems >= 1000000)
				{
					fprintf(stderr, "More than 1M items at iteration %d!\n", Iteration);
					exit(6);
				}
				sscanf(Str+(Len/2), "%llu", &Item[ItemNr+1]);
				Str[Len/2] = '\0';
				sscanf(Str, "%llu", &Item[ItemNr]);
				ItemNr++;
			}
			else
			{
				Item[ItemNr] *= 2024;
				if (Item[ItemNr] < PrevItem)
				{
					fprintf(stderr, "ull overflow %llu x 2024 = %llu ??\n", PrevItem, Item[ItemNr]);
					exit(5);
				}
			}
		} /* for (ItemNr) */
#ifdef DEBUG
		printf("After iteration #%2d, there are %d items\n", Iteration, NrOfItems);
#endif
	} /* for (Iteration) */

  ///////////////////////////////    O u t p u t    /////////////////////////////

  printf("Ended up with %d items\n", NrOfItems);
  return 0;
}
