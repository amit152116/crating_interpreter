file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS src/*.cpp)

# Library target (all your core logic)
add_library(Thor_lib ${LIB_SOURCES})
target_include_directories(Thor_lib
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Thor_lib PUBLIC fmt::fmt)
