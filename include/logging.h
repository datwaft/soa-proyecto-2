#ifndef LOGGING_H
#define LOGGING_H

// Display an information message to stderr with its timestamp.
void log_info(const char *message, ...) __attribute__((format(printf, 1, 2)));

// Display an important message to stderr with its timestamp.
void log_important(const char *message, ...)
    __attribute__((format(printf, 1, 2)));

// Display a warning message to stderr with its timestamp.
void log_warn(const char *message, ...) __attribute__((format(printf, 1, 2)));

// Display an error message to stderr with its timestamp.
void log_error(const char *message, ...) __attribute__((format(printf, 1, 2)));

#endif // !LOGGING_H
