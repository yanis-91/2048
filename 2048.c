// ldd: -lncurses
#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

// Variables globales
int board[4][4];
int score = 0;

typedef enum
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    FIN_PARTIE
} key;

void INIT_2048()
{
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    srand(time(NULL));
}

void DONE_2048()
{
    endwin();
    exit(0);
}

// Créer tableau vide
void init_board()
{
    for (int lig = 0; lig < 4; lig++)
    {
        for (int col = 0; col < 4; col++)
        {
            board[lig][col] = 0;
        }
    }
}

// Afficher le tableau
void display_board()
{
    move(0, 0);
    printw("Jeu:2048 ============================\n");
    printw("========================= Score:%5d\n", score);

    for (int lig = 0; lig < 4; lig++)
    {
        printw("+--------+--------+--------+--------+\n");
        printw("|        |        |        |        |\n");
        printw("|");
        for (int col = 0; col < 4; col++)
        {
            if (board[lig][col] != 0)
                printw("  %4i  |", board[lig][col]);
            else
                printw("        |");
        }
        printw("\n");
        printw("|        |        |        |        |\n");
    }
    printw("+--------+--------+--------+--------+\n");
    refresh();
}

// Compter case vide
int count_empty()
{
    int vide = 0;

    for (int lig = 0; lig < 4; lig++)
    {
        for (int col = 0; col < 4; col++)
        {
            if (board[lig][col] == 0)
            {
                vide++;
            }
        }
    }
    return vide;
}

// Ajouter un 2 dans une case au hasard
void add_two(int empty)
{
    int n = rand() % empty;
    int vide;

    for (int lig = 0; lig < 4; lig++)
    {
        for (int col = 0; col < 4; col++)
        {
            if (board[lig][col] == 0)
            {
                if (vide == n)
                {
                    board[lig][col] = 2;
                }
                vide++;
            }
        }
    }
}

int game_over(int add)
{
    int case_libre = count_empty();

    if (case_libre == 0) // Afficher tableau de fin de partie quand il n'y a plus de case vide
    {
        printw("============= GAME OVER =============\n");
        printw("=========== (press a key) ===========\n");
        getch();
        return 1;
    }
    else if (case_libre > 0 && add) // Ajouter un 2 s'il y a une case vide dans le tableau
    {
        add_two(case_libre);
        display_board();
        return 0;
    }
    else
        return 0;
}

// Décale le tableau
bool shift_board()
{
    int tof = 0;
    for (int lig = 0; lig < 4; lig++)
    {
        int ligne_libre = 0;
        for (int col = 0; col < 4; col++)
        {
            if (board[lig][col] != 0)
            {
                if (col != ligne_libre)
                {
                    board[lig][ligne_libre] = board[lig][col];
                    board[lig][col] = 0;
                    tof = 1;
                }
                ligne_libre++;
            }
        }
    }
    if (tof == 1)
        return true;
    return false;
}

// Additionne les cases de mêmes valeurs
bool update_board()
{
    int tof = 0;
    shift_board();
    for (int lig = 0; lig < 4; lig++)
    {
        for (int col = 0; col < 4; col++)
        {
            if (board[lig][col] != 0 && board[lig][col] == board[lig][col + 1])
            {
                board[lig][col] = board[lig][col] * 2;
                score = score + board[lig][col];
                board[lig][col + 1] = 0;
                tof = 1;
            }
        }
    }
    shift_board();
    if (tof == 1)
        return true;
    return false;
}

key get_key()
{
    int ch;
    while (1)
    {
        ch = getch();
        switch (ch)
        {
        case KEY_UP: // flèche vers le haut
            return UP;
        case KEY_DOWN: // flèche vers le bas
            return DOWN;
        case KEY_LEFT: // flèche vers la gauche
            return LEFT;
        case KEY_RIGHT: // flèche vers la droite
            return RIGHT;
        case KEY_BACKSPACE: // backspace
            return FIN_PARTIE;
        default:
            continue;
        }
    }
}

void swap(int *a, int *b)
{
    int c = *a;
    *a = *b;
    *b = c;
}

void mirror_board()
{
    for (int lig = 0; lig < 4; lig++)
    {
        for (int col = 0; col < 2; col++)
        {
            int val = board[lig][col];
            board[lig][col] = board[lig][3 - col];
            board[lig][3 - col] = val;
        }
    }
}

void pivot_board()
{
    for (int lig = 0; lig < 4; lig++)
    {
        for (int col = lig + 1; col < 4; col++)
        {
            int val = board[lig][col];
            board[lig][col] = board[col][lig];
            board[col][lig] = val;
        }
    }
}

bool play(key dir)
{
    int tof = 0;
    switch (dir)
    {
    case UP:
        mirror_board();
        pivot_board();
        update_board(); // compresse tableau vers la gauche
        pivot_board();  // combinaison inverse
        mirror_board();
        tof = 1;
        break;
    case DOWN:
        pivot_board();
        mirror_board();
        update_board(); // compresse tableau vers la gauche
        mirror_board(); // combinaison inverse
        pivot_board();
        tof = 1;
        break;
    case LEFT:
        update_board(); // compresse tableau vers la gauche
        tof = 1;
        break;
    case RIGHT:
        mirror_board();
        update_board(); // compresse tableau vers la gauche
        mirror_board(); // combinaison inverse
        tof = 1;
        break;
    default:
        break;
    }
    if (tof == 1)
        return true;
    return false;
}

// Appeler les fonctions
int main()
{
    INIT_2048();
    init_board();
    int add = 1;
    display_board();

    while (!game_over(add))
    {
        if (play(get_key()))
        {
            display_board();
        }
    }
    DONE_2048();
    return 0;
}