cmake_minimum_required(VERSION 3.16)
project(MediCore)

set(CMAKE_CXX_STANDARD 17)

set(SFML_DIR "C:/SFML/SFML-3.1.0/lib/cmake/SFML")

find_package(SFML 3.1 COMPONENTS Graphics Window System REQUIRED)

set(SOURCES
    main.cpp
    src/Person.cpp
    src/Patient.cpp
    src/Doctor.cpp
    src/Admin.cpp
    src/Appointment.cpp
    src/Bill.cpp
    src/Prescription.cpp
    src/FileHandler.cpp
    src/Validator.cpp
    src/Menus.cpp
    src/GUI.cpp
)

add_executable(MediCore ${SOURCES})
target_include_directories(MediCore PRIVATE include)
target_link_libraries(MediCore SFML::Graphics SFML::Window SFML::System)
