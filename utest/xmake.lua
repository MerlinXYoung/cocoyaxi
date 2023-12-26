target("libutest")
    set_kind("static")
    set_basename("utest")
    add_files("**.cc")
    add_deps("libco")

    add_cxflags("-Wno-builtin-macro-redefined")
    before_build_file(redefine_file_macro)