#file(GLOB SOURCES ./*.cpp)

set(IMGUI_SRC
    ./imgui.cpp
    ./imgui_demo.cpp
    ./imgui_draw.cpp
    ./imgui_widgets.cpp
    ./imgui_tables.cpp
    ./imgui_impl_glfw.cpp
    ./imgui_impl_opengl3.cpp
)

file(GLOB HEADERS ./*.h)
add_library(imgui STATIC ${IMGUI_SRC} ${HEADERS})
target_compile_definitions(imgui PUBLIC -DIMGUI_IMPL_OPENGL_LOADER_GLEW)
