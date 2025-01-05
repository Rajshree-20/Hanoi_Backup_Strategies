// This is the code for 4 peg tower of hanoi
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAX_SONGS_PER_DISK 1000
#define MAX_SONG_LENGTH 1024  // Maximum song length
#define MAX_DISKS 1000
#define FILENAME "input2.txt"
#define BACKUP_FILENAME "backup_info_4peg.txt"
#define MAX_DAYS 365  // Maximum number of days in a year

void moveDisk(char source, char dest, int disk_number, int day, FILE *backup_info, int **disk_move_days) {
    printf("Day %d: Move disk%d from %c to %c\n", day, disk_number, source, dest);
    fprintf(backup_info, "Day %d: Move disk%d from %c to %c\n", day, disk_number, source, dest);
    disk_move_days[disk_number - 1][day - 1] = 1; // Record the move day for the disk
}

void ToH41(int i, int j, char a, char b, char c, char d, int *day, int *moves, int *disk_moves, FILE *backup_info, int **disk_move_days) {
    int m = j - i + 1;
    int k = (int)floor(m / 2);
    ToH4(m - k, a, d, b, c, day, moves, disk_moves, backup_info, disk_move_days);
    ToH3(k, a, b, c, day, moves, disk_moves, backup_info, disk_move_days);
    ToH4(m - k, d, b, a, c, day, moves, disk_moves, backup_info, disk_move_days);
    printf("\n");
}

void ToH3(int m, char p, char q, char r, int *day, int *moves, int *disk_moves, FILE *backup_info, int **disk_move_days) {
    if (m == 1) {
        moveDisk(p, q, 1, ++(*day), backup_info, disk_move_days);
        (*moves)++;
        disk_moves[0]++;
        return;
    }
    ToH3(m - 1, p, r, q, day, moves, disk_moves, backup_info, disk_move_days);
    moveDisk(p, q, m, ++(*day), backup_info, disk_move_days);
    (*moves)++;
    disk_moves[m - 1]++;
    ToH3(m - 1, r, q, p, day, moves, disk_moves, backup_info, disk_move_days);
}

void ToH4(int n, char p, char q, char r, char s, int *day, int *moves, int *disk_moves, FILE *backup_info, int **disk_move_days) {
    if (n == 0)
        return;
    if (n == 1) {
        moveDisk(p, q, 1, ++(*day), backup_info, disk_move_days);
        (*moves)++;
        disk_moves[0]++;
        return;
    }

    ToH4(n - 2, p, r, s, q, day, moves, disk_moves, backup_info, disk_move_days);

    moveDisk(p, s, n - 1, ++(*day), backup_info, disk_move_days);
    (*moves)++;
    disk_moves[n - 2]++;
    moveDisk(p, q, n, ++(*day), backup_info, disk_move_days);
    (*moves)++;
    disk_moves[n - 1]++;
    moveDisk(s, q, n - 1, ++(*day), backup_info, disk_move_days);
    (*moves)++;
    disk_moves[n - 2]++;

    ToH4(n - 2, r, q, p, s, day, moves, disk_moves, backup_info, disk_move_days);
}

int total_moves_4pegs(int n) {
    if (n == 0) {
        return 0;
    }
    return 2 * total_moves_4pegs(n - 1) + 1;
}

int main() {
    FILE *input, *backup_info;
    char ***disks;
    int num_disks = 0;
    int num_songs = 0;
    char song[MAX_SONG_LENGTH]; // Buffer to store each song read from the file

    // Open the input file
    input = fopen(FILENAME, "r");
    backup_info = fopen(BACKUP_FILENAME, "w");
    if (input == NULL || backup_info == NULL) {
        printf("Error opening files.\n");
        return 1;
    }

    // Allocate memory for the disks
    disks = (char ***)malloc(MAX_DISKS * sizeof(char **));
    if (disks == NULL) {
        printf("Memory allocation failed!\n");
        fclose(input);
        fclose(backup_info);
        return 1;
    }

    // Read songs from the file
    while (fscanf(input, "%[^\n]%*c", song) == 1 && num_disks < MAX_DISKS) {
        // Allocate memory for current disk
        disks[num_disks] = (char **)malloc(MAX_SONGS_PER_DISK * sizeof(char *));
        if (disks[num_disks] == NULL) {
            printf("Memory allocation failed!\n");
            fclose(input);
            fclose(backup_info);
            return 1;
        }

        // Allocate memory for each song in the current disk
        for (int i = 0; i < (num_disks + 1) * 4 && num_songs < MAX_DISKS * MAX_SONGS_PER_DISK; i++) {
            disks[num_disks][i] = (char *)malloc(MAX_SONG_LENGTH * sizeof(char));
            if (disks[num_disks][i] == NULL) {
                printf("Memory allocation failed!\n");
                fclose(input);
                fclose(backup_info);
                return 1;
            }
            strcpy(disks[num_disks][i], song);
            num_songs++;
            // Read next song
            if (fscanf(input, "%[^\n]%*c", song) != 1) {
                break;
            }
        }

        num_disks++;
    }

    // Close the input file
    fclose(input);

    // Allocate memory to record the days each disk was moved
    int **disk_move_days = (int **)malloc(num_disks * sizeof(int *));
    for (int i = 0; i < num_disks; i++) {
        disk_move_days[i] = (int *)calloc(MAX_DAYS, sizeof(int));
    }

    // Start the clock
    clock_t start = clock();

    // Print the disks and songs
    for (int i = 0; i < num_disks; i++) {
        fprintf(backup_info, "Disk %d:\n", i + 1);
        for (int j = 0; j < (i + 1) * 4 && j < MAX_SONGS_PER_DISK && disks[i][j] != NULL; j++) {
            fprintf(backup_info, "%s\n", disks[i][j]);
        }
        fprintf(backup_info, "\n");
    }

    // Backup Rotation Schedule
    printf("Backup Rotation Schedule:\n");
    int day = 0;
    int moves = 0;
    int disk_moves[MAX_DISKS] = {0}; // Initialize array to count moves for each disk
    ToH41(1, num_disks, 'A', 'B', 'C', 'D', &day, &moves, disk_moves, backup_info, disk_move_days);

    // End the clock
    clock_t end = clock();

    // Calculate CPU time used
    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    // Print the time taken
    printf("Time taken: %f seconds\n", cpu_time_used);
    fprintf(backup_info, "Time taken: %f seconds\n", cpu_time_used);

    printf("\nMoves:\n");
    printf("Number of Moves: %d\n", moves);
    fprintf(backup_info, "Number of Moves: %d\n", moves);


    // Calculate space used
    size_t total_size = 0;

    // Size of disk_moves array
    size_t size_disk_moves = sizeof(int) * num_disks;
    total_size += size_disk_moves;

    // Calculate the space used by the disk pointers and song storage
    for (int i = 0; i < num_disks; i++) {
        // Add the space for the disk pointers array
        total_size += sizeof(char *) * MAX_SONGS_PER_DISK;

        // Add the space for each song string in the disk
        for (int j = 0; j < MAX_SONGS_PER_DISK && disks[i][j] != NULL; j++) {
            total_size += strlen(disks[i][j]) + 1; // +1 for the null terminator
        }
    }

    // Print the space used
    printf("Space taken: %zu bytes\n", total_size);
    fprintf(backup_info, "Space taken: %zu bytes\n", total_size);

    // Close the backup info file
    fclose(backup_info);

    // Free allocated memory
    for (int i = 0; i < num_disks; i++) {
        for (int j = 0; j < MAX_SONGS_PER_DISK && disks[i][j] != NULL; j++) {
            free(disks[i][j]);
        }
        free(disks[i]);
    }
    free(disks);

    // Free allocated memory for disk_move_days
    for (int i = 0; i < num_disks; i++) {
        free(disk_move_days[i]);
    }
    free(disk_move_days);

    return 0;
}