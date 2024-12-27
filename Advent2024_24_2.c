#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

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

  // The circuit uses 5 gates for every pair of input bits :
  //
  // x[n] XOR y[n] => HalfAdd[n]
  //   Carry[n-1] XOR HalfAdd[n] => z[n]
  //   Carry[n-1] AND HalfAdd[n] => FullAdd[n]
  // x[n] AND y[n] => Overflow[n]
  //                  Overflow[n] OR FullAdd[n] => Carry[n]
  
  // 45 pairs of inputbits are being added into 46 outputbits
  // Aside from bits [0] and [45], the regular pattern should be detected.

  int BitNr, HalfAdd[50], FullAdd[50], Overflow[50], OutputZ[50], Carry[50];
  int InputX,InputY;

  // Special case for Bit[0]
  {
    BitNr = 0;

    // No anomalies in the inputbits: perfectly predictable
    InputX = BitNr;   InputY = BitNr+45;

    // Look for the HalfAdder : XOR between both inputbits
    for (GateNr=0; GateNr<NrOfGates; GateNr++)
      if ((Operator[GateNr] == eXor) &&
          (((Input[GateNr][0] == InputX) && (Input[GateNr][1] == InputY)) ||
           ((Input[GateNr][1] == InputX) && (Input[GateNr][0] == InputY))))  break;
    if (GateNr >= NrOfGates)
    {
      fprintf(stderr, "Could not find HalfAdd[%d] : x%02d XOR y%02d ?\n",BitNr,BitNr,BitNr);
      exit(6);
    }
    else
    {
      HalfAdd[BitNr] = Output[GateNr];
      // Exceptionally, this HalfAdder will become OutputZ[0]
      OutputZ[BitNr] = HalfAdd[BitNr];
      sprintf(Third, "z%02d", BitNr);  // Expected name
      if (strcmp(Name[OutputZ[BitNr]], Third))
        printf("Anomaly: OutputZ[%d] should be named %s instead of %s\n",
            BitNr, Third, Name[OutputZ[BitNr]]);
    }

    // Look for the Overflow : AND between both inputs
    for (GateNr=0; GateNr<NrOfGates; GateNr++)
      if ((Operator[GateNr] == eAnd) &&
          (((Input[GateNr][0] == InputX) && (Input[GateNr][1] == InputY)) ||
           ((Input[GateNr][1] == InputX) && (Input[GateNr][0] == InputY))))  break;
    if (GateNr >= NrOfGates)
    {
      fprintf(stderr, "Could not find Overflow[%d] : x%02d AND y%02d ?\n",BitNr,BitNr,BitNr);
      exit(6);
    }
    Overflow[BitNr] = Output[GateNr];
    if (Name[Overflow[BitNr]][0] == 'z')
      printf("Anomaly: Overflow[%d] should not be connected directly to output %s\n",
          BitNr, Name[Overflow[BitNr]]);
    // Exceptionally, this Overflow will become Carry[0]
    Carry[BitNr] = Overflow[BitNr];

  } /* Bit[0] */

  for (BitNr=1; BitNr<45; BitNr++)
  {
    // No anomalies in the inputbits: perfectly predictable
    InputX = BitNr;   InputY = BitNr+45;

    // Look for the HalfAdder : XOR between both inputbits
    for (GateNr=0; GateNr<NrOfGates; GateNr++)
      if ((Operator[GateNr] == eXor) &&
          (((Input[GateNr][0] == InputX) && (Input[GateNr][1] == InputY)) ||
           ((Input[GateNr][1] == InputX) && (Input[GateNr][0] == InputY))))  break;
    if (GateNr >= NrOfGates)
    {
      fprintf(stderr, "Could not find HalfAdd[%d] : x%02d XOR y%02d ?\n",BitNr,BitNr,BitNr);
      exit(6);
    }
    HalfAdd[BitNr] = Output[GateNr];
    if (Name[HalfAdd[BitNr]][0] == 'z')
      printf("Anomaly: HalfAdd[%d] should not be connected directly to output %s\n",
          BitNr, Name[HalfAdd[BitNr]]);

    // Look for the Overflow : AND between both inputs
    for (GateNr=0; GateNr<NrOfGates; GateNr++)
      if ((Operator[GateNr] == eAnd) &&
          (((Input[GateNr][0] == InputX) && (Input[GateNr][1] == InputY)) ||
           ((Input[GateNr][1] == InputX) && (Input[GateNr][0] == InputY))))  break;
    if (GateNr >= NrOfGates)
    {
      fprintf(stderr, "Could not find Overflow[%d] : x%02d AND y%02d ?\n",BitNr,BitNr,BitNr);
      exit(6);
    }
    Overflow[BitNr] = Output[GateNr];
    if (Name[Overflow[BitNr]][0] == 'z')
      printf("Anomaly: Overflow[%d] should not be connected directly to output %s\n",
          BitNr, Name[Overflow[BitNr]]);

    // Look for the OutputZ : XOR between previous Carry and HalfAdd
    for (GateNr=0; GateNr<NrOfGates; GateNr++)
      if ((Operator[GateNr] == eXor) &&
          (((Input[GateNr][0] == HalfAdd[BitNr]) && (Input[GateNr][1] == Carry[BitNr-1])) ||
           ((Input[GateNr][1] == HalfAdd[BitNr]) && (Input[GateNr][0] == Carry[BitNr-1]))))  break;
    if (GateNr >= NrOfGates)
    {
      printf("Anomaly: Could not find Output[%d] : HalfAdd[%s] XOR Carry[%s]\n",
          BitNr, Name[HalfAdd[BitNr]], Name[Carry[BitNr]]);
      OutputZ[BitNr] = 0;  // Dummy
    }
    else
    {
      OutputZ[BitNr] = Output[GateNr];
      sprintf(Third, "z%02d", BitNr);  // Expected name
      if (strcmp(Name[OutputZ[BitNr]], Third))
        printf("Anomaly: OutputZ[%d] should be named %s instead of %s\n",
            BitNr, Third, Name[OutputZ[BitNr]]);
    }

    // Look for the FullAdder : AND between previous Carry and HalfAdd
    for (GateNr=0; GateNr<NrOfGates; GateNr++)
      if ((Operator[GateNr] == eAnd) &&
          (((Input[GateNr][0] == HalfAdd[BitNr]) && (Input[GateNr][1] == Carry[BitNr-1])) ||
           ((Input[GateNr][1] == HalfAdd[BitNr]) && (Input[GateNr][0] == Carry[BitNr-1]))))  break;
    if (GateNr >= NrOfGates)
    {
      printf("Anomaly: Could not find FullAdd[%d] : HalfAdd[%s] AND Carry[%s]\n",
          BitNr, Name[HalfAdd[BitNr]], Name[Carry[BitNr]]);
      FullAdd[BitNr] = 0;  // Dummy
    }
    else
    {
      FullAdd[BitNr] = Output[GateNr];
      if (Name[FullAdd[BitNr]][0] == 'z')
        printf("Anomaly: FullAdd[%d] should not be connected directly to output %s\n",
            BitNr, Name[FullAdd[BitNr]]);
    }

    // Look for the Carry : OR between Overflow and FullAdd
    for (GateNr=0; GateNr<NrOfGates; GateNr++)
      if ((Operator[GateNr] == eOr) &&
          (((Input[GateNr][0] == FullAdd[BitNr]) && (Input[GateNr][1] == Overflow[BitNr])) ||
           ((Input[GateNr][1] == FullAdd[BitNr]) && (Input[GateNr][0] == Overflow[BitNr]))))  break;
    if (GateNr >= NrOfGates)
    {
      printf("Anomaly: Could not find Carry[%d] : FullAdd[%s] OR Overflow[%s]\n",
          BitNr, Name[FullAdd[BitNr]], Name[Overflow[BitNr]]);
      Carry[BitNr] = 0;  // Dummy
    }
    else
    {
      Carry[BitNr] = Output[GateNr];
      if ((Name[Carry[BitNr]][0] == 'z') ^ (BitNr == 44))
        printf("Anomaly: Carry[%d] should not be connected directly to output %s\n",
            BitNr, Name[Carry[BitNr]]);
    }
  } /* for (BitNr) */

  ///////////////////////////////    O u t p u t    /////////////////////////////

  return 0;
}
