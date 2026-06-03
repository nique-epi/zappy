add_library(zappy_schema STATIC
  Tokenizer.cpp
  SchemaError.cpp
  Fields/StringFieldType.cpp
  Fields/NumberFieldType.cpp
  Fields/LongStringFieldType.cpp
)

target_include_directories(zappy_schema PUBLIC ${ZAPPY_COMMON_INCLUDE_DIR})
target_compile_options(zappy_schema PRIVATE -Wall -Wextra -Werror)
