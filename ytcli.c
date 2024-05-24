#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define URL_MAX_LENGTH 1024
#define QUERY_MAX_LENGTH 256
#define RESULTS_FILE "results.txt"

void search_video(char *query) {
    char command[QUERY_MAX_LENGTH + 50];
    snprintf(command, sizeof(command), "yt-dlp \"ytsearch10:%s\" --print \"%(title)s %(duration)s %(webpage_url)s\" -o \"\" > " RESULTS_FILE, query);
    system(command);
}

void list_results() {
    FILE *file = fopen(RESULTS_FILE, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening results file.\n");
        return;
    }

    char line[URL_MAX_LENGTH];
    int i = 1;
    while (fgets(line, sizeof(line), file)) {
        printf("%d. %s", i, line);
        i++;
    }

    fclose(file);
}

char* get_video_url(int choice) {
    FILE *file = fopen(RESULTS_FILE, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening results file.\n");
        return NULL;
    }

    char line[URL_MAX_LENGTH];
    int i = 1;
    while (fgets(line, sizeof(line), file)) {
        if (i == choice) {
            // Get the URL part of the line
            char *url = strrchr(line, ' ') + 1;
            url[strcspn(url, "\n")] = 0; // Remove newline character
            fclose(file);
            return strdup(url); // Return a duplicate of the URL
        }
        i++;
    }

    fclose(file);
    return NULL;
}

int main() {
    char query[QUERY_MAX_LENGTH];

    // Prompt user for search query
    printf("Search Video: ");
    if (fgets(query, QUERY_MAX_LENGTH, stdin) == NULL) {
        fprintf(stderr, "Error reading input.\n");
        return 1;
    }

    // Remove newline character from fgets
    query[strcspn(query, "\n")] = 0;

    // Search for videos
    search_video(query);

    // List results
    printf("Results:\n");
    list_results();

    // Prompt user to choose a video
    int choice;
    printf("Enter the number of the video to play: ");
    if (scanf("%d", &choice) != 1) {
        fprintf(stderr, "Invalid input.\n");
        return 1;
    }

    // Get the chosen video URL
    char *video_url = get_video_url(choice);
    if (video_url == NULL) {
        fprintf(stderr, "Invalid choice.\n");
        return 1;
    }

    // Command string to open URL in mpv
    char command[URL_MAX_LENGTH + 50];
    snprintf(command, sizeof(command), "mpv \"%s\"", video_url);
    free(video_url);

    // Execute command
    int result = system(command);
    if (result == -1) {
        fprintf(stderr, "Error executing command.\n");
        return 1;
    }

    return 0;
}
