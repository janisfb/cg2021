file(GLOB SOURCES ./*.cpp)
file(GLOB HEADERS ./*.h)

add_library(implot STATIC ${SOURCES} ${HEADERS})
target_link_libraries(implot imgui)
