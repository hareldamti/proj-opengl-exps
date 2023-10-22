#include <stdio.h>
#include "io.h"
#include "../utils.h"

File io_file_read(const char *path) {
	File file = { .is_valid = false };
	FILE *fp = fopen(path, "r");

	if (!fp || ferror(fp)) {
		ERROR_RETURN(file, "Cannot read file %s\n", path);
	}

    fseek(fp, 0, SEEK_END);
	i32 length = ftell(fp);
    if (length == -1L) {
		fclose(fp);
		ERROR_RETURN(file, "Could not assertain length of file %s\n", path);
	}
    fseek(fp, 0, SEEK_SET);

    char *buffer = malloc((length + 1) * sizeof(char));
	if (!buffer) {
		fclose(fp);
		ERROR_RETURN(file, "Cannot allocate file buffer for %s\n", path);
	}

    fread(buffer, sizeof(char), length, fp);
	buffer[length] = 0;
	fclose(fp);

	printf("File loaded. %s\n", path);
	file.data = buffer;
	file.len = length;
	file.is_valid = true;
	return file;
}

int io_file_write(void *buffer, size_t size, const char *path) {
	FILE *fp = fopen(path, "w");
	if (!fp) {
		ERROR_RETURN(1, "Cannot write file %s\n", path);
	}

	size_t chunks_written = fwrite(buffer, size, 1, fp);

	fclose(fp);

	if (chunks_written != 1) {
		ERROR_RETURN(1, "Incorrect chunks written. Expected 1, got %zu.\n", chunks_written);
	}

	printf("File saved. %s\n", path);
	return 0;
}