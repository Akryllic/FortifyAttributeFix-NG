if(NOT DEFINED ENV{COMMONLIB_SSE_FOLDER})
  message(FATAL_ERROR "Missing COMMONLIB_SSE_FOLDER environment variable")
endif()

include(cmake/lib/copyOutputs.cmake)
include(cmake/lib/autoCollectSources.cmake)

add_src_folder(src)
add_include_folder(include)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

cmake_minimum_required(VERSION 3.21)

# ---------------------------------------------------------------------------
# Rename these three, and the .ini under data/SKSE/Plugins/, then delete this block.
# PRODUCT_NAME becomes the DLL name and the log file name.
# ---------------------------------------------------------------------------
set(AUTHOR_NAME "Apodeictic")
set(PRODUCT_NAME "FortifyAttributeFixNG")
project(${PRODUCT_NAME} VERSION 1.1.0.0 LANGUAGES CXX)

# Where the built DLL and everything under data/ is copied after each build.
# Point this at the MO2 mod folder itself, not the mods root.
if(NOT DEFINED MOD_FOLDER)
  set(MOD_FOLDER "C:/Lumina/mods/Fortify Attribute Fix - NG")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/version.rc
        @ONLY)

set(RESOURCE_FILE ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
set(RESOURCE_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/version.res)

add_custom_command(
    OUTPUT ${RESOURCE_OUTPUT}
    COMMAND rc /fo ${RESOURCE_OUTPUT} ${RESOURCE_FILE}
    DEPENDS ${RESOURCE_FILE}
)

add_custom_target(Resource ALL DEPENDS ${RESOURCE_OUTPUT})

set(ENABLE_SKYRIM_SE OFF CACHE BOOL " " FORCE)
set(ENABLE_SKYRIM_AE ON CACHE BOOL " " FORCE)
set(ENABLE_SKYRIM_VR ON CACHE BOOL " " FORCE)
set(BUILD_TESTS OFF CACHE BOOL " " FORCE)

add_subdirectory("$ENV{COMMONLIB_SSE_FOLDER}" ${CMAKE_CURRENT_BINARY_DIR}/_deps/clib-build)
get_target_property(COMMONLIB_SRC_DIR CommonLibSSE SOURCE_DIR)

file(
  GLOB_RECURSE
  sources
  ${CMAKE_CURRENT_BINARY_DIR}/version.rc
  ${sources}
)

source_group(
  TREE ${CMAKE_CURRENT_SOURCE_DIR}
  FILES ${headers} ${sources}
)

include(${COMMONLIB_SRC_DIR}/cmake/CommonLibSSE.cmake)

add_commonlibsse_plugin(
  ${PROJECT_NAME}
  AUTHOR AUTHOR_NAME
  SOURCES ${headers} ${sources}
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
target_precompile_headers(${PROJECT_NAME} PRIVATE include/PCH.h)
target_include_directories(
  ${PROJECT_NAME}
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Two published builds from one source tree. OFF compiles the Debug page out entirely rather than
# hiding it, so the release download carries no debug UI at all.
#   cmake --preset release -DFF_DEBUG_PANEL=ON
option(FF_DEBUG_PANEL "Include the SKSE Menu Framework Debug page" OFF)
if(FF_DEBUG_PANEL)
  target_compile_definitions(${PROJECT_NAME} PRIVATE FF_DEBUG_PANEL)

  # Symbols so a crash log from this build resolves function names. Release optimisation is kept
  # deliberately: RelWithDebInfo drops link-time codegen and produces a far larger DLL and PDB
  set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_DEBUG_INFORMATION_FORMAT "ProgramDatabase")
  target_link_options(${PROJECT_NAME} PRIVATE /DEBUG /OPT:REF /OPT:ICF)

  # copyOutputs only ships the pdb for Debug and RelWithDebInfo builds, and this is Release plus
  # /DEBUG. Without this a rebuild leaves the previous pdb next to a new dll, and a crash log then
  # resolves to the wrong function names, which is worse than having no symbols at all
  if(IS_DIRECTORY "${MOD_FOLDER}")
    add_custom_command(
      TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E copy_if_different
              "$<TARGET_PDB_FILE:${PROJECT_NAME}>"
              "${MOD_FOLDER}/SKSE/Plugins/$<TARGET_PDB_FILE_NAME:${PROJECT_NAME}>"
      VERBATIM)
  endif()

  message(STATUS "${PRODUCT_NAME}: DEBUG build, Debug page and PDB included")
else()
  # A release dll sitting beside a debug pdb is the same wrong-symbols trap in reverse
  if(IS_DIRECTORY "${MOD_FOLDER}")
    add_custom_command(
      TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND "${CMAKE_COMMAND}" -E rm -f
              "${MOD_FOLDER}/SKSE/Plugins/${PRODUCT_NAME}.pdb"
      VERBATIM)
  endif()

  message(STATUS "${PRODUCT_NAME}: RELEASE build, no debug UI")
endif()

if(IS_DIRECTORY "${MOD_FOLDER}")
  copyOutputs("${MOD_FOLDER}")
else()
  message(WARNING "MOD_FOLDER not found, the build will not deploy: ${MOD_FOLDER}")
endif()
