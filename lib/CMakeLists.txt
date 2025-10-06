file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS src/*.cpp)

# Library target (all your core logic)
add_library(Krypton_lib ${LIB_SOURCES})
target_include_directories(Krypton_lib
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(Krypton_lib PUBLIC fmt::fmt)
