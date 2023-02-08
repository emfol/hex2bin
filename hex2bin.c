#include <string.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_SIZE (128UL)
#define STATUS_OK (0)
#define STATUS_INVALID (1)
#define STATUS_UNEXPECTED (2)
#define STATUS_INPUT_ERROR (3)
#define STATUS_OUTPUT_ERROR (4)

static void
output_error(const char *msg)
{
	write(STDERR_FILENO, msg, strlen(msg));
}

int
main(int argc, char **argv)
{
	int digit, digit_count = 0, result = STATUS_OK;
	char byte, buf[BUFFER_SIZE];
	long int i, count;

	while ((count = read(STDIN_FILENO, buf, BUFFER_SIZE)) != 0L) {
		if (count < 0L) {
			if (errno == EINTR || errno == EAGAIN)
				continue;
			result = STATUS_INPUT_ERROR;
			goto shutdown;
		}
		for (i = 0L; i < count; ++i) {
			digit = buf[i];
			if (
				digit == '\t' ||
				digit == '\n' ||
				digit == '\r' ||
				digit == ' '
			) {
				if (digit_count == 0)
					continue;
				result = STATUS_UNEXPECTED;
				goto shutdown;
			}
			if (
				(digit >= '0' && digit <= '9') ||
				(digit >= 'A' && digit <= 'F') ||
				(digit >= 'a' && digit <= 'f')
			) {
				byte = byte << 4 | (
					(digit >= '0' && digit <= '9')
						? digit - '0'
						: digit + 10 - ((digit >= 'A' && digit <= 'F') ? 'A' : 'a')
				);
				if (++digit_count > 1) {
					digit_count = 0;
					while (write(STDOUT_FILENO, &byte, 1UL) != 1L) {
						if (errno == EINTR || errno == EAGAIN)
							continue;
						result = STATUS_OUTPUT_ERROR;
						goto shutdown;
					}
				}
				continue;
			}
			result = STATUS_INVALID;
			goto shutdown;
		}
	}

	shutdown:
	switch (result) {
		case STATUS_INVALID:
			output_error("Invalid digit...\n");
			break;
		case STATUS_UNEXPECTED:
			output_error("Unexpected character after first digit of a byte...\n");
			break;
		case STATUS_INPUT_ERROR:
			output_error("Error reading from standard input...\n");
			break;
		case STATUS_OUTPUT_ERROR:
			output_error("Error writing to standard output...\n");
			break;
	}

	return result;
}
