/* Andrew Wilder */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
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
	while(--choice) { // Progress to the chosen difficulty
		fgets(buf, 1024, f);
		do {
			fgetpos(f, &_pos);
			fgets(buf, 1024, f);
		} while(strncmp("#NOTES:", buf, 7));
		fsetpos(f, &_pos);
	}
	for(int i = 0; i < 6; ++i) {
		fgets(buf, 1024, f);
		char *ptr = buf;
		switch(i) {
		case 1:
			ptr = "     dance-single:\r\n";
			break;
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

	// Process the step data
	srand(time(NULL));
	int feet[2] = {0, 3}; // Feet start on left and right
	int next_step = rand() & 1; // Random first foot
	int counts[4] = {1, 1, 1, 1}; // Weights start at 1
	int *weight_arr = malloc(sizeof(int) *128);
	// continue to end of the chart
	do {
		fgets(buf, 1024, f);
		fgetpos(f, &_pos);
		// copy the line if it's not step data
		if(*buf >= '0' && *buf <= '3') {
			char *ptr = buf;
			int has_step = 0;
			do {
				if(*ptr == '1' || *ptr == '2') {
					has_step = 1;
				}
			} while(*++ptr != '\r');
			// if there is no step, write 0000
			if(has_step) {

				// Generate a step
				int step;
				if(!next_step) {
					// left
					if(!feet[1]) {
						// previous step if right crossed over
						step = *feet;
					} else {
						// weight the arrows and pick one
						int idx = 0;
						for(int i = 0; i < 4; ++i) {
							// don't jackhammer
							if(i != feet[1]) {
								for(int j = 0; j < counts[i]; ++j) {
									weight_arr[idx++] = i;
								}
							}
						}
						step = weight_arr[rand() % idx];
					}
					*feet = step;
				} else {
					// right
					if(*feet == 3) {
						// previous step if left crossed over
						step = feet[1];
					} else {
						// weight the arrows and pick one
						int idx = 0;
						for(int i = 0; i < 4; ++i) {
							// don't jackhammer
							if(i != *feet) {
								for(int j = 0; j < counts[i]; ++j) {
									weight_arr[idx++] = i;
								}
							}
						}
						step = weight_arr[rand() % idx];
					}
					feet[1] = step;
				}

				// Update the weights
				for(int i = 0; i < 4; ++i) {
					int inc = next_step ?
						(i == 3 ? 1 : 5) :
						(!i ? 1 : 5);
					counts[i] = i == step ? 1 : counts[i] + inc;
				}
				next_step ^= 1;
				
				// Write the step
				char *write_str = malloc(sizeof(char) * 7);
				strcpy(write_str, "0000\r\n");
				write_str[step] = '1';
				fwrite(write_str, 6, 1, f);

				printf("%s", write_str);

			} else {
				fwrite("0000\r\n", 6, 1, f);
			}
		} else {
			fwrite(buf, (int)(long)(strchr(buf, '\n') - (long)buf + 1), 1, f);
		}
		fsetpos(f, &_pos);
	} while(*buf != ';');

	return 0;
}

