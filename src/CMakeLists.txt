set ( SRC_LIST
    geojsonWriter.cpp
    jsoncpp.cpp
    recordLoader.cpp
    simplifyLine.cpp
    transform.cpp
    )

add_executable(${PROJECT_NAME} main.cpp ${SRC_LIST})
MESSAGE(STATUS "Found OpenCV." ${OpenCV_LIBS})
target_link_libraries(
    ${PROJECT_NAME}
    ${OpenCV_LIBS}
)

add_executable("CheckLine" checkLine.cpp ${SRC_LIST})
target_link_libraries(
    "CheckLine"
    ${OpenCV_LIBS}
)
