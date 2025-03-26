
file(GLOB SRC_FILES *.c *.cpp)


idf_component_register(SRCS ${SRC_FILES} INCLUDE_DIRS ../include)
