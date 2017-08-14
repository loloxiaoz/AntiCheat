FILE(GLOB SRC_LIST *.cpp)

add_executable(${PROJECT_NAME} ${SRC_LIST})
MESSAGE(STATUS "Found OpenCV." ${OpenCV_LIBS})
target_link_libraries(
    ${PROJECT_NAME}
    ${OpenCV_LIBS}
)
