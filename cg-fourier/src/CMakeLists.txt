file(GLOB_RECURSE SRCS ./*.cpp)
file(GLOB_RECURSE HDRS ./*.h)

add_executable(fourier ${SRCS} ${HDRS})
target_link_libraries(fourier pmp implot)


