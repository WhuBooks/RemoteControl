project(VOLib)
add_executable( TestApp TestApp.cpp)
target_link_libraries(TestApp TestLib ${Third_Party_Libs})

add_executable(FastFeatureTest FastFeatureTest.cpp)
target_link_libraries(FastFeatureTest ${Third_Party_Libs})

add_executable(OrbFeatureTest OrbFeatureTest.cpp)
target_link_libraries(OrbFeatureTest ${Third_Party_Libs})