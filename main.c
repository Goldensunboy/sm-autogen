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
	fpos_t begin_data, _pos;
	int begin_data_set = 0;
	char *buf = malloc(sizeof(char) << 10);
	while(!feof(f)) {

		// Read up to the next note data
		while(1) {
			fgetpos(f, &_pos);
			if(fgets(buf, 1024, f)) {
				if(!strncmp(buf, "#NOTES:", 7)) {
					if(!begin_data_set) {
						begin_data_set = 1;
						begin_data = _pos; // Location of first data
					}
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
			} else if(challenge_exists < 0) {
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

	// Write new difficulty metadata
	fwrite("\r\n//----------dance-single - autogen----------\r\n", 48, 1, f);
	fsetpos(f, &begin_data);
	while(--choice) {
		
	}
	for(int i = 0; i < 6; ++i) {
		fgets(buf, 1024, f);
		char *ptr = buf;
		switch(i) {
		case 2:
			ptr = "     autogen:\r\n";
			break;
		case 3:
			ptr = challenge_exists ? "     Edit:\r\n" : "     Challenge:\r\n";
			break;
		}
		fgetpos(f, &_pos);
		fwrite(ptr, strlen(ptr), 1, f);
		fsetpos(f, &_pos);
	}


	printf("You chose: %d\n", choice);
	printf("Challenge exists: %s\n", challenge_exists ? "yes" : "no");

	return 0;
}

