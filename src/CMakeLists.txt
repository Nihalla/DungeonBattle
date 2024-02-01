## local variable for managing the gamelib target
set(GAMELIB "ASGEGameLib")

## glob is useful but not reliant in compilation so just use it for headers ##
file(GLOB_RECURSE HEADER_LIST
     LIST_DIRECTORIES FALSE
     CONFIGURE_DEPENDS
     ${CMAKE_SOURCE_DIR}/include/${GAMELIB}/*.hpp
     ${CMAKE_SOURCE_DIR}/include/${GAMELIB}/*.h
     *.hpp *.h)

## cpp files go here, [h]pp files are globbed so don't need to be explicitly added ##
set(GAMELIB_SOURCES
        ${GAMELIB}/GComponent.cpp
        ${GAMELIB}/GCNetClient.cpp
        ${GAMELIB}/GCNetServer.cpp
        ${GAMELIB}/Unit.cpp
        ${GAMELIB}/MyPacket.cpp
        ${GAMELIB}/Archer.cpp
        ${GAMELIB}/Knight.cpp
        ${GAMELIB}/Paladin.cpp)

## create the game library
add_library(${GAMELIB} STATIC ${GAMELIB_SOURCES} "${HEADER_LIST}")
add_library(libs::${GAMELIB} ALIAS ${GAMELIB})
target_include_directories(${GAMELIB} PRIVATE ${CMAKE_SOURCE_DIR}/include/${GAMELIB} ${CMAKE_SOURCE_DIR}/src/${GAMELIB})
target_compile_options(${GAMELIB} PUBLIC $<$<STREQUAL:x${CMAKE_CXX_SIMULATE_ID},xMSVC>:/EHsc>)

## set the build target properties ##
configure_target(${GAMELIB} ${CMAKE_SOURCE_DIR}/include "${HEADER_LIST}" "kissnet::kissnet")
add_asge_dependency(${GAMELIB})