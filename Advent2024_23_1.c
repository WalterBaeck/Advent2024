#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_23_1 <.dat file>\n");
    exit(1);
  }

  FILE* InputFile;
  char InputLine[1000];
  int InputLineNr=0;
  char Id[700][3];
  int IdNr, NrOfIds=0, PairNr, FirstId;
  char Pair[2][3], *Str;

  int Link[700][50];
  int LinkNr, NrOfLinks[700]={0}, SearchNr;

	int Sum=0;

  ////////////////////////////////    I n p u t    //////////////////////////////

  if (!(InputFile = fopen(Argument[1], "r")))
  {
    fprintf(stderr, "Could not open %s for reading.\n", Argument[1]);
    exit(2);
  }
	while (fgets(InputLine, 1000, InputFile))
  {
    InputLineNr++;
    // Strip terminating newline
    while ((InputLine[strlen(InputLine)-1]=='\n') || (InputLine[strlen(InputLine)-1]=='\r'))
      InputLine[strlen(InputLine)-1] = '\0';
    for (PairNr=0; PairNr<2; PairNr++)
    {
      if (!(Str = strtok(PairNr ? NULL : InputLine, "-")))
      {
        fprintf(stderr, "Could not get ID #%d on InputLine #%d: %s\n",
            PairNr, InputLineNr, InputLine);
        exit(3);
      }
      if (strlen(Str) != 2)
      {
        fprintf(stderr, "ID[%d] %s has strlen %d!=2 on InputLine #%d: %s\n",
            PairNr, Str, strlen(Str), InputLineNr, InputLine);
        exit(3);
      }
      for (IdNr=0; IdNr<NrOfIds; IdNr++)
        if (!strcmp(Str, Id[IdNr]))  break;
      if (IdNr >= NrOfIds)
      {
        if (NrOfIds >= 700)
        {
          fprintf(stderr, "Encountered more than 700 IDs on InputLine[#%d]: %s\n",
              InputLineNr, InputLine);
          exit(4);
        }
        strcpy(Id[IdNr], Str);
        NrOfIds++;
      }
      if (!PairNr)
      {
        FirstId = IdNr;
        continue;
      }
      // Add this connection to the links of both IDs involved
#ifdef DEBUG
      //printf("Linking ID[%d]%s to ID[%d]%s\n", FirstId, Id[FirstId], IdNr, Id[IdNr]);
#endif
      for (SearchNr=0; SearchNr<NrOfLinks[FirstId]; SearchNr++)
        if (Link[FirstId][SearchNr] == IdNr)
        {
          fprintf(stderr, "InputLine[%d] has connection [%d]%s - [%d]%s that was known: %s\n",
              InputLineNr, FirstId, Id[FirstId], IdNr, Id[IdNr], InputLine);
          exit(5);
        }
      Link[FirstId][NrOfLinks[FirstId]++] = IdNr;
      if (NrOfLinks[FirstId] >= 50)
      {
        fprintf(stderr, "ID[%d]%s has over 50 links\n", FirstId, Id[FirstId]);
        exit(6);
      }
      for (SearchNr=0; SearchNr<NrOfLinks[IdNr]; SearchNr++)
        if (Link[IdNr][SearchNr] == FirstId)
        {
          fprintf(stderr, "InputLine[%d] has connection [%d]%s - [%d]%s that was known: %s\n",
              InputLineNr, IdNr, Id[IdNr], FirstId, Id[FirstId], InputLine);
          exit(5);
        }
      Link[IdNr][NrOfLinks[IdNr]++] = FirstId;
      if (NrOfLinks[IdNr] >= 50)
      {
        fprintf(stderr, "ID[%d]%s has over 50 links\n", IdNr, Id[IdNr]);
        exit(6);
      }
    } /* for (PairNr) */
  } /* while (fgets) */
	fclose(InputFile);
	printf("%d inputlines were read, found %d IDs\n", InputLineNr, NrOfIds);

  ////////////////////////////    O p e r a t i o n    //////////////////////////

  // Investigate every link between 2 IDs, to see if they connect to a common 3rd
  int ThirdId, Search3rd, From2nd;
  for (FirstId=0; FirstId<NrOfIds; FirstId++)
    for (SearchNr=0; SearchNr<NrOfLinks[FirstId]; SearchNr++)
    {
      IdNr = Link[FirstId][SearchNr];
      if (IdNr < FirstId)  continue;  // Otherwise every link is investigated twice
      // We have a pair {FirstId, IdNr} now.
      // Consider all other links from FirstId ..
      for (Search3rd=0; Search3rd<NrOfLinks[FirstId]; Search3rd++)
      {
        if (Search3rd == SearchNr)  continue;
        ThirdId = Link[FirstId][Search3rd];
        // Insist a rising order in IDs, to avoid counting triplets twice
        if (ThirdId < FirstId)  continue;
        if (ThirdId < IdNr)     continue;
        // .. and see whether IdNr also has a link to it
        for (From2nd=0; From2nd<NrOfLinks[IdNr]; From2nd++)
          if (Link[IdNr][From2nd] == ThirdId)
          {
            // Bingo!
            printf("Found triplet %s,%s,%s\n", Id[FirstId], Id[IdNr], Id[ThirdId]);
            if ((Id[FirstId][0]=='t') || (Id[IdNr][0]=='t') || (Id[ThirdId][0]=='t'))
              Sum++;
          }
      } /* for (Search3rd) */
    } /* for (SearchNr) */

  ///////////////////////////////    O u t p u t    /////////////////////////////

	printf("Found Sum %d\n", Sum);
  return 0;
}
