#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")
int get_secure_random(int max) 
{
    unsigned int number;
    if (!BCRYPT_SUCCESS(BCryptGenRandom(NULL, (PUCHAR)&number, sizeof(number), BCRYPT_USE_SYSTEM_PREFERRED_RNG))) 
    {
        printf("BCryptGenRandom failed\n");
        exit(1);
    }
    return number % max;
}
#else
#include <fcntl.h>
#include <unistd.h>
int get_secure_random(int max) 
{
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd == -1) 
    {
        printf("Failed to open /dev/urandom\n");
        exit(1);
    }
    unsigned int number;
    if (read(fd, &number, sizeof(number)) != sizeof(number)) 
    {
        printf("Failed to read from /dev/urandom\n");
        exit(1);
    }
    close(fd);
    return number % max;
}
#endif

int monty_hall(int switch_choice) 
{
    int doors[3] = {0, 0, 1}; // 0 = Ziege, 1 = Auto
    int player_choice, opened_door, i;

    // Türen zufällig mischen
    for (i = 0; i < 3; i++) 
    {
        int j = get_secure_random(3);
        int temp = doors[i];
        doors[i] = doors[j];
        doors[j] = temp;
    }

    // Spieler wählt zufällig eine Tür
    player_choice = get_secure_random(3);

    // Moderator öffnet eine Tür mit einer Ziege
    for (i = 0; i < 3; i++) 
    {
        if (i != player_choice && doors[i] == 0) 
        {
            opened_door = i;
            break;
        }
    }

    // Spieler wechselt die Tür, wenn switch_choice wahr ist
    if (switch_choice) 
    {
        for (i = 0; i < 3; i++) 
        {
            if (i != player_choice && i != opened_door) 
            {
                player_choice = i;
                break;
            }
        }
    }

    // Spieler gewinnt, wenn er das Auto wählt
    return doors[player_choice] == 1;
}

double simulate(int n, int switch_choice) 
{
    int wins = 0;
    for (int i = 0; i < n; i++) 
    {
        wins += monty_hall(switch_choice);
    }
    return (double)wins / n;
}

int main() 
{
    // No need to initialize srand() as we are using true randomness
    int n = 10000;
    printf("Gewinnrate ohne Wechsel (%d Versuche): %.2f%%\n", n, simulate(n, 0) * 100);
    printf("Gewinnrate mit Wechsel (%d Versuche): %.2f%%\n", n, simulate(n, 1) * 100);
    return 0;
}