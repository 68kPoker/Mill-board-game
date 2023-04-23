
/* Game logic */

/* Board consists of 3 rings of 8 fields. All rings are connected on
 * odd-indexed fields (counting from upper-left). */

#include <stdio.h>
#include <exec/types.h>

#define ABS(a) ((a)>=0?(a):-(a))

#define PAWNS 9

typedef enum Field
{
    NO_PAWN, /* No pawn */
    WHITE_PAWN,
    BLACK_PAWN
} Field;

Field board[3][8];

WORD whitePawns = PAWNS, blackPawns = PAWNS;

/* Clear board */
void clearBoard(void)
{
    WORD ring, field;

    for (ring = 0; ring < 3; ring++)
    {
        for (field = 0; field < 8; field++)
        {
            board[ring][field] = NO_PAWN;
        }
    }
}

int checkBoard(WORD ring, WORD sibField)
{
    if (sibField < 0)
    {
        sibField += 8;
    }
    else if (sibField >= 8)
    {
        sibField -= 8;
    }

    return(board[ring][sibField]);
}

/* Check for new mills upon given pawn and movement direction */
WORD checkInsertForMills(WORD ring, WORD field)
{
    BOOL ringConnection = field & 1;
    WORD pawn = board[ring][field];
    WORD mills = 0;

    if (!ringConnection)
    {
        /* Corner */
        if (checkBoard(ring, field - 1) == pawn     &&
            checkBoard(ring, field - 2) == pawn)
        {
            mills++;
        }
        if (checkBoard(ring, field + 1) == pawn     &&
            checkBoard(ring, field + 2) == pawn)
        {
            mills++;
        }
    }
    else
    {
        /* Middle */
        if (checkBoard(ring, field - 1) == pawn     &&
            checkBoard(ring, field + 1) == pawn)
        {
            mills++;
        }

        if (checkBoard(0, field) == pawn    &&
            checkBoard(1, field) == pawn    &&
            checkBoard(2, field) == pawn)
        {
            mills++;
        }
    }
    return(mills);
}

BOOL checkMoveForMills(WORD ring, WORD field, WORD srcRing, WORD srcField)
{
    BOOL ringConnection = field & 1;
    WORD pawn = board[ring][field];
    WORD mills = 0;

    if (!ringConnection)
    {
        /* Corner */
        if (checkBoard(ring, field - 1) == pawn     &&
            checkBoard(ring, field - 2) == pawn)
        {
            mills++;
        }
        if (checkBoard(ring, field + 1) == pawn     &&
            checkBoard(ring, field + 2) == pawn)
        {
            mills++;
        }
    }
    else
    {
        /* Middle */
        if (checkBoard(ring, field - 1) == pawn     &&
            checkBoard(ring, field + 1) == pawn)
        {
            mills++;
        }

        if (checkBoard(0, field) == pawn    &&
            checkBoard(1, field) == pawn    &&
            checkBoard(2, field) == pawn)
        {
            mills++;
        }
    }
    return(mills);
}

/* Insert pawn for given player */
BOOL insertPawn(Field pawn, WORD ring, WORD field, WORD *pmills)
{
    WORD mills;

    if (board[ring][field] == NO_PAWN)
    {
        board[ring][field] = pawn;

        /* Check for mills! */
        mills = checkInsertForMills(ring, field);

        if (mills > 0)
        {
            printf("Created %d mills!\n", mills);
        }

        *pmills = mills;
        return(TRUE);
    }
    else
    {
        printf("Pawn already there.\n");
    }
    return(FALSE);
}

WORD fieldDiff(WORD field_1, WORD field_2)
{
    WORD diff = ABS(field_1 - field_2);

    if (diff <= 4)
    {
        return(diff);
    }

    return((WORD)(8 - diff));
}

BOOL movePawn(Field pawn, WORD ring, WORD field, WORD destRing, WORD destField, WORD *pmills)
{
    BOOL ringConnection = field & 1;
    WORD mills;

    if (board[ring][field] == pawn && board[destRing][destField] == NO_PAWN)
    {
        if (!ringConnection)
        {
            if (ring != destRing)
            {
                printf("Illegal move.\n");
                return(FALSE);
            }
            if (fieldDiff(field, destField) != 1)
            {
                printf("Illegal move.\n");
                return(FALSE);
            }
        }
        else
        {
            WORD df = fieldDiff(field, destField);
            WORD dr = ABS(ring - destRing);

            if (dr + df != 1)
            {
                printf("Illegal move.\n");
                return(FALSE);
            }
        }
        board[destRing][destField] = pawn;
        board[ring][field] = NO_PAWN;

        mills = checkMoveForMills(destRing, destField, ring, field);

        if (mills > 0)
        {
            printf("Created %d mills!\n", mills);
            *pmills = mills;
        }
        return(TRUE);
    }
    else
    {
        printf("No proper source pawn or dest occupied.\n");
    }
    return(FALSE);
}

BOOL removePawn(WORD pawn, WORD ring, WORD field)
{
    if (pawn != board[ring][field])
    {
        printf("Remove opponent's pawn.\n");
        return(FALSE);
    }

    if (checkInsertForMills(ring, field) > 0)
    {
        printf("Pawn in a mill!\n");
        return(FALSE);
    }

    board[ring][field] = NO_PAWN;
    if (pawn == WHITE_PAWN)
    {
        whitePawns--;
    }
    else
    {
        blackPawns--;
    }
    return(TRUE);
}

char text[] =
{
    "%c----%c----%c\n"
    "|    |    |\n"
    "| %c--%c--%c |\n"
    "| |  |  | |\n"
    "| | %c%c%c | |\n"
    "%c-%c-%c %c-%c-%c\n"
    "| | %c%c%c | |\n"
    "| |  |  | |\n"
    "| %c--%c--%c |\n"
    "|    |    |\n"
    "%c----%c----%c\n"
};

void printBoard(void)
{
    char sym[] =
    {
        '·', 'o', '*'
    };

    printf(text,
        sym[board[0][0]],
        sym[board[0][1]],
        sym[board[0][2]],
        sym[board[1][0]],
        sym[board[1][1]],
        sym[board[1][2]],
        sym[board[2][0]],
        sym[board[2][1]],
        sym[board[2][2]],
        sym[board[0][7]],
        sym[board[1][7]],
        sym[board[2][7]],
        sym[board[2][3]],
        sym[board[1][3]],
        sym[board[0][3]],
        sym[board[2][6]],
        sym[board[2][5]],
        sym[board[2][4]],
        sym[board[1][6]],
        sym[board[1][5]],
        sym[board[1][4]],
        sym[board[0][6]],
        sym[board[0][5]],
        sym[board[0][4]]);
}

int main(void)
{
    int ring, field;
    BOOL done = FALSE;
    Field pawn = WHITE_PAWN;
    WORD mills = 0;
    WORD turn;

    clearBoard();
    printBoard();

    printf("Insertion phase.\n");

    for (turn = 0; turn < PAWNS * 2; turn++)
    {
        do
        {
            do
            {
                printf("%dth %s pawn.\n", (turn / 2) + 1, pawn == WHITE_PAWN ? "white" : "black");
                printf("Ring/field (insert): ");
                fflush(stdin);
            }
            while (scanf("%d %d", &ring, &field) != 2 || ring < 1 || ring > 3 || field < 1 || field > 8);

            mills = 0;
        }
        while (!insertPawn(pawn, ring - 1, field - 1, &mills));

        printBoard();

        while (mills > 0)
        {
            do
            {
                do
                {
                    printf("Ring/field (remove): ");
                    fflush(stdin);
                }
                while (scanf("%d %d", &ring, &field) != 2 || ring < 1 || ring > 3 || field < 1 || field > 8);
            }
            while (!removePawn(3 - pawn, ring - 1, field - 1));

            mills--;

            printBoard();
        }

        pawn = 3 - pawn; /* Switch */
    }

    printf("Moving phase.\n");

    while (!done)
    {
        int destRing, destField;
        do
        {
            do
            {
                printf("%s turn.\n", pawn == WHITE_PAWN ? "White" : "Black");
                printf("Source ring/field (move): ");
                fflush(stdin);
            }
            while (scanf("%d %d", &ring, &field) != 2 || ring < 1 || ring > 3 || field < 1 || field > 8);

            do
            {
                printf("Destination ring/field (move): ");
                fflush(stdin);
            }
            while (scanf("%d %d", &destRing, &destField) != 2 || destRing < 1 || destRing > 3 || destField < 1 || destField > 8);

            mills = 0;
        }
        while (!movePawn(pawn, ring - 1, field - 1, destRing - 1, destField - 1, &mills));

        printBoard();

        while (mills > 0)
        {
            do
            {
                do
                {
                    printf("Ring/field (remove): ");
                    fflush(stdin);
                }
                while (scanf("%d %d", &ring, &field) != 2 || ring < 1 || ring > 3 || field < 1 || field > 8);
            }
            while (!removePawn(3 - pawn, ring - 1, field - 1));

            mills--;

            printBoard();
        }

        pawn = 3 - pawn; /* Switch */

        if (whitePawns <= 2)
        {
            printf("Black wins!\n");
            done = TRUE;
        }
        else if (blackPawns <= 2)
        {
            printf("White wins!\n");
            done = TRUE;
        }
    }
    return(0);
}
