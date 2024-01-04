target("unitest")
    set_kind("binary")
    add_includedirs("../3rd")
    set_default(false)
    add_deps("libco")
    add_files("*.cc")

    add_includedirs("../utest")
    add_deps("libutest")

    -- add_cxflags("-Wno-builtin-macro-redefined")
    -- before_build_file(redefine_file_macro)

