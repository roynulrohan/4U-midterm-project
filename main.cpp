/* Roynul Rohan - Mid-term Project
 *
 * This program reads and writes student data and saves them in a binary file.
 * The file path isn't hard-coded. User will be prompted to enter desired file path.
 * The main program allows user to create a new file or open existing ones/edit it.
 * I've implemented seek to edit precise data of each student.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// I got this function from some of your lessons and quizzes
// Prompt user and process input text
void prompt(const char *prompt, char *buffer, int buffer_size) {

	printf("%s: ", prompt);
	fflush(stdout);
	fgets(buffer, buffer_size, stdin);

	// Get rid of that annoying \n from fgets()
	buffer[strlen(buffer) - 1] = '\0';
}

#define NAME_MAX 15

struct Student {
	char first_name[NAME_MAX];
	char last_name[NAME_MAX];
	int age;
	int grade_level;
	int absences;
	float average;
};

// prints individual student
void print_student(const Student * const s) {
	printf("Name: %s %s\nAge: %d\nGrade Level: %d\nAbsences: %d\nAverage: %.1f\n", s->first_name, s->last_name, s->age, s->grade_level, s->absences, s->average);
}

// prompts user for information of a student
void get_student(Student *list) {
	prompt("\nEnter first name", list->first_name, NAME_MAX);
	prompt("Enter last name", list->last_name, NAME_MAX);

	printf("Enter age: ");
	fflush(stdout);
	scanf("%d", &list->age);

	printf("Enter grade: ");
	fflush(stdout);
	scanf("%d", &list->grade_level);

	printf("Enter absences: ");
	fflush(stdout);
	scanf("%d", &list->absences);

	printf("Enter average: ");
	fflush(stdout);
	scanf("%f", &list->average);

	fflush(stdin);

	list->first_name[NAME_MAX - 1] = '\0';
	list->last_name[NAME_MAX - 1] = '\0';
}

// for char that holds the location of file
#define PATH_MAX 50

/*
 * this function adds in extra slashes into the path
 * i got the part about inserting and shifting elements from here
 * https://stackoverflow.com/questions/11638123/how-to-add-an-element-to-array-and-shift-indexes
 *
 */

void tweak_filepath(char *path) {
	int i = 0;

	while (i < PATH_MAX) {
		if (path[i] == '\\') {
			int prev_value = '\\';

			// shifts all elements to the right starting at i
			for (int j = i; j < (int) strlen(path - 1); j++) {
				char temp = prev_value;
				prev_value = path[j];
				path[j] = temp;
			}
			i += 2;
		}

		i++;
	}
}

#define LIST_MAX 10 // max num of students

// this function deletes a students and shifts the array over to fill it in
void delete_student(Student * const list, int index) {
	if (index == 0)
		index++;

	for (int j = index - 1; j < LIST_MAX - 1; j++) {
		list[j] = list[j + 1];
	}
}

// functions to close and open file since i used them a lot. makes code look cleaner
void close_file(FILE *file) {
	if (fclose(file) != 0) {
		printf("error closing file\n");
		exit(1);
	}
}

FILE *open_file(FILE *file, const char *path, char mode) {
	if (mode == 'r') {
		if ((file = fopen(path, "r+b")) == NULL) {
			printf("\nerror opening file.\n");
			exit(1);
		}
	} else if (mode == 'w') {
		if ((file = fopen(path, "w+b")) == NULL) {
			printf("\nerror opening file.\n");
			exit(1);
		}
	} else if (mode == 'a') {
		if ((file = fopen(path, "a+b")) == NULL) {
			printf("\nerror opening file.\n");
			exit(1);
		}
	}

	return file;
}

// prototype for edit function which i put after the main function
void edit(Student *, char *, char *, int);

#define INPUT_MAX 15

int main() {
	FILE *filePtr;
	Student list[LIST_MAX], temp;
	int num_students = 0;
	char input[INPUT_MAX]; // user's input
	char path[PATH_MAX]; // location of file

	printf("Welcome. This program will allow you to create \nnew lists of students or view\\edit existing ones.\n");

	do {

		prompt("\nnew \\ open \\ exit\n\nenter", input, INPUT_MAX);

		if (strcmp(input, "new") == 0) {

			// prompts user for file location and then adds in the slashes
			prompt("\nEnter location of where you want to create the new file.\nEx. C:\\test.bin\n\nPath", path, PATH_MAX);
			tweak_filepath(path);

			// first checks if a file at that location already exists, if yes then prompts to replace it
			if ((filePtr = fopen(path, "r+b")) != NULL) {
				close_file(filePtr);

				prompt("\nFile already exists. Do you want to replace it? (Y/N) ", input, INPUT_MAX);
			}

			if (strcmp(input, "N") != 0) {

				filePtr = open_file(filePtr, path, 'w');

				// error proofing
				do {
					printf("\nHow many students would you like to add? (max %d) ", LIST_MAX);
					fflush(stdout);
					scanf("%d", &num_students);
					fflush(stdin);

					if (num_students > LIST_MAX)
						printf("\nInvalid number.\n");

				} while ((num_students) > LIST_MAX);

				// gets students and writes them to file
				for (int i = 0; i < num_students; i++) {
					printf("\nStudent # %d\n", i + 1);
					get_student(&list[i]);

					list[i].first_name[NAME_MAX - 1] = '\0';
					list[i].last_name[NAME_MAX - 1] = '\0';

					if ((fwrite(&list[i], sizeof(list[i]), 1, filePtr) != 1)) {
						printf("error writing to file\n");
						return 1;
					}
				}

				close_file(filePtr);

				printf("\nThe .bin file was successfully saved.\n");
			}

		} else if (strcmp(input, "open") == 0) {

			prompt("\nEnter location of the .bin file you want to open.\nEx. C:\\test.bin\n\nPath", path, PATH_MAX);
			tweak_filepath(path);

			if ((filePtr = fopen(path, "r+b")) == NULL) {
				printf("\nError. File path invalid or file doesn't exist.\n");
			}

			else {
				int count = 0;
				num_students = 0;

				// reads from file
				while (!feof(filePtr)) {
					if ((fread(&temp, sizeof(temp), 1, filePtr) != 1) && !feof(filePtr)) {
						printf("error reading file.\n");
						return 1;
					}

					count++;

					if (num_students < LIST_MAX && num_students != count) {
						list[num_students] = temp;

						list[num_students].first_name[NAME_MAX - 1] = '\0';
						list[num_students].last_name[NAME_MAX - 1] = '\0';

						num_students++;
					}
				}

				if (count - 1 > num_students) {
					printf("\n%d Students Detected. Only %d Stored.\n", count - 1, num_students);
				} else {
					num_students--;
				}

				close_file(filePtr);

				// displays the data read from file
				if (num_students - 1 == 0) {
					printf("\nFile empty.\n");

				} else {

					for (int i = 0; i < num_students; i++) {
						printf("\n%d)\n\n", i + 1);
						print_student(&list[i]);
					}
				}

				prompt("\nDo you want to edit this list(Y/N) ", input, INPUT_MAX);

				/*
				 *
				 * THIS IS WHERE THE FILE EDITS
				 *
				 */

				if (strcmp(input, "Y") == 0) {
					edit(&list[0], path, input, num_students);
				}
			}

		} else if (strcmp(input, "exit") != 0) {
			printf("\nInvalid Command.\n");
		}

	} while (strcmp(input, "exit") != 0);

	printf("\nProgram Closed.\n");

	return 0;
} // end main function

// edit mode function
void edit(Student *list, char *path, char *input, int num_students) {
	Student temp;
	FILE *filePtr;

	printf("\nEditing file in real time.\n");
	if ((filePtr = fopen(path, "r+b")) == NULL) {
		printf("\nerror opening file.\n");
		exit(1);
	}

	// loop for edit mode
	do {

		// seeks to start of file
		if (fseek(filePtr, 0, SEEK_SET) == -1) {
			printf("Error seeking\n");
			exit(1);
		}

		num_students = 0;
		// reads from file and save into arrays
		while (!feof(filePtr)) {
			if ((fread(&temp, sizeof(temp), 1, filePtr) != 1) && !feof(filePtr)) {
				printf("error reading file.\n");
				exit(1);
			}

			if (num_students < LIST_MAX) {
				list[num_students] = temp;

				list[num_students].first_name[NAME_MAX - 1] = '\0';
				list[num_students].last_name[NAME_MAX - 1] = '\0';

				num_students++;
			}
		}

		// displays students
		if (num_students >= 1) {
			for (int i = 0; i < num_students; i++) {
				printf("\n%d)\n\n", i + 1);
				print_student(&list[i]);
			}
		} else
			printf("\nList Currently Empty.\n");

		prompt("\nadd \\ delete \\ edit \\ save\n\nenter", input, INPUT_MAX);

		if (strcmp(input, "add") == 0) { // adding student

			filePtr = open_file(filePtr, path, 'a'); // opens files for appending since we're only adding a student

			// first checks if list is full or not
			if (num_students == LIST_MAX) {

				printf("\nList full. Delete a student first.\n");

			} else {

				printf("\nAdding new student.\n");
				get_student(&temp); // gets data in temporary student since everything is being read at the start of loop

				temp.first_name[NAME_MAX - 1] = '\0';
				temp.last_name[NAME_MAX - 1] = '\0';

				// writes data
				if ((fwrite(&temp, sizeof(temp), 1, filePtr) != 1)) {
					printf("error writing file\n");
					exit(1);
				}

				num_students++;
			}

			// closes file and re-opens for reading
			close_file(filePtr);
			filePtr = open_file(filePtr, path, 'r');

		} else if (strcmp(input, "delete") == 0) {

			filePtr = open_file(filePtr, path, 'w'); // opened for w since u cant directly erase something from a file.
											 // so i just erase it from the array and re-write the entire array.
			int index;

			// error proofed prompt
			do {
				printf("\nWhich one would you like to delete? (#) ");
				fflush(stdout);
				scanf("%d", &index);
				fflush(stdin);

				if (index > num_students) {
					printf("\nInvalid Entry.\n");
				}

			} while (index > num_students);

			num_students--;

			// deletes student
			printf("\nDeleting %s %s.\n", list[index - 1].first_name, list[index - 1].last_name);
			if (index < num_students) {
				delete_student(&list[index - 1], index - 1);
			}

			// re-writes data
			for (int i = 0; i < num_students; i++) {
				list[i].first_name[NAME_MAX - 1] = '\0';
				list[i].last_name[NAME_MAX - 1] = '\0';

				if ((fwrite(&list[i], sizeof(list[i]), 1, filePtr) != 1)) {
					printf("error writing file\n");
					exit(1);
				}
			}

			// closes file and re-opens for reading
			close_file(filePtr);
			filePtr = open_file(filePtr, path, 'r');

		} else if (strcmp(input, "edit") == 0) {

			int index, bytes = 0; // num of bytes

			// error proofed prompt
			do {
				printf("\nWhich one would you like to edit? (#) ");
				fflush(stdout);
				scanf("%d", &index);
				fflush(stdin);

				if (index > num_students - 1) {
					printf("\nInvalid Entry.\n");
				}

			} while (index > num_students - 1);

			// counts all the bytes leading up to but excluding the desired index
			if (index != 1) {
				for (int i = 0; i < index - 1; i++) {
					bytes += sizeof(list[i]);
				}
			}

			// prompts what they want to edit
			printf("\nEditing %s %s.\n", list[index - 1].first_name, list[index - 1].last_name);
			prompt("\nname \\ age \\ grade \\ absences \\ average\n\nenter what you want to edit", input, INPUT_MAX);

			// edits name
			if (strcmp(input, "name") == 0) {
				// since name is the first thing, i just have to offset the num of students(bytes) before index - 1
				if (fseek(filePtr, bytes, SEEK_SET) == -1) {
					printf("Error seeking\n");
					exit(1);
				}

				// prompts just for names
				prompt("\nEnter first name", list[index - 1].first_name, NAME_MAX);
				prompt("Enter last name", list[index - 1].last_name, NAME_MAX);

				list[index - 1].first_name[NAME_MAX - 1] = '\0';
				list[index - 1].last_name[NAME_MAX - 1] = '\0';

				// writes only the names
				if ((fwrite(list[index - 1].first_name, sizeof(char), NAME_MAX, filePtr) != NAME_MAX) || (fwrite(list[index - 1].last_name, sizeof(char), NAME_MAX, filePtr) != NAME_MAX)) {
					printf("error writing to file.\n");
					exit(1);
					;
				}

			}

			// edits age
			else if (strcmp(input, "age") == 0) {
				// this time i add on the bytes of the name * 2 since i have first and last names
				if (fseek(filePtr, bytes + (NAME_MAX + 1) * 2, SEEK_SET) == -1) {
					printf("Error seeking\n");
					exit(1);
				}

				printf("\nEnter age: ");
				fflush(stdout);
				scanf("%d", &list[index - 1].age);
				fflush(stdin);

				// writes only age
				if ((fwrite(&list[index - 1].age, sizeof(int), 1, filePtr) != 1)) {
					printf("error writing to file.\n");
					exit(1);
				}
			}

			// edits grade
			else if (strcmp(input, "grade") == 0) {
				// same as before but i add on 4 since ints for 4 bytes
				if (fseek(filePtr, bytes + ((NAME_MAX + 1) * 2) + 4, SEEK_SET) == -1) {
					printf("Error seeking\n");
					exit(1);
				}

				printf("\nEnter grade level: ");
				fflush(stdout);
				scanf("%d", &list[index - 1].grade_level);
				fflush(stdin);

				// writes only grade
				if ((fwrite(&list[index - 1].grade_level, sizeof(int), 1, filePtr) != 1)) {
					printf("error writing to file.\n");
					exit(1);
				}
			}

			// edits absences
			else if (strcmp(input, "absences") == 0) {
				// add on 4 more bytes
				if (fseek(filePtr, bytes + ((NAME_MAX + 1) * 2) + 8, SEEK_SET) == -1) {
					printf("Error seeking\n");
					exit(1);
				}

				printf("\nEnter absences: ");
				fflush(stdout);
				scanf("%d", &list[index - 1].absences);
				fflush(stdin);

				// writes only absences
				if ((fwrite(&list[index - 1].absences, sizeof(int), 1, filePtr) != 1)) {
					printf("error writing to file.\n");
					exit(1);
				}
			}

			// edits average
			else if (strcmp(input, "average") == 0) {
				// add on 4 more bytes
				if (fseek(filePtr, bytes + ((NAME_MAX + 1) * 2) + 12, SEEK_SET) == -1) {
					printf("Error seeking\n");
					exit(1);
				}

				printf("\nEnter average: ");
				fflush(stdout);
				scanf("%f", &list[index - 1].average);
				fflush(stdin);

				// writes only average
				if ((fwrite(&list[index - 1].average, sizeof(int), 1, filePtr) != 1)) {
					printf("error writing to file.\n");
					exit(1);
				}
			}
		}

		else if (strcmp(input, "save") == 0) {
			printf("\nSaving...\n");
			close_file(filePtr);
		}

		else {
			printf("\nInvalid Command.\n");
		}

	} while (strcmp(input, "save") != 0);

	printf("\nSuccessfully saved.\n");

}
// end of code
