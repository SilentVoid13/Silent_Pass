#include "log.h"

// TODO: Maybe change this ?
#ifdef _WIN32
#include "log_win.h"
#endif // _WIN32

int verbose;

void log_info(char *msg, ...) {
	va_list args;
	va_start(args, msg);
	printf("[" BOLD_BLUE "*" RESET "] ");	
	log_format(msg, args);
	printf("\n");  
	va_end(args);
}

void log_verbose(char *msg, ...) {
    if(verbose == 1) {
        va_list args;
        va_start(args, msg);
        printf("[" BOLD_YELLOW "*" RESET "] ");
        log_format(msg, args);
        printf("\n");
        va_end(args);
    }
}

void log_success(char *msg, ...) {
	va_list args;
	va_start(args, msg);
	printf("[" BOLD_GREEN "+" RESET "] ");	
	log_format(msg, args);
	printf("\n");  
	va_end(args);
}

void log_error(char *msg, ...) {
	va_list args;
	va_start(args, msg);
	fprintf(stderr, "[" BOLD_RED "-" RESET "] ");	
	log_format_error(msg, args);
	fprintf(stderr, "\n");  
	va_end(args);
}

void log_progress(char *msg, ...) {
	va_list args;
	va_start(args, msg);
	printf("\r[" BOLD_BLUE "*" RESET "] ");	
	log_format(msg, args);
	va_end(args);
}

void log_format(const char* message, va_list args) {   
	vprintf(message, args);     
}

void log_format_error(const char *message, va_list args) {
	vfprintf(stderr, message, args);
}
