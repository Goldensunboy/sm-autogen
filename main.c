/* Andrew Wilder */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char **argv) {

	// Validate input
	if(argc != 2) {
		fprintf(stderr, "Usage: ./sm-autogen <simfile\'s .sm path>\n");
		exit(1);
	}
	FILE *f = fopen(argv[1], "a+");
	if(!f) {
		fprintf(stderr, "Error: %s\n", strerror(errno));
	}

	// Retrieve available difficulties
	int count = 0;
	int challenge_exists = 0;
	char *buf = malloc(sizeof(char) << 10);
	while(!feof(f)) {

		// Read up to the next note data
		while(1) {
			if(fgets(buf, 1024, f)) {
				if(!strncmp(buf, "#NOTES:", 7)) {
					break; // found note data
				}
			} else {
				break; // EOF
			}
		}

		// Print info about this difficulty
		if(!feof(f)) {
			fgets(buf, 1024, f);
			char *ptr = buf;
			while(*ptr++ == ' ');
			strchr(--ptr, '\n')[-2] = '\0';
			printf("%s - ", ptr);

			if(challenge_exists < 1 && !strcmp(ptr, "dance-single")) {
				challenge_exists = -1;
			}

			fgets(buf, 1024, f);
			fgets(buf, 1024, f);
			ptr = buf;
			while(*ptr++ == ' ');
			strchr(--ptr, '\n')[-2] = '\0';
			printf("%s (", ptr);

			if(challenge_exists < 0 && !strcmp(ptr, "Challenge")) {
				challenge_exists = 1;
			} else {
				challenge_exists = 0;
			}

			fgets(buf, 1024, f);
			ptr = buf;
			while(*ptr++ == ' ');
			strchr(--ptr, '\n')[-2] = '\0';
			printf("%s):\t[%d]\n", ptr, ++count);
		}
	}

	// Chose a source chart
	int choice;
	do {
		printf("Pick one (1-%d): ", count);
		fgets(buf, 1024, stdin);
		choice = atoi(buf);
		if(choice < 1 || choice > count) {
			*strchr(buf, '\n') = '\0';
			printf("Invalid choice: %s\n", buf);
			choice = 0;
		}
	} while(!choice);

	printf("You chose: %d\n", choice);
	printf("Challenge exists: %s\n", challenge_exists ? "yes" : "no");

	return 0;
}

