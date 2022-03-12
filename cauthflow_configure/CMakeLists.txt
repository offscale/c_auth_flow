get_filename_component(LIBRARY_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)

###########################
# Vars for configure file #
###########################

if (NOT DEFINED STACK_SIZE)
  if (WIN32)
    set(STACK_SIZE 16384)
  else ()
    set(STACK_SIZE 65535)
  endif ()
endif ()

if (NOT DEFINED SERVER_ADDR)
  set(SERVER_ADDR "127.0.0.1")
endif ()

if (NOT DEFINED SERVER_HOST)
  set(SERVER_HOST "localhost")
endif ()

if (NOT DEFINED PORT_TO_BIND)
  set(PORT_TO_BIND "3000")
endif ()

if (NOT DEFINED MSG_BACKLOG)
  set(MSG_BACKLOG 5)
endif ()

if (NOT DEFINED EXPECTED_PATH)
  set(EXPECTED_PATH "/ibm/cloud/appid/callback")
endif ()

if (NOT DEFINED API_HOST)
  set(API_HOST "31f5ff35.eu-gb.apigw.appdomain.cloud")
endif ()

if (NOT DEFINED API_APPLICATION_ENDPOINT_PATH)
  set(API_APPLICATION_ENDPOINT_PATH "/authtest/GetApplicationEndpoint")
endif ()

if (NOT DEFINED API_GET_ACCESS_TOKEN_PATH)
  set(API_GET_ACCESS_TOKEN_PATH "/authtest/GetAccessToken")
endif ()

# https://github.com/GoogleCloudPlatform/gsutil/blob/7d103/gslib/utils/system_util.py#L174-L193
if (NOT DEFINED CLIENT_ID)
  set(CLIENT_ID "32555940559.apps.googleusercontent.com")
  # set(CLIENT_ID "909320924072.apps.googleusercontent.com")
endif ()

# https://github.com/GoogleCloudPlatform/gsutil/blob/7d103/gslib/utils/system_util.py#L174-L193
if (NOT DEFINED CLIENT_SECRET)
  set(CLIENT_SECRET "ZmssLNjJy2998hD4CTg2ejr2")
  # set(CLIENT_SECRET "p3RlpR10xMFh9ZXBS/ZNLYUu")
endif ()

if (NOT DEFINED TARGET_ARCH)
  set(TARGET_ARCH ${CMAKE_HOST_SYSTEM_PROCESSOR})
endif ()

if (NOT DEFINED CRYPTO_LIB)
  set(CRYPTO_LIB OpenSSL)
endif ()
if (CRYPTO_LIB STREQUAL "OpenSSL")
  set(USE_OPENSSL 1)
endif ()

################################
# Configure file (interpolate) #
################################

configure_file("configure.h.in" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_configure.h" @ONLY)

set(Header_Files "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_configure.h")
source_group("Header Files" FILES "${Header_Files}")

add_library("${LIBRARY_NAME}" INTERFACE)

target_include_directories(
        "${LIBRARY_NAME}"
        INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
        "$<INSTALL_INTERFACE:include>"
)

set_target_properties(
        "${LIBRARY_NAME}"
        PROPERTIES
        LINKER_LANGUAGE
        C
)

# setup the version numbering
set_property(TARGET "${LIBRARY_NAME}" PROPERTY VERSION "1.0.0")
set_property(TARGET "${LIBRARY_NAME}" PROPERTY SOVERSION "1")

# install rules
include(GNUInstallDirs)
install(FILES ${Header_Files} TYPE "INCLUDE")
