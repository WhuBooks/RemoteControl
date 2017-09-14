project(VOLib)
add_library(TestLib TestLib.h TestLib.cpp)
target_link_libraries(TestLib ${Third_Party_Libs})