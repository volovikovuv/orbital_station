cmake_minimum_required(VERSION 4.3)
project(orbital_station)

set(CMAKE_CXX_STANDARD 26)

add_executable(orbital_station src/main.cpp
        include/module_attributes.h
        include/module.h
        include/module_hierarchy.h
        include/station_iterator.h
        include/station.h
        include/module_factory.h
        src/module_attributes.cpp
        src/module.cpp
        src/module_hierarchy.cpp
        src/station.cpp
        src/module_factory.cpp
        src/station_iterator.cpp)
