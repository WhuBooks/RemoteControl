# 编译选项
aux_source_directory("." JSON_SRC)
add_library(json STATIC ${JSON_SRC})

