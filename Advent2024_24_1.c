#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define DEBUG

int main(int Arguments, char* Argument[])
{
  if (Arguments != 2)
  {
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "Advent2024_24_1 <.dat file>\n");
    exit(1);
  }

  FILE* InputFile;
  char InputLine[1000];
  int InputLineNr=0;

  char Name[500][4];
  unsigned char Value[500], Known[500] = {0};

  int FirstNr, SecondNr, ThirdNr, NodeNr, NrOfNodes=0;
  char Str, First[4],Second[4],Third[4],Fourth[4];

  int GateNr, NrOfGates=0;
  int Input[300][2], Output[300];
  typedef enum {
    eAnd, eOr, eXor,
  } tOperator;
  tOperator Operator[300];

  ////////////////////////////////    I n p u t    //////////////////////////////

  if (!(InputFile = fopen(Argument[1], "r")))
  {
    fprintf(stderr, "Could not open %s for reading.\n", Argument[1]);
    exit(2);
  }
  // Input values
	while (fgets(InputLine, 1000, InputFile))
  {
    InputLineNr++;
    // Strip terminating newline
    while ((InputLine[strlen(InputLine)-1]=='\n') || (InputLine[strlen(InputLine)-1]=='\r'))
      InputLine[strlen(InputLine)-1] = '\0';
    if (!strlen(InputLine))  break;

    if (1 != sscanf(strtok(InputLine, ":"), "%s", Name + NrOfNodes))
    {
      fprintf(stderr, "Could not scan InputName in #%d: %s\n",
          InputLineNr, InputLine);
      exit(3);
    }
    if (1 != sscanf(strtok(NULL, ": "), "%hhu", Value + NrOfNodes))
    {
      fprintf(stderr, "Could not scan InputValue in #%d: %s\n",
          InputLineNr, InputLine);
      exit(3);
    }
    Known[NrOfNodes++] = 1;
  } /* while (fgets) */
  // Gates
	while (fgets(InputLine, 1000, InputFile))
  {
    InputLineNr++;
    // Strip terminating newline
    while ((InputLine[strlen(InputLine)-1]=='\n') || (InputLine[strlen(InputLine)-1]=='\r'))
      InputLine[strlen(InputLine)-1] = '\0';
    if (1 != sscanf(strtok(InputLine, " "), "%s", First))
    {
      fprintf(stderr, "Could not scan Node1 in #%d: %s\n",
          InputLineNr, InputLine);
      exit(3);
    }
    if (1 != sscanf(strtok(NULL, " "), "%s", Fourth))
    {
      fprintf(stderr, "Could not scan Op in #%d: %s\n",
          InputLineNr, InputLine);
      exit(3);
    }
    if (1 != sscanf(strtok(NULL, " "), "%s", Second))
    {
      fprintf(stderr, "Could not scan Node2 in #%d: %s\n",
          InputLineNr, InputLine);
      exit(3);
    }
    if (1 != sscanf(strtok(NULL, " ->"), "%s", Third))
    {
      fprintf(stderr, "Could not scan Out in #%d: %s\n",
          InputLineNr, InputLine);
      exit(3);
    }
    // Search for Input1
    for (FirstNr=0; FirstNr<NrOfNodes; FirstNr++)
      if (!strcmp(First, Name[FirstNr]))  break;
    if (FirstNr >= NrOfNodes)
      strcpy(Name[NrOfNodes++], First);
    Input[NrOfGates][0] = FirstNr;
    // Search for Input2
    for (SecondNr=0; SecondNr<NrOfNodes; SecondNr++)
      if (!strcmp(Second, Name[SecondNr]))  break;
    if (SecondNr >= NrOfNodes)
      strcpy(Name[NrOfNodes++], Second);
    Input[NrOfGates][1] = SecondNr;
    // Search for Output
    for (ThirdNr=0; ThirdNr<NrOfNodes; ThirdNr++)
      if (!strcmp(Third, Name[ThirdNr]))  break;
    if (ThirdNr >= NrOfNodes)
      strcpy(Name[NrOfNodes++], Third);
    Output[NrOfGates] = ThirdNr;
    // Parse Operator
    if      (!strcmp(Fourth, "AND"))  Operator[NrOfGates] = eAnd;
    else if (!strcmp(Fourth,  "OR"))  Operator[NrOfGates] = eOr;
    else if (!strcmp(Fourth, "XOR"))  Operator[NrOfGates] = eXor;
    else
    {
      fprintf(stderr, "Could not parse Operator #%d: %s\n",
          InputLineNr, InputLine);
      exit(5);
    }
    NrOfGates++;
  } /* while (fgets) */
	fclose(InputFile);
	printf("%d inputlines were read, found %d Nodes and %d Gates\n",
      InputLineNr, NrOfNodes, NrOfGates);

  ////////////////////////////    O p e r a t i o n    //////////////////////////

  int Index[20], IndexNr, PrevNr, MaxSize=0, Array[20], MaxArray[20];
  for (;;)
  {
    int KnownMoreGates = 0;
    for (GateNr=0; GateNr<NrOfGates; GateNr++)
      if ((!Known[Output[GateNr]]) &&
          (Known[Input[GateNr][0]]) && (Known[Input[GateNr][1]]))
      {
        switch (Operator[GateNr])
        {
          case eAnd:
            Value[Output[GateNr]] = Value[Input[GateNr][0]] & Value[Input[GateNr][1]];
            break;
          case eOr:
            Value[Output[GateNr]] = Value[Input[GateNr][0]] | Value[Input[GateNr][1]];
            break;
          case eXor:
            Value[Output[GateNr]] = Value[Input[GateNr][0]] ^ Value[Input[GateNr][1]];
            break;
        } /* switch (Operator) */
        Known[Output[GateNr]] = 1;
        KnownMoreGates = 1;
      } /* if (!Output && Inputs) */
    // After the loop has searched for more results :
    if (!KnownMoreGates)  break;  /* from for(;;) */
  } /* for(;;) */

  ///////////////////////////////    O u t p u t    /////////////////////////////

  int BitNr;
  unsigned long long Word=0;
  for (BitNr=0; ; BitNr++)
  {
    sprintf(Third, "z%02d", BitNr);
    for (ThirdNr=0; ThirdNr<NrOfNodes; ThirdNr++)
      if (!strcmp(Third, Name[ThirdNr]))  break;
    if (ThirdNr >= NrOfNodes)  break;
    if (!Known[ThirdNr])
    {
      fprintf(stderr, "Name[%d] %s but value is not Known\n", ThirdNr, Name[ThirdNr]);
      exit(6);
    }
    Word |= ((unsigned long long)Value[ThirdNr]<<BitNr);
  } /* for (BitNr) */

  printf("After %d Bits, found Word %llX = %llu\n", BitNr, Word, Word);
  return 0;
}
