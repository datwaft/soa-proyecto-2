#ifndef LOGGING_H
#define LOGGING_H

// Display an information message to stderr with its timestamp.
void log_info(const char *message, ...);

// Display an important message to stderr with its timestamp.
void log_important(const char *message, ...);

// Display a warning message to stderr with its timestamp.
void log_warn(const char *message, ...);

// Display an error message to stderr with its timestamp.
void log_error(const char *message, ...);

#endif // !LOGGING_H
