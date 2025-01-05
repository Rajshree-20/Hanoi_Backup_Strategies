// This is the code for disks of varying size where each disk stores different number of songs in increasing order
//when we move from top to buttom
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_SONGS_PER_DISK 1000
#define MAX_SONG_LENGTH 1024  // Maximum song length
#define MAX_DISKS 1000
#define FILENAME "input2.txt"
#define BACKUP_FILENAME "backup_info.txt"
#define MAX_DAYS 365  // Maximum number of days in a year

void moveDisk(char source, char dest, int disk_number, int day, FILE *backup_info, int **disk_move_days) {
    printf("Day %d: Move disk%d from %c to %c\n", day, disk_number, source, dest);
    fprintf(backup_info, "Day %d: Move disk%d from %c to %c\n", day, disk_number, source, dest);
    disk_move_days[disk_number - 1][day - 1] = 1; // Record the move day for the disk
}

void towerOfHanoi(int num_disks, char source, char spare, char dest, char ***disks, int *day, int *moves, int *disk_moves, FILE *backup_info, int **disk_move_days) {
    if (num_disks == 0) {
        return;
    }
    towerOfHanoi(num_disks - 1, source, dest, spare, disks, day, moves, disk_moves, backup_info, disk_move_days);
    moveDisk(source, dest, num_disks, ++(*day), backup_info, disk_move_days);
    (*moves)++;
    disk_moves[num_disks - 1]++;
    towerOfHanoi(num_disks - 1, spare, source, dest, disks, day, moves, disk_moves, backup_info, disk_move_days);
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
    towerOfHanoi(num_disks, 'A', 'B', 'C', (char ***)disks, &day, &moves, disk_moves, backup_info, disk_move_days);

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

    // Print the number of moves for each disk
    for (int i = 0; i < num_disks; i++) {
        printf("Disk%d moves: %d\n", i + 1, disk_moves[i]);
        fprintf(backup_info, "Disk%d moves: %d\n", i + 1, disk_moves[i]);
    }

    // Print the days each disk was moved
    for (int i = 0; i < num_disks; i++) {
        printf("Disk%d was moved on days: ", i + 1);
        fprintf(backup_info, "Disk%d was moved on days: ", i + 1);
        for (int j = 0; j < MAX_DAYS; j++) {
            if (disk_move_days[i][j] == 1) {
                printf("%d ", j + 1); // Day numbering starts from 1
                fprintf(backup_info, "%d ", j + 1);
            }
        }
        printf("\n");
        fprintf(backup_info, "\n");
    }

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
