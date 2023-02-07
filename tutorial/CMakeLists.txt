set(FOUNDATION_DIR ${TOOLS_DIR}/foundation)

add_subdirectory(client)
add_subdirectory(server)

# 将模块添加到自定义目录中
SET_PROPERTY(TARGET client PROPERTY FOLDER "Tutorial")
SET_PROPERTY(TARGET server PROPERTY FOLDER "Tutorial")