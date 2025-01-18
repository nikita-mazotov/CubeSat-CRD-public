cmake_minimum_required(VERSION 3.15)

project(MyGeant4App)

# Find Geant4 package
find_package(Geant4 REQUIRED)

# clangd quiet
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include Geant4 Use file
include(${Geant4_USE_FILE})

# Add sources
add_executable(myApp src/main.cpp)

# Link the executable to Geant4 libraries
target_link_libraries(myApp ${Geant4_LIBRARIES})