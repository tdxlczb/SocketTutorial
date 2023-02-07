set(TARGET_NAME server)
add_executable(${TARGET_NAME})

file(GLOB_RECURSE ${TARGET_NAME}_SOURCES FOLLOW_SYMLINKS LIST_DIRECTORIES false CONFIGURE_DEPENDS
     *.h
     *.cpp
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${${TARGET_NAME}_SOURCES})
target_sources(${TARGET_NAME} PRIVATE ${${TARGET_NAME}_SOURCES})

target_link_libraries(${TARGET_NAME} PRIVATE ws2_32)
target_link_libraries(${TARGET_NAME} PRIVATE foundation)
target_link_libraries(${TARGET_NAME} PRIVATE u2socket)

install_executable(${TARGET_NAME})