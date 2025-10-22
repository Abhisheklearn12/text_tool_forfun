
#ifndef CPP_LIB_H
#define CPP_LIB_H

#ifdef __cplusplus
extern "C" {
#endif

// -------- Rust callback registration --------

// Called by Rust to provide a logging callback.
// C++ will use it to log internal messages.
typedef void (*rust_log_fn)(const char* message);
void register_rust_logger(rust_log_fn cb);

// -------- Text processing --------
char* to_uppercase(const char* input);
char* reverse_text(const char* input);
char* capitalize_text(const char* input);

// -------- Text analysis --------
void text_stats(const char* input,
                int* words, int* chars, int* lines,
                int* vowels, int* consonants);

int is_palindrome(const char* input);
int is_anagram(const char* a, const char* b);

// -------- Memory ownership --------
// Rust must free() the malloc'd strings.
#ifdef __cplusplus
}
#endif

#endif
