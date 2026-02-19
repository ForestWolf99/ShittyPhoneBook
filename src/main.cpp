#undef GLFW_INCLUDE_NONE
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <algorithm>
#include <cctype>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

int WINDOW_WIDTH = 400;
int WINDOW_HEIGHT = 320;

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#include "core/phonebook.h"

static void error_callback(int e, const char *d) {
    printf("Error %d: %s\n", e, d);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;
}

std::string toLowerStr(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), 
        [](unsigned char c){ return std::tolower(c); } // Use a lambda for locale-independent tolower with unsigned char
    );
    return str;
}

bool containsCaseInsensitive(const std::string& mainStr, const std::string& subStr) {
    std::string lowerMain = toLowerStr(mainStr);
    std::string lowerSub = toLowerStr(subStr);

    // Use standard find on the lowercase strings
    return lowerMain.find(lowerSub) != std::string::npos;
}

int main() {
    struct nk_glfw glfw = {0};
    static GLFWwindow* window;
    int width = 0, height = 0;
    struct nk_context *ctx;
    struct nk_colorf bg;
    struct nk_list_view view;
    static char buffer[64];
    static int len = 0;
    static bool adding_contact = false;
    static char name_buffer[64];
    static int len2 = 0;
    static char number_buffer[10];
    static int len3 = 0;

    buffer[0] = '\0';

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return -1;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Shitty Phone Book", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    /* OpenGL */
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glewExperimental = 1;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to setup GLEW\n");
        exit(1);
    }

    ctx = nk_glfw3_init(&glfw, window, NK_GLFW3_INSTALL_CALLBACKS);

    {struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&glfw, &atlas);

    nk_glfw3_font_stash_end(&glfw);}

    phonebook book;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        nk_glfw3_new_frame(&glfw);
        
        /* GUI */

        if (nk_begin(ctx, "Main", nk_rect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT),
            NULL
            ))
        {
            nk_layout_row_static(ctx, 30, 170, 1);

            nk_label(ctx, "Contacts:", NK_TEXT_ALIGN_LEFT);

            nk_edit_string(ctx, NK_EDIT_FIELD, buffer, &len, 64, nk_filter_default);

            nk_layout_row_static(ctx, 200, 170, 1);
            
            if (nk_list_view_begin(ctx, &view, "ContactList", NK_WINDOW_BORDER, 20, sizeof(book.contacts) / sizeof(book.contacts[0]))) {
                for (int i = view.begin; i < view.end; ++i) {
                    nk_layout_row_dynamic(ctx, 20, 1);
                    if (len == 0 || containsCaseInsensitive(book.contacts[i].contact_name, string(buffer))) {
                        if (book.contacts[i].contact_number != 0) {
                            string contact_name = string();
                            contact_name += book.contacts[i].contact_name;
                            contact_name += ": ";
                            contact_name += to_string(book.contacts[i].contact_number);
                            if (nk_button_label(ctx, contact_name.c_str())) {
                            
                            }
                        } else {
                            if (nk_button_label(ctx, book.contacts[i].contact_name.c_str())) {
                                
                            }
                        }
                    }
                }
            nk_list_view_end(&view);

            nk_layout_row_static(ctx, 30, 30, 1);

            if (nk_button_label(ctx, "+")) {
                    adding_contact = true;
                }
            }
        }
        nk_end(ctx);

        if (adding_contact) {
            if (nk_begin(ctx, "Add Contact", nk_rect(50, 50, 300, 200),
                 NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_TITLE))
                {
                nk_layout_row_static(ctx, 30, 120, 2);
                nk_label(ctx, "Name", NK_TEXT_LEFT);
                nk_edit_string(ctx, NK_EDIT_FIELD, name_buffer, &len2, 64, nk_filter_default);

                nk_label(ctx, "Number", NK_TEXT_LEFT);
                nk_edit_string(ctx, NK_EDIT_FIELD, number_buffer, &len3, 64, nk_filter_float);

                // Add a dynamic row for a button
                nk_layout_row_dynamic(ctx, 30, 2);
                if (nk_button_label(ctx, "Cancel")) {
                    adding_contact = false;
                    fill(begin(name_buffer), end(name_buffer), ' ');
                    fill(begin(number_buffer), end(number_buffer), ' ');
                    len2 = 0;
                    len3 = 0;
                }
                if (nk_button_label(ctx, "OK")) {
                    adding_contact = false;
                    book.add_contact(string(name_buffer), stoi(number_buffer));
                    fill(begin(name_buffer), end(name_buffer), '\0');
                    fill(begin(number_buffer), end(number_buffer), '\0');
                    len2 = 0;
                    len3 = 0;
                }

            }
            nk_end(ctx);
        }
        
        glfwGetWindowSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);

        glfwSwapBuffers(window);
    }

    nk_glfw3_shutdown(&glfw);
    glfwTerminate();
    return 0;
}