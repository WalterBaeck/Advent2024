#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_05_1 <.dat file>\n");
    exit(1);
  }

  FILE* InputFile;
  char InputLine[1000];
  char* InputPtr;
  int InputLineNr=0, InputNr, NrOfInputs;

  ////////////////////////////////    I n p u t    //////////////////////////////

  int Rule[1200][2];
	int RuleNr=0, NrOfRules, DoingRules=1;
	int FieldNr;
	char* FieldStr;

  if (!(InputFile = fopen(Argument[1], "r")))
  {
    fprintf(stderr, "Could not open %s for reading.\n", Argument[1]);
    exit(2);
  }
  while (fgets(InputLine, 1000, InputFile))
  {
    ++InputLineNr;
		// Strip terminating newline
		while ((InputLine[strlen(InputLine)-1] == '\n') || (InputLine[strlen(InputLine)-1] == '\r'))
			InputLine[strlen(InputLine)-1] = '\0';
		// Look for blank line marking the end of Rules
		if (!strlen(InputLine))
		{
			DoingRules = 0;
			NrOfRules = RuleNr;
			break;
		}
		for (FieldNr=0; FieldNr<2; FieldNr++)
		{
			FieldStr = strtok((FieldNr ? NULL : InputLine), "|");
      if (1 != sscanf(FieldStr, "%d", &Rule[RuleNr][FieldNr]))
      {
        fprintf(stderr, "Could not parse Rule[%d][%d] : %s as int\n",
            RuleNr, FieldNr, FieldStr);
        exit(2);
      }
    } /* for (FieldNr) */
		RuleNr++;
	} /* while (fgets) */

  ////////////////////////////    O p e r a t i o n    //////////////////////////

	int Page[30];
	int NrOfFields;
	int RuleOccurs[2],RulePart,Sum=0;

  while (fgets(InputLine, 1000, InputFile))
  {
    ++InputLineNr;
		// Strip terminating newline
		while ((InputLine[strlen(InputLine)-1] == '\n') || (InputLine[strlen(InputLine)-1] == '\r'))
			InputLine[strlen(InputLine)-1] = '\0';
		// Parse all the pages mentioned on this line
		FieldNr = 0;
		while (FieldStr = strtok((FieldNr ? NULL : InputLine), ","))
		{
      if (1 != sscanf(FieldStr, "%d", &Page[FieldNr]))
      {
        fprintf(stderr, "Could not parse LineNr %d field %d : %s as int\n",
            InputLineNr, FieldNr, FieldStr);
        exit(2);
      }
			if (++FieldNr > 30)
			{
				fprintf(stderr, "More than 30 page numbers on Line #%d:\n%s",
						InputLineNr, InputLine);
				exit(3);
			}
    } /* while (strtok) */
		NrOfFields = FieldNr;

		// Go through all the known Rules, and see if this set of Pages complies with each Rule
		for (RuleNr=0; RuleNr<NrOfRules; RuleNr++)
		{
			for (RulePart=0; RulePart<2; RulePart++)
			{
				RuleOccurs[RulePart] = -1;
				for (FieldNr=0; FieldNr<NrOfFields; FieldNr++)
					if (Page[FieldNr] == Rule[RuleNr][RulePart])
					{
						RuleOccurs[RulePart] = FieldNr;
						break;
					}
			} /* for (RulePart) */
			// When both sides of the Rule apply to this line of Pages, verify compliance
			if ((RuleOccurs[0] >= 0) && (RuleOccurs[1] >= 0))
				if (RuleOccurs[0] > RuleOccurs[1])  // Violation !
					break;
		}
		// Did we not exit early from the loop of RuleNrs on an InputLine ?
		if (RuleNr >= NrOfRules)
			// Need to find the middle Page and add it to the running Sum
			Sum += Page[NrOfFields/2];

  } /* while (fgets) */
  printf("%d InputLines were read.\n", InputLineNr);


  ///////////////////////////////    O u t p u t    /////////////////////////////

  printf("Found sum of middle complying pages %d\n", Sum);
  return 0;
}

