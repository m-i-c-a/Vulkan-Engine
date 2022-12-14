#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "KTX::ktx" for configuration "Debug"
set_property(TARGET KTX::ktx APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(KTX::ktx PROPERTIES
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libktx.so.4.1.0"
  IMPORTED_SONAME_DEBUG "libktx.so.4"
  )

list(APPEND _IMPORT_CHECK_TARGETS KTX::ktx )
list(APPEND _IMPORT_CHECK_FILES_FOR_KTX::ktx "${_IMPORT_PREFIX}/lib/libktx.so.4.1.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
