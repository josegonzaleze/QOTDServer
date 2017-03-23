// timestamp()

#include <time.h>
#include <string.h>
const int TIMEBUF_SIZE = 26;

void chomp (char* s)
{	// If the last char in a C-style string is a '\n', this function removes it.
	int i=0;
	while (s[i] != '\0') {i++;}				// Find end of C-style string
	if (s[i-1] == '\n') {s[i-1] = '\0';}	// If last char is '\n', toss it.
}

char* timestamp()
{	// This function returns a string equal to the current date and time.
	//   (Requires: #include <time.h>)
	static char time_s[TIMEBUF_SIZE];
	time_t ltime;							// ltime will hold local time in time_t struct

	time(&ltime);							// Get number of seconds since beginning of epoch
	ctime_s(time_s, TIMEBUF_SIZE, &ltime);	// Convert time_t struct to char string
	chomp(time_s);							// Remove trailing '\n' char
	return time_s;
}