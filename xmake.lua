add_requires("glfw", "nuklear", "glew")

target("ShittyPhoneBook")
    set_kind("binary")
    add_files("src/*.cpp")
    add_packages("glfw", "nuklear", "glew")