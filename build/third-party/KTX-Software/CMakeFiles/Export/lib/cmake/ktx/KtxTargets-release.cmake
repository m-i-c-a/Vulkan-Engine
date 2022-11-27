#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "KTX::ktx" for configuration "Release"
set_property(TARGET KTX::ktx APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(KTX::ktx PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libktx.so.4.1.0"
  IMPORTED_SONAME_RELEASE "libktx.so.4"
  )

list(APPEND _IMPORT_CHECK_TARGETS KTX::ktx )
list(APPEND _IMPORT_CHECK_FILES_FOR_KTX::ktx "${_IMPORT_PREFIX}/lib/libktx.so.4.1.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
