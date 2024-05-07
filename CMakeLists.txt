cmake_minimum_required(VERSION 3.22)
project(Splotch)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HOST_NAME "localhost" CACHE STRING "Set the host name")
set(PORT "43845" CACHE STRING "Set the port")

add_definitions("-DHOST_NAME=\"${HOST_NAME}\"")
add_definitions("-DPORT=${PORT}")

find_package(OpenGL REQUIRED)
find_package(SFML COMPONENTS system network CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)

OPTION(ENABLE_SANITIZERS "Enable sanitizers" OFF)
OPTION(USE_TRACY "Enable Tracy profiling" OFF)

IF (MSVC)
    # Define ON_MSVC to use MSVC specific code
    add_compile_definitions(ON_MSVC)
ENDIF ()

file(GLOB_RECURSE DATA_FILES
        "data/*.png"
        "data/*.jpg"
        "data/*.ttf"
)
foreach (DATA ${DATA_FILES})
    get_filename_component(FILE_NAME ${DATA} NAME)
    get_filename_component(PATH_NAME ${DATA} DIRECTORY)
    get_filename_component(EXTENSION ${DATA} EXT)
    file(RELATIVE_PATH PATH_NAME "${CMAKE_CURRENT_SOURCE_DIR}" ${PATH_NAME})
    #MESSAGE("Data PATH: ${PATH_NAME} NAME: ${FILE_NAME}")
    set(DATA_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${PATH_NAME}/${FILE_NAME}")
    #MESSAGE("Data OUT PATH: ${DATA_OUTPUT}")
    add_custom_command(
            OUTPUT ${DATA_OUTPUT}
            COMMAND ${CMAKE_COMMAND} -E copy
            ${DATA}
            ${DATA_OUTPUT}
            DEPENDS ${DATA})
    list(APPEND Data_OUTPUT_FILES ${DATA_OUTPUT})
endforeach (DATA)


add_custom_target(
        data_target
        DEPENDS ${Data_OUTPUT_FILES}
)

# Physics common library
file(GLOB_RECURSE PHYSICS_COMMON_FILES libs/PhysicsCommon/include/*.h libs/PhysicsCommon/src/*.cpp)
add_library(PhysicsCommon ${PHYSICS_COMMON_FILES})
set_target_properties(PhysicsCommon PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(PhysicsCommon PUBLIC libs/PhysicsCommon/include/)
target_include_directories(PhysicsCommon PUBLIC libs/Math/)

# Physics library
file(GLOB_RECURSE PHYSICS_ENGINE_FILES libs/Physics/include/*.h libs/Physics/src/*.cpp)
add_library(PhysicsEngine ${PHYSICS_ENGINE_FILES})
set_target_properties(PhysicsEngine PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(PhysicsEngine PUBLIC libs/Physics/include/)
target_include_directories(PhysicsEngine PUBLIC libs/Math/)
target_link_libraries(PhysicsEngine PUBLIC PhysicsCommon fmt::fmt)

# Client
file(GLOB_RECURSE CLIENT_FILES client/src/*.cpp client/include/*.h)
add_library(ClientPart STATIC ${CLIENT_FILES})
target_include_directories(ClientPart PUBLIC client/include/)
target_include_directories(ClientPart PUBLIC client/src/)
target_link_libraries(ClientPart PUBLIC sfml-graphics sfml-window sfml-system sfml-network)

if(MSVC)
    target_link_libraries(ClientPart PRIVATE sfml-main FLAC OpenAL OpenGL Vorbis)
else()
    target_link_libraries(ClientPart PRIVATE X11 FLAC UDev OpenAL ${OPENGL_LIBRARIES})
endif()

# Server
file(GLOB_RECURSE SERVER_FILES server/src/*.cpp server/include/*.h)
add_library(ServerPart STATIC ${SERVER_FILES})
target_include_directories(ServerPart PUBLIC server/include/)

# Common
file(GLOB_RECURSE COMMON_FILES common/include/*.h common/src/*.cpp)
add_library(Common STATIC ${COMMON_FILES})
target_include_directories(Common PUBLIC common/include/)
target_link_libraries(Common PUBLIC PhysicsEngine)

# Network
# Common
file(GLOB_RECURSE NETWORK_COMMON_FILES network/common/include/*.h network/common/src/*.cpp)
add_library(NetworkCommon STATIC ${NETWORK_COMMON_FILES})
target_include_directories(NetworkCommon PUBLIC network/common/include/)
target_link_libraries(NetworkCommon PUBLIC sfml-system sfml-network)
target_link_libraries(NetworkCommon PUBLIC Common)
# Client
add_library(NetworkClient INTERFACE)
set_target_properties(NetworkClient PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(NetworkClient INTERFACE network/client/include/)
target_link_libraries(NetworkClient INTERFACE NetworkCommon)
# Server
add_library(NetworkServer INTERFACE)
set_target_properties(NetworkServer PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(NetworkServer INTERFACE network/server/include/)
target_link_libraries(NetworkServer INTERFACE NetworkCommon)

# Tracy
if (USE_TRACY)
    add_subdirectory(libs/tracy-0.10) # target: TracyClient or alias Tracy::TracyClient
    # Link Tracy::TracyClient to any target where you use Tracy for profiling:
    target_link_libraries(ClientPart PUBLIC Tracy::TracyClient)
    target_link_libraries(ServerPart PUBLIC Tracy::TracyClient)
    target_link_libraries(Common PUBLIC Tracy::TracyClient)
    target_link_libraries(PhysicsEngine PUBLIC Tracy::TracyClient)
endif()

# Linking
target_link_libraries(Common PUBLIC NetworkCommon)

target_link_libraries(ClientPart PUBLIC NetworkClient PhysicsEngine)
target_link_libraries(ServerPart PUBLIC NetworkServer PhysicsEngine)

add_executable(client MainClient.cpp)
add_executable(server MainServer.cpp)
add_executable(allInOne MainAllInOne.cpp)
add_executable(splitScreen MainSplitScreen.cpp)

target_link_libraries(client PUBLIC ClientPart)
target_link_libraries(server PUBLIC ServerPart)
target_link_libraries(allInOne PUBLIC ClientPart ServerPart)
target_link_libraries(splitScreen PUBLIC ClientPart ServerPart ImGui-SFML::ImGui-SFML)

add_dependencies(client data_target)
add_dependencies(allInOne data_target)
add_dependencies(splitScreen data_target)

file(GLOB_RECURSE TEST_FILES tests/*.cpp)
foreach(test_file ${TEST_FILES} )
    get_filename_component(test_name ${test_file} NAME_WE)

    add_executable(${test_name} ${test_file})

    target_link_libraries(${test_name} PRIVATE GTest::gtest GTest::gtest_main)
    target_link_libraries(${test_name} PUBLIC ClientPart ServerPart)
endforeach()