# Install onnxruntime manually
cmake_minimum_required(VERSION 3.20)

if (onnxruntime_installed)
    return()
endif()
set(VERSION "1.22.0")
set(ZIP_NAME "onnxruntime-win-x64-${VERSION}")
set(ZIP_URL "https://github.com/microsoft/onnxruntime/releases/download/v${VERSION}/${ZIP_NAME}.zip")
set(ZIP_PATH "${CMAKE_BINARY_DIR}/downloads/${ZIP_NAME}.zip")
set(EXTRACT_DIR "${CMAKE_BINARY_DIR}/downloads")

# Download the zip file
file(DOWNLOAD
    "${ZIP_URL}"
    "${ZIP_PATH}"
)

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