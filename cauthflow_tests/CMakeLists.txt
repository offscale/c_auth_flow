get_filename_component(EXEC_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)

#########################
# Dependencies download #
#########################

set(GREATEST_SHA256 "1292593d95c35eeccc89ffa1c91d6fe53b49f81cbf2c2b7758842b7f3186fcc2")
set(DOWNLOAD_DIR "${PROJECT_BINARY_DIR}/test_downloads")

file(MAKE_DIRECTORY "${DOWNLOAD_DIR}")
set(GREATEST_FILE "${DOWNLOAD_DIR}/greatest.h")
set(GREATEST_URL "https://raw.githubusercontent.com/silentbicycle/greatest/11a6af1/greatest.h")
file(DOWNLOAD "${GREATEST_URL}" "${GREATEST_FILE}"
        EXPECTED_HASH "SHA256=${GREATEST_SHA256}")

###############
# Test target #
###############

set(Header_Files "${CMAKE_CURRENT_SOURCE_DIR}/test_cauthflow.h")
source_group("${EXEC_NAME} Header Files" FILES "${Header_Files}")

set(Source_Files "${CMAKE_CURRENT_SOURCE_DIR}/test.c")
source_group("${EXEC_NAME} Source Files" FILES "${Source_Files}")

add_executable("${EXEC_NAME}" "${Header_Files}" "${GREATEST_FILE}" "${Source_Files}")

target_include_directories(
        "${EXEC_NAME}"
        PRIVATE
        "${DOWNLOAD_DIR}"
)

target_link_libraries(
        "${EXEC_NAME}"
        PRIVATE
        "${PROJECT_NAME}_compiler_flags"
        "${PROJECT_NAME}_utils"
        "${PROJECT_NAME}"
        "${PROJECT_NAME}_google_auth"
)

add_test(NAME "${EXEC_NAME}" COMMAND "${EXEC_NAME}")
