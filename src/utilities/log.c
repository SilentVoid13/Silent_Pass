#include "log.h"
#include "main.h"

void log_info(char *msg, ...) {
	va_list args;
	va_start(args, msg);
	printf("[" BOLD_BLUE "*" RESET "] ");	
	log_format(msg, args);
	va_end(args);
}

void log_success(char *msg, ...) {
	va_list args;
	va_start(args, msg);
	printf("[" BOLD_GREEN "+" RESET "] ");	
	log_format(msg, args);
	va_end(args);
}

void log_error(char *msg, ...) {
	va_list args;
	va_start(args, msg);
	printf("[" BOLD_RED "-" RESET "] ");	
	log_format(msg, args);
	va_end(args);
}

void log_format(const char* message, va_list args) {   
	vprintf(message, args);     
	printf("\n");  
}
