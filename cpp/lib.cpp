
// Header files
#include "lib.h"
#include <string>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <set>
#include <sstream>

static rust_log_fn rust_logger = nullptr;
static void log_to_rust(const std::string& msg) {
    if (rust_logger) rust_logger(msg.c_str());
}

extern "C" void register_rust_logger(rust_log_fn cb) {
    rust_logger = cb;
    log_to_rust("[C++] Logger registered successfully");
}

static char* dup_str(const std::string& s) {
    char* out = (char*)std::malloc(s.size() + 1);
    if (!out) return nullptr;
    std::memcpy(out, s.data(), s.size());
    out[s.size()] = '\0';
    return out;
}

extern "C" char* to_uppercase(const char* input) {
    if (!input) return dup_str("");
    std::string s(input);
    for (char& c : s)
        c = std::toupper(static_cast<unsigned char>(c));
    log_to_rust("[C++] to_uppercase done");
    return dup_str(s);
}

extern "C" char* reverse_text(const char* input) {
    if (!input) return dup_str("");
    std::string s(input);
    std::reverse(s.begin(), s.end());
    log_to_rust("[C++] reverse_text done");
    return dup_str(s);
}

extern "C" char* capitalize_text(const char* input) {
    if (!input) return dup_str("");
    std::string s(input);
    bool new_word = true;
    for (char& c : s) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            new_word = true;
        } else {
            if (new_word) {
                c = std::toupper(static_cast<unsigned char>(c));
                new_word = false;
            } else {
                c = std::tolower(static_cast<unsigned char>(c));
            }
        }
    }
    log_to_rust("[C++] capitalize_text done");
    return dup_str(s);
}

extern "C" void text_stats(const char* input,
                           int* words, int* chars, int* lines,
                           int* vowels, int* consonants) {
    if (!input) { *words = *chars = *lines = *vowels = *consonants = 0; return; }

    int w = 0, c = 0, l = 0, v = 0, cons = 0;
    bool in_word = false;
    for (const char* p = input; *p; ++p) {
        char ch = *p;
        c++;
        if (std::isspace(static_cast<unsigned char>(ch))) {
            if (ch == '\n') l++;
            in_word = false;
        } else {
            if (!in_word) { w++; in_word = true; }
            char lower = std::tolower(static_cast<unsigned char>(ch));
            if (std::isalpha(lower)) {
                if (std::string("aeiou").find(lower) != std::string::npos)
                    v++;
                else
                    cons++;
            }
        }
    }
    *words = w; *chars = c; *lines = l + 1; *vowels = v; *consonants = cons;
    log_to_rust("[C++] text_stats done");
}

extern "C" int is_palindrome(const char* input) {
    if (!input) return 0;
    std::string s;
    for (const char* p = input; *p; ++p)
        if (std::isalnum(static_cast<unsigned char>(*p)))
            s.push_back(std::tolower(static_cast<unsigned char>(*p)));

    std::string r = s;
    std::reverse(r.begin(), r.end());
    int result = (s == r);
    log_to_rust(result ? "[C++] palindrome ✅" : "[C++] palindrome ❌"); // Used emoji's for CLI touch.
    return result;
}

extern "C" int is_anagram(const char* a, const char* b) {
    if (!a || !b) return 0;
    std::string s1(a), s2(b);
    auto norm = [](std::string& s) {
        s.erase(std::remove_if(s.begin(), s.end(),
                               [](char c){ return !std::isalnum(static_cast<unsigned char>(c)); }),
                s.end());
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c){ return std::tolower(c); });
        std::sort(s.begin(), s.end());
    };
    norm(s1); norm(s2);
    int result = (s1 == s2);
    log_to_rust(result ? "[C++] anagram ✅ " : "[C++] anagram ❌");  // Used emoji's for CLI touch.
    return result;
}
