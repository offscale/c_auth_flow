get_filename_component(LIBRARY_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
string(REPLACE " " "_" LIBRARY_NAME "${LIBRARY_NAME}")

set(Header_Files "google_auth.h")
source_group("Header Files" FILES "${Header_Files}")

set(Source_Files "google_auth.c")
source_group("Source Files" FILES "${Source_Files}")

add_library("${LIBRARY_NAME}" "${LIBRARY_TYPE_FLAG}" "${Header_Files}" "${Source_Files}")

target_include_directories(
        "${LIBRARY_NAME}"
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/configure>"
        "$<INSTALL_INTERFACE:include>"
)

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
)

find_path(RANDOMSTR_INCLUDE_DIRS "randomstr.h")

target_include_directories(
        "${LIBRARY_NAME}"
        PRIVATE
        "${RANDOMSTR_INCLUDE_DIRS}"
        "${LIBCURL_SIMPLE_HTTPS_INCLUDE_DIRS}"
)

find_package(libcurl-simple-https CONFIG REQUIRED)
find_package(unofficial-parson CONFIG REQUIRED)
set(_libs "unofficial::parson::parson")
if (APPLE)
  list(APPEND _libs "-framework CoreServices")
endif (APPLE)

target_link_libraries(
        "${LIBRARY_NAME}"
        PRIVATE
        "${_libs}"
        "libcurl-simple-https"
        "${PROJECT_NAME}_utils"
        "${PROJECT_NAME}"
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

# setup the version numbering
set_property(TARGET "${LIBRARY_NAME}" PROPERTY VERSION "1.0.0")
set_property(TARGET "${LIBRARY_NAME}" PROPERTY SOVERSION "1")

# install rules
set(installable_libs "${LIBRARY_NAME}")
if (TARGET "${DEPENDANT_LIBRARY}")
  list(APPEND installable_libs "${DEPENDANT_LIBRARY}")
endif (TARGET "${DEPENDANT_LIBRARY}")
install(TARGETS ${installable_libs}
        DESTINATION "lib"
        EXPORT "${LIBRARY_NAME}Targets")
install(FILES "${_export_file}" ${Header_Files} DESTINATION "include")
install(EXPORT "${LIBRARY_NAME}Targets" DESTINATION "share/${PROJECT_NAME}")
