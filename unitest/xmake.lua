target("unitest")
    set_kind("binary")
    set_default(false)
    add_deps("libco")
    add_files("*.cc")

    add_cxflags("-Wno-builtin-macro-redefined")
    before_build_file(redefine_file_macro)

