get_filename_component(LIBRARY_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)

set(Header_Files "${PROJECT_NAME}_stdbool.h" "json_common.h" "cauthflow_types_common.h" "char_utils.h")
source_group("Header Files" FILES "${Header_Files}")

set(Source_Files "json_common.c" "char_utils.c")
source_group("Source Files" FILES "${Source_Files}")

add_library("${LIBRARY_NAME}" "${LIBRARY_TYPE_FLAG}" "${Header_Files}" "${Source_Files}")

find_package(parson CONFIG REQUIRED)
find_package(libcurl-simple-https CONFIG REQUIRED)

if (MSVC)
  find_package(CURL CONFIG REQUIRED)
  set(CURL_LINK_LIBRARIES CURL::libcurl)
else ()
  include(${CMAKE_SOURCE_DIR}/cmake/modules/FindCurlCustom.cmake)
  get_curl(CURL_LINK_LIBRARIES)
endif ()

target_link_libraries(
        "${LIBRARY_NAME}"
        PUBLIC
        "${CURL_LINK_LIBRARIES}"
        "libcurl-simple-https"
        "parson::parson"
)

target_link_libraries(
        "${LIBRARY_NAME}"
        PRIVATE
        "${PROJECT_NAME}_compiler_flags"
)
include(GNUInstallDirs)
target_include_directories(
        "${LIBRARY_NAME}"
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

set_target_properties(
        "${LIBRARY_NAME}"
        PROPERTIES
        LINKER_LANGUAGE
        C
)

include(GenerateExportHeader)
set(_export_file "${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}_export.h")
generate_export_header("${LIBRARY_NAME}" EXPORT_FILE_NAME "${_export_file}")

# install rules
install(FILES "${_export_file}" ${Header_Files} TYPE "INCLUDE")
