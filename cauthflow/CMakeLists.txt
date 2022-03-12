get_filename_component(LIBRARY_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)

set(Header_Files "cauthflow_server.h")
source_group("Header Files" FILES "${Header_Files}")

set(Source_Files "cauthflow_server.c")
source_group("Source Files" FILES "${Source_Files}")

add_library("${LIBRARY_NAME}" "${LIBRARY_TYPE_FLAG}" "${Header_Files}" "${Source_Files}")

target_include_directories(
        "${LIBRARY_NAME}"
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "$<INSTALL_INTERFACE:include>"
)

##########################
# 3rd party dependencies #
##########################

if (USE_OPENSSL)
  find_package(OpenSSL REQUIRED)
  target_link_libraries("${LIBRARY_NAME}" PRIVATE "OpenSSL::SSL" "OpenSSL::Crypto")
endif (USE_OPENSSL)

###########
# Linkage #
###########

#target_link_libraries("${LIBRARY_NAME}" PUBLIC "${PROJECT_NAME}_utils")

target_link_libraries(
        "${LIBRARY_NAME}"
        PRIVATE
        "${PROJECT_NAME}_google_auth"
        "${PROJECT_NAME}_compiler_flags"
)

target_include_directories(
        "${LIBRARY_NAME}"
        PRIVATE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "$<INSTALL_INTERFACE:include>"
)

if (WIN32)
  target_link_libraries("${LIBRARY_NAME}" PUBLIC wsock32 ws2_32)
endif (WIN32)

set_target_properties(
        "${LIBRARY_NAME}"
        PROPERTIES
        LINKER_LANGUAGE
        C
)

##########
# Export #
##########

include(GenerateExportHeader)
set(_export_file "${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}_export.h")
generate_export_header("${LIBRARY_NAME}" EXPORT_FILE_NAME "${_export_file}")

# setup the version numbering
set_property(TARGET "${LIBRARY_NAME}" PROPERTY VERSION "1.0.0")
set_property(TARGET "${LIBRARY_NAME}" PROPERTY SOVERSION "1")

###########
# Install #
###########

# install rules
include(GNUInstallDirs)
set(installable_libs
        "${LIBRARY_NAME}" "${PROJECT_NAME}_utils"
        "${PROJECT_NAME}_google_auth" "${PROJECT_NAME}_open_browser"
        "${PROJECT_NAME}_configure" "${PROJECT_NAME}_compiler_flags")
if (TARGET "${DEPENDANT_LIBRARY}")
  list(APPEND installable_libs "${DEPENDANT_LIBRARY}")
endif (TARGET "${DEPENDANT_LIBRARY}")
install(TARGETS ${installable_libs}
        EXPORT "${LIBRARY_NAME}Targets"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
install(FILES "${_export_file}" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_configure.h" ${Header_Files}
        TYPE "INCLUDE")
install(EXPORT "${LIBRARY_NAME}Targets" DESTINATION "${CMAKE_INSTALL_DATADIR}/${LIBRARY_NAME}")
