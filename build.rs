
fn main() {
    cc::Build::new()
        .cpp(true)
        .file("cpp/lib.cpp")
        .include("cpp")
        .flag_if_supported("-std=c++17")
        .compile("cpp_text_lib");

    println!("cargo:rerun-if-changed=cpp/lib.cpp");
    println!("cargo:rerun-if-changed=cpp/lib.h");
}
