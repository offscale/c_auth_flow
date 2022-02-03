get_filename_component(LIBRARY_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
string(REPLACE " " "_" LIBRARY_NAME "${LIBRARY_NAME}")

include(GenerateExportHeader)

set(Header_Files "google_auth.h")
source_group("Header Files" FILES "${Header_Files}")

set(Source_Files "google_auth.c")
source_group("Source Files" FILES "${Source_Files}")

add_library("${LIBRARY_NAME}" "${LIBRARY_TYPE_FLAG}" "${Header_Files}" "${Source_Files}")

set(_libs
        "libcurl_simple_https"
        "common"
        "oauth2")

target_include_directories(
        "${LIBRARY_NAME}"
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/configure>"
        "$<INSTALL_INTERFACE:include>"
)

find_path(RANDOMSTR_INCLUDE_DIRS "randomstr.h")
find_path(LIBCURL_SIMPLE_HTTPS_INCLUDE_DIRS "curl_simple_https.h")

target_include_directories(
        "${LIBRARY_NAME}"
        PRIVATE
        "${RANDOMSTR_INCLUDE_DIRS}"
        "${LIBCURL_SIMPLE_HTTPS_INCLUDE_DIRS}"
)

find_package(unofficial-parson CONFIG REQUIRED)
set(_pub_libs "${PROJECT_NAME}_compiler_flags" "cauthflow_utils" "cauthflow_oauth2" "unofficial::parson::parson")
if (APPLE)
  list(APPEND _pub_libs "-framework CoreServices")
endif ()
if (NOT _GNU_SOURCE AND NOT HAVE_ASPRINTF)
  list(APPEND _libs "morestd")
endif (NOT _GNU_SOURCE AND NOT HAVE_ASPRINTF)

target_link_libraries("${LIBRARY_NAME}" PRIVATE "${_libs}")

target_link_libraries("${LIBRARY_NAME}" PUBLIC "${_pub_libs}")

set_target_properties(
        "${LIBRARY_NAME}"
        PROPERTIES
        LINKER_LANGUAGE
        C
)

set(_export_file "${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}_export.h")
generate_export_header("${LIBRARY_NAME}" EXPORT_FILE_NAME "${_export_file}")

# setup the version numbering
set_property(TARGET "${LIBRARY_NAME}" PROPERTY VERSION "1.0.0")
set_property(TARGET "${LIBRARY_NAME}" PROPERTY SOVERSION "1")

# install rules
set(installable_libs "${LIBRARY_NAME}" "${PROJECT_NAME}_compiler_flags")
if (TARGET "${DEPENDANT_LIBRARY}")
  list(APPEND installable_libs "${DEPENDANT_LIBRARY}")
endif (TARGET "${DEPENDANT_LIBRARY}")
install(TARGETS ${installable_libs}
        DESTINATION "${INSTALL_DEST}"
        EXPORT "${LIBRARY_NAME}Targets")
install(FILES "${_export_file}" ${Header_Files} DESTINATION "include")
