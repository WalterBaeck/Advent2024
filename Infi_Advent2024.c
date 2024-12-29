#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
	ePushReg,
	ePush,
	eAdd,
	eJump,
	eRet,
} tOp;

int main(int Arguments, char* Argument[])
{
	FILE* InputFile;
	char InputLine[100];
	int InputLineNr=0;

	tOp *Code;
	int *Arg;
	int *Stack;

	int PC=0,StackPtr;

	// Initialize memory
	if (!(Code = (tOp*)malloc(200000 * sizeof(tOp))))
	{
		fprintf(stderr, "Could not allocate program memory for 200K instructions.\n");
		exit(2);
	}
	if (!(Arg = (int*)malloc(200000 * sizeof(int))))
	{
		fprintf(stderr, "Could not allocate argument memory for 200K operands.\n");
		exit(2);
	}
	if (!(Stack = (int*)malloc(100000 * sizeof(int))))
	{
		fprintf(stderr, "Could not allocate 100K stack space\n");
		exit(2);
	}

	// Read the program into memory
	if (!(InputFile = fopen(Argument[1], "r")))
	{
		fprintf(stderr, "Could not open %s for reading.\n", Argument[1]);
		exit(1);
	}
	while (fgets(InputLine, 100, InputFile))
	{
		if (!strncmp(InputLine, "push ", 5))
		{
			if ((InputLine[5] >= 'X') && (InputLine[5] <= 'Z'))
			{
				Code[PC] = ePushReg;
				Arg[PC] = InputLine[5] - 'X';
			}
			else
			{
				Code[PC] = ePush;
				if (1 != sscanf(InputLine+5, "%d", &Arg[PC]))
				{
					fprintf(stderr, "Could not scan argument from InputLine #%d: %s\n",
							InputLineNr, InputLine);
					exit(3);
				}
			}
		} /* if (push) */
		else if (!strncmp(InputLine, "add", 3))
			Code[PC] = eAdd;
		else if (!strncmp(InputLine, "jmpos ", 6))
		{
			Code[PC] = eJump;
			if (1 != sscanf(InputLine+6, "%d", &Arg[PC]))
			{
				fprintf(stderr, "Could not scan argument from InputLine #%d: %s\n",
						InputLineNr, InputLine);
				exit(3);
			}
		}
		else if (!strncmp(InputLine, "ret", 3))
			Code[PC] = eRet;
		else
		{
			fprintf(stderr, "Unrecognized opcode on InputLine #%d: %s\n",
					InputLineNr, InputLine);
			exit(3);
		}

		InputLineNr++;
		if (++PC > 200000)
		{
			fprintf(stderr, "More than 200K opcodes on InputLine #%d\n", InputLineNr);
			exit(4);
		}
	} /* while (fgets) */
	printf("%d inputlines were read.\n", InputLineNr);

	// Compute airspace data
	int X,Y,Z;
	int Cube[30][30][30];
	int Sum=0, Operand;
	for (X=0; X<30; X++)
	for (Y=0; Y<30; Y++)
	for (Z=0; Z<30; Z++)
	// Execute the program for these coordinates
	{
		PC=0; StackPtr=0;
		for(;;)
		{
			// Catch Ret instruction outside of switch statement
			if (Code[PC] == eRet)  break;  // from forever loop
			switch (Code[PC])
			{
				case ePushReg:
					switch (Arg[PC])
					{
						case 0: Operand=X; break;
						case 1: Operand=Y; break;
						case 2: Operand=Z; break;
					}
					Stack[StackPtr++] = Operand;
					if (StackPtr >= 100000)
					{
						fprintf(stderr, "Stack overflow for X=%d Y=%d Z=%d at PC=%d\n", X,Y,Z, PC);
						exit(5);
					}
					break;
				case ePush:
					Stack[StackPtr++] = Arg[PC];
					if (StackPtr >= 100000)
					{
						fprintf(stderr, "Stack overflow for X=%d Y=%d Z=%d at PC=%d\n", X,Y,Z, PC);
						exit(5);
					}
					break;
				case eAdd:
					Operand = Stack[StackPtr-1] + Stack[StackPtr-2];
					Stack[StackPtr-2] = Operand;
					StackPtr--;
					break;
				case eJump:
					Operand = Stack[--StackPtr];
					if (Operand >= 0)  PC += Arg[PC];
					break;
			} /* switch (Code) */
			PC++;
		} /* for(;;) */
		Sum += Stack[StackPtr-1];
		if (Stack[StackPtr-1] > 0)  Cube[X][Y][Z] = 1;  else Cube[X][Y][Z] = 0;
	}
	// Display result
	printf("Found sum %d\n", Sum);

int Zval;
for (Zval=0; Zval<30; Zval+=5)
{
	for (Y=0; Y<30; Y++)
		for (Z=Zval; Z<Zval+5; Z++)
		{
			for (X=0; X<30; X++)
				putchar(Cube[X][Y][Z] ? '*' : ' ');
			putchar(Z==Zval+4 ? '\n' : '|');
		}
	for (Z=Zval; Z<Zval+5; Z++)
	{
		for (X=0; X<30; X++)  putchar('-');
		putchar(Z==Zval+4 ? '\n' : '+');
	}
}

	// Gather the Cloud Atlas
	int QueueX[100],QueueY[100],QueueZ[100];
	int ReadPtr,WritePtr;
	int CloudNr=0;
	// Keep trying to find a filled cell
	for(;;)
	{
		for (X=0; X<30; X++)
		{
			for (Y=0; Y<30; Y++)
			{
				for (Z=0; Z<30; Z++)
					if (Cube[X][Y][Z] == 1)  break;
				if (Z<30)  break;
			}
			if (Y<30)  break;
		}
		if (X>=30)  break;  // No more filled cells

		// From a filled cell, map all adjacent cells
printf("Start cell [X=%d,Y=%d,Z=%d]\n", X,Y,Z);
		Cube[X][Y][Z] = 0;
		CloudNr++;
		QueueX[0] = X;  QueueY[0] = Y;  QueueZ[0] = Z;
		WritePtr = 1;  ReadPtr = 0;
		for(;;)
		{
			// Take the oldest cell from the queue, and look for filled neighbors
			int Dir, NextX, NextY, NextZ;
			for (Dir=0; Dir<6; Dir++)
			{
				NextX = QueueX[ReadPtr];  NextY = QueueY[ReadPtr];  NextZ = QueueZ[ReadPtr];
				switch (Dir)
				{
					case 0: NextX--;  break;
					case 1: NextX++;  break;
					case 2: NextY--;  break;
					case 3: NextY++;  break;
					case 4: NextZ--;  break;
					case 5: NextZ++;  break;
				}
				if ((NextX < 0) || (NextX >= 30))  continue;
				if ((NextY < 0) || (NextY >= 30))  continue;
				if ((NextZ < 0) || (NextZ >= 30))  continue;
				if (!Cube[NextX][NextY][NextZ])    continue;
printf("Next cell [X=%d,Y=%d,Z=%d]\n", NextX,NextY,NextZ);
				Cube[NextX][NextY][NextZ] = 0;
				QueueX[WritePtr] = NextX; QueueY[WritePtr] = NextY; QueueZ[WritePtr] = NextZ;
				WritePtr++; WritePtr%=100;
				if (WritePtr == ReadPtr)
				{
					fprintf(stderr, "Queue overflow at [X=%d,Y=%d,Z=%d]\n", NextX,NextY,NextZ);
					exit(5);
				}
			} /* for (Dir) */
			ReadPtr++; ReadPtr%=100;
			if (ReadPtr == WritePtr)  break;  // Queue depleted
		} /* for(;;) */

printf("====================================================================================\n");
printf("  - - - - - - - - - - - - -       CLOUD %2d              - - - - - - - - - - - - - -\n", CloudNr);
printf("====================================================================================\n");
for (Zval=0; Zval<30; Zval+=5)
{
	for (Y=0; Y<30; Y++)
		for (Z=Zval; Z<Zval+5; Z++)
		{
			for (X=0; X<30; X++)
				putchar(Cube[X][Y][Z] ? '*' : ' ');
			putchar(Z==Zval+4 ? '\n' : '|');
		}
	for (Z=Zval; Z<Zval+5; Z++)
	{
		for (X=0; X<30; X++)  putchar('-');
		putchar(Z==Zval+4 ? '\n' : '+');
	}
}

	} /* for(;;) */
	// Display result
	printf("NrOfClouds = %d\n", CloudNr);

	return 0;
}
