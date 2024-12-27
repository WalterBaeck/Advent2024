#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_11_2 <.dat file>\n");
    exit(1);
  }

	//////////////////////////    H a s h t a b l e s     /////////////////////////

	unsigned long long *Value,*NewValue;
	int NrOfValues=0, ValueNr, NewValueNr;
	unsigned long long *Amount, *NewAmount;
	Value = (unsigned long long*)malloc(10000 * sizeof(unsigned long long));
	NewValue = (unsigned long long*)malloc(10000 * sizeof(unsigned long long));
	Amount = (unsigned long long*)malloc(10000 * sizeof(unsigned long long));
	int Index;

	void AddNewValue(unsigned long long newValue, unsigned long long newAmount)
	{
		for (Index=0; Index<NewValueNr; Index++)
			if (NewValue[Index] == newValue)  break;
		if (Index < NewValueNr)
			NewAmount[Index] += newAmount;
		else
		{
			NewAmount[NewValueNr] = newAmount;
			NewValue[NewValueNr] = newValue;
			if (++NewValueNr >= 10000)
			{
				fprintf(stderr, "Hashmap overflow >10K for newVal=%llu newAmnt=%llu\n",
						newValue, newAmount);
				exit(7);
			}
		}
	} /* void AddNewValue(ull newValue, ull newAmount) */

  ////////////////////////////////    I n p u t    //////////////////////////////

  FILE* InputFile;
  char InputLine[1000];
  char* InputPtr;

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

	ValueNr = 0;
	while (InputPtr = strtok((ValueNr ? NULL : InputLine), " "))
	{
		if (1 != sscanf(InputPtr, "%llu", &Value[ValueNr]))
		{
			fprintf(stderr, "Could not parse field %d : %s as ull\n",
					ValueNr, InputPtr);
			exit(2);
		}
		Amount[ValueNr] = 1;
		if (++ValueNr > 10)
		{
			fprintf(stderr, "More than 10 input numbers:\n%s", InputLine);
			exit(3);
		}
	} /* while (strtok) */
	NrOfValues = ValueNr;
  printf("%d Items were read.\n", NrOfValues);

  ////////////////////////////    O p e r a t i o n    //////////////////////////

	int Iteration, Len;
	char Str[30];
	unsigned long long Rule[3], SplitPart;
	unsigned long long SumAmount;

	for (Iteration=0; Iteration<75; Iteration++)
	{
		memset(Rule, 0, 3*sizeof(unsigned long long));
		NewValueNr = 0;
		for (ValueNr=0; ValueNr<NrOfValues; ValueNr++)
		{
			// Apply the 3 rules in consecutive order
			if (!Value[ValueNr])
			{
				AddNewValue(1, Amount[ValueNr]);
				Rule[0] = Amount[ValueNr];
				continue;
			}
			sprintf(Str, "%llu", Value[ValueNr]);
			Len = strlen(Str);
			if (!(Len & 1))
			{
				sscanf(Str+(Len/2), "%llu", &SplitPart);
				AddNewValue(SplitPart, Amount[ValueNr]);
				Str[Len/2] = '\0';
				sscanf(Str, "%llu", &SplitPart);
				AddNewValue(SplitPart, Amount[ValueNr]);
				Rule[1] += Amount[ValueNr];
			}
			else
			{
				SplitPart = Value[ValueNr] * 2024;
				if (SplitPart < Value[ValueNr])
				{
					fprintf(stderr, "ull overflow %llu x 2024 = %llu ??\n", Value[ValueNr], SplitPart);
					exit(5);
				}
				AddNewValue(SplitPart, Amount[ValueNr]);
				Rule[2] += Amount[ValueNr];
			}
		} /* for (ValueNr) */

		// Now copy all NewValues back to the Value array, which has been processed through
		NrOfValues = NewValueNr;
		memcpy(Value, NewValue, NrOfValues * sizeof(unsigned long long));
		memcpy(Amount, NewAmount, NrOfValues * sizeof(unsigned long long));

#ifdef DEBUG
		SumAmount = 0;
		for (ValueNr=0; ValueNr<NrOfValues; ValueNr++)
			SumAmount += Amount[ValueNr];
		printf("After iteration #%2d, NrOfValues %4d   SumAmount %15llu items  -  %15llu[0] %15llu[1] %15llu[2]\n",
				Iteration, NrOfValues, SumAmount, Rule[0], Rule[1], Rule[2]);
#endif
	} /* for (Iteration) */

  ///////////////////////////////    O u t p u t    /////////////////////////////

	SumAmount = 0;
	for (ValueNr=0; ValueNr<NrOfValues; ValueNr++)
		SumAmount += Amount[ValueNr];
  printf("Ended up with SumAmount %llu\n", SumAmount);
  return 0;
}
