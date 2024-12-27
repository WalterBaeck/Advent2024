#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_23_2 <.dat file>\n");
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
  int MaxLinks=0;

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
      if (NrOfLinks[FirstId] >= MaxLinks)
        MaxLinks = NrOfLinks[FirstId];
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
      if (NrOfLinks[IdNr] >= MaxLinks)
        MaxLinks = NrOfLinks[IdNr];
    } /* for (PairNr) */
  } /* while (fgets) */
	fclose(InputFile);
	printf("%d inputlines were read, found %d IDs with MaxLinks=%d\n", InputLineNr, NrOfIds, MaxLinks);

  ////////////////////////////    O p e r a t i o n    //////////////////////////

  int Index[20], IndexNr, PrevNr, MaxSize=0, Array[20], MaxArray[20];

  for (FirstId=0; FirstId<NrOfIds; FirstId++)
  {
    // For an N-cluster to occur, any single member should have N-1 connections in its list.
    // So go through all FirstIds, and within each, consider all possible groupings
    //   of its connections. No partner #ID should ever be lower than the FirstId.
    //
    // First member of group has position Index[0] within FirstId's connection list
    Index[0] = -1;
    IndexNr = 0;                             // Trying to expand group to next member
    for (;;)
    {
#ifdef DEBUG
      printf("Index[%d]: Trying to go to connection #%d of %d for FirstId=%d\n",
          IndexNr, Index[IndexNr]+1, NrOfLinks[FirstId], FirstId);
#endif
      // Always try to find a viable next value for Index[IndexNr]
      if (++Index[IndexNr] >= NrOfLinks[FirstId])
        // No further group expansion possible anymore
        if (--IndexNr < 0)
          break; /* from for(;;) */
        else
          continue;
      // So, the new value of Index[IndexNr] is a candidate to expand the current group.
      Array[IndexNr] = Link[FirstId][Index[IndexNr]];  // Actual ID of this candidate
#ifdef DEBUG
      printf(" this connection #%d of FirstId[%d] points to %d\n",
          Index[IndexNr], FirstId, Array[IndexNr]);
#endif
      if (Array[IndexNr] < FirstId)  continue;
      // The requirement is that this candidate should be linked
      //   to all existing members of the group
      for (PrevNr=0; PrevNr<IndexNr; PrevNr++)
      {
#ifdef DEBUG
        printf("  verifying that Prev[%d]=%d is connected to %d ..\n",
            PrevNr, Array[PrevNr], Array[IndexNr]);
#endif
        for (SearchNr=0; SearchNr<NrOfLinks[Array[PrevNr]]; SearchNr++)
          if (Link[Array[PrevNr]][SearchNr] == Array[IndexNr])  break;
        if (SearchNr >= NrOfLinks[Array[PrevNr]])  break;
      }
      if (PrevNr < IndexNr)  continue;   // Requirement not met
      // So, we have succeeded in extending the current group with one more member.
      if (IndexNr+2 > MaxSize)
      {
        MaxSize = IndexNr+2;
        MaxArray[0] = FirstId;
        for (PrevNr=0; PrevNr<=IndexNr; PrevNr++)
          MaxArray[PrevNr+1] = Array[PrevNr];
      }
      // Then, the search continues to add yet another member to the group
      Index[IndexNr+1] = Index[IndexNr];
      IndexNr++;
    } /* for(;;) */
  } /* for (FirstId) */

  ///////////////////////////////    O u t p u t    /////////////////////////////

  printf("Found MaxSize=%d: %s", MaxSize, Id[MaxArray[0]]);
  for (IndexNr=1; IndexNr<MaxSize; IndexNr++)  printf(",%s", Id[MaxArray[IndexNr]]);
  putchar('\n');

  // Sort alphabetically
  for (IndexNr=0; IndexNr<MaxSize; IndexNr++)  Array[IndexNr] = 1;
  for (IndexNr=0; IndexNr<MaxSize; IndexNr++)
  {
    for (SearchNr=0; SearchNr<MaxSize; SearchNr++)
      if (Array[SearchNr])  { FirstId = SearchNr;  break; }
#ifdef DEBUG
    printf("Alfasort[%d]: starting with MaxArray[%d]=%d\n", IndexNr, FirstId, MaxArray[FirstId]);
#endif
    for (SearchNr++; SearchNr<MaxSize; SearchNr++)
      if ((Array[SearchNr]) && (strcmp(Id[MaxArray[SearchNr]], Id[MaxArray[FirstId]]) < 0))
        FirstId = SearchNr;
#ifdef DEBUG
    printf("Alfasort[%d]: ending with FirstId=%d\n", IndexNr, FirstId);
#endif
    if (IndexNr)  putchar(',');
    printf("%s", Id[MaxArray[FirstId]]);
    Array[FirstId] = 0;
  } /* for (IndexNr) */
  putchar('\n');

  return 0;
}
