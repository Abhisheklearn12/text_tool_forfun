
use std::ffi::{CStr, CString};
use std::os::raw::{c_char};
use libc::free;
use std::io::{self, Read};

unsafe extern "C" {
    fn register_rust_logger(cb: extern "C" fn(*const c_char));

    fn to_uppercase(input: *const c_char) -> *mut c_char;
    fn reverse_text(input: *const c_char) -> *mut c_char;
    fn capitalize_text(input: *const c_char) -> *mut c_char;

    fn text_stats(input: *const c_char,
                  words: *mut i32, chars: *mut i32, lines: *mut i32,
                  vowels: *mut i32, consonants: *mut i32);

    fn is_palindrome(input: *const c_char) -> i32;
    fn is_anagram(a: *const c_char, b: *const c_char) -> i32;
}

// -------- Logger: called from C++ --------
#[unsafe(no_mangle)]
extern "C" fn rust_log(msg: *const c_char) {
    unsafe {
        if let Ok(s) = CStr::from_ptr(msg).to_str() {
            println!("[from C++] {s}");
        }
    }
}

// -------- Helper to own returned C strings --------
fn from_cpp(ptr: *mut c_char) -> String {
    unsafe {
        if ptr.is_null() { return String::from("<null>"); }
        let s = CStr::from_ptr(ptr).to_string_lossy().into_owned();
        free(ptr as *mut libc::c_void);
        s
    }
}

fn main() {
    unsafe { register_rust_logger(rust_log); }

    let args: Vec<String> = std::env::args().collect();
    let input = if args.len() >= 2 {
        args[1..].join(" ")
    } else {
        let mut buf = String::new();
        io::stdin().read_to_string(&mut buf).expect("stdin read failed");
        buf
    };
    let c_input = CString::new(input.clone()).expect("CString failed");

    unsafe {
        let upper = from_cpp(to_uppercase(c_input.as_ptr()));
        let rev   = from_cpp(reverse_text(c_input.as_ptr()));
        let caps  = from_cpp(capitalize_text(c_input.as_ptr()));

        let (mut w, mut ch, mut l, mut v, mut cons) = (0,0,0,0,0);
        text_stats(c_input.as_ptr(), &mut w, &mut ch, &mut l, &mut v, &mut cons);

        let pal = is_palindrome(c_input.as_ptr());

        println!("\n===== Rust ↔ C++ Text Analyzer =====");
        println!("Input: {}", input.trim_end());
        println!("-----------------------------------");
        println!("Uppercase   : {}", upper);
        println!("Reversed    : {}", rev);
        println!("Capitalized : {}", caps);
        println!("Palindrome  : {}", if pal==1 { "Yes" } else { "No" });
        println!("-----------------------------------");
        println!("Words: {}, Chars: {}, Lines: {}", w, ch, l);
        println!("Vowels: {}, Consonants: {}", v, cons);

        // --- demonstrate anagram check ---
        let word1 = CString::new("listen").unwrap();
        let word2 = CString::new("silent").unwrap();
        let ana = is_anagram(word1.as_ptr(), word2.as_ptr());
        println!("Anagram test (listen/silent): {}", if ana==1 { "True" } else { "False" });
    }
}
