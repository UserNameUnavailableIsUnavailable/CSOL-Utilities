# Install onnxruntime manually
cmake_minimum_required(VERSION 3.20)

if (onnxruntime_installed)
    return()
endif()
set(VERSION "1.22.0")

set(ZIP_NAME "onnxruntime-win-x64-${VERSION}")
set(ZIP_URL "https://github.com/microsoft/onnxruntime/releases/download/v${VERSION}/${ZIP_NAME}.zip")
set(ZIP_PATH "${DOWNLOADS_DIR}/${ZIP_NAME}.zip")
set(EXTRACT_DIR "${CMAKE_BINARY_DIR}/downloads")

# Check if HTTP_PROXY or HTTPS_PROXY is set
if(DEFINED ENV{HTTPS_PROXY})
    execute_process(
        COMMAND curl -L -x "$ENV{HTTPS_PROXY}" -o "${ZIP_PATH}" "${ZIP_URL}"
        RESULT_VARIABLE CURL_RESULT
        OUTPUT_VARIABLE CURL_OUTPUT
    )
    message(STATUS "Download ${ZIP_URL} using HTTPS proxy: $ENV{HTTPS_PROXY}")
elseif(DEFINED ENV{HTTP_PROXY})
    execute_process(
        COMMAND curl -L -x "$ENV{HTTP_PROXY}" -o "${ZIP_PATH}" "${ZIP_URL}"
        RESULT_VARIABLE CURL_RESULT
        OUTPUT_VARIABLE CURL_OUTPUT
        ERROR_VARIABLE CURL_ERROR
    )
    message(STATUS "Download ${ZIP_URL} using HTTP proxy: $ENV{HTTP_PROXY}")
else()
    message(STATUS "Download ${ZIP_URL} without proxy")
    execute_process(
        COMMAND curl -L -o "${ZIP_PATH}" "${ZIP_URL}"
        RESULT_VARIABLE CURL_RESULT
        OUTPUT_VARIABLE CURL_OUTPUT
        ERROR_VARIABLE CURL_ERROR
    )
endif()

# Check if the download was successful
if (NOT CURL_RESULT EQUAL 0)
    message(FATAL_ERROR "Failed to download ${ZIP_URL}: ${CURL_ERROR}")
endif()

# Extract the zip file
file(ARCHIVE_EXTRACT
    INPUT "${ZIP_PATH}"
    DESTINATION "${EXTRACT_DIR}"
)

file(GLOB items "${CMAKE_BINARY_DIR}/downloads/${ZIP_NAME}/include/*")
foreach(item IN LISTS items)
    file(COPY "${item}" DESTINATION "${DEPENDENCIES_DIR}/include/onnxruntime")
endforeach(item IN LISTS items)

file(GLOB items "${CMAKE_BINARY_DIR}/downloads/${ZIP_NAME}/lib/*")
foreach(item IN LISTS items)
    # copy .lib to lib
    if (item MATCHES "\\.lib$")
        file(COPY "${item}" DESTINATION "${DEPENDENCIES_DIR}/lib")
    endif()
    # copy .dll and .pdb to bin
    if (item MATCHES "\\.dll$")
        file(COPY "${item}" DESTINATION "${DEPENDENCIES_DIR}/bin")
    elseif (item MATCHES "\\.pdb$")
        file(COPY "${item}" DESTINATION "${DEPENDENCIES_DIR}/bin")
    endif()
endforeach(item IN LISTS items)
file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/downloads/${ZIP_NAME}")
set(onnxruntime_installed ON CACHE BOOL "onnxruntime installed" FORCE)