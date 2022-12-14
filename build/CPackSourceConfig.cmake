# This file will be configured to contain variables for CPack. These variables
# should be set in the CMake list file of the project before CPack module is
# included. The list of available CPACK_xxx variables and their associated
# documentation may be obtained using
#  cpack --help-variable-list
#
# Some variables are common to all generators (e.g. CPACK_PACKAGE_NAME)
# and some are specific to a generator
# (e.g. CPACK_NSIS_EXTRA_INSTALL_COMMANDS). The generator specific variables
# usually begin with CPACK_<GENNAME>_xxxx.


set(CPACK_APP_VALUE_LEN "1")
set(CPACK_BINARY_7Z "")
set(CPACK_BINARY_BUNDLE "")
set(CPACK_BINARY_CYGWIN "")
set(CPACK_BINARY_DEB "")
set(CPACK_BINARY_DRAGNDROP "")
set(CPACK_BINARY_FREEBSD "")
set(CPACK_BINARY_IFW "")
set(CPACK_BINARY_NSIS "")
set(CPACK_BINARY_NUGET "")
set(CPACK_BINARY_OSXX11 "")
set(CPACK_BINARY_PACKAGEMAKER "")
set(CPACK_BINARY_PRODUCTBUILD "")
set(CPACK_BINARY_RPM "")
set(CPACK_BINARY_STGZ "")
set(CPACK_BINARY_TBZ2 "")
set(CPACK_BINARY_TGZ "")
set(CPACK_BINARY_TXZ "")
set(CPACK_BINARY_TZ "")
set(CPACK_BINARY_WIX "")
set(CPACK_BINARY_ZIP "")
set(CPACK_BUILD_SOURCE_DIRS "/home/mica/Desktop/Vulkan/Vulkan-Engine;/home/mica/Desktop/Vulkan/Vulkan-Engine/build")
set(CPACK_CMAKE_GENERATOR "Unix Makefiles")
set(CPACK_COMPONENTS_ALL "library;dev;tools")
set(CPACK_COMPONENTS_ALL_SET_BY_USER "TRUE")
set(CPACK_COMPONENT_DEV_DEPENDS "library")
set(CPACK_COMPONENT_DEV_DESCRIPTION "Additional resources for development (header files and documentation).")
set(CPACK_COMPONENT_DEV_DISABLED "TRUE")
set(CPACK_COMPONENT_DEV_DISPLAY_NAME "Development")
set(CPACK_COMPONENT_DEV_DOWNLOADED "FALSE")
set(CPACK_COMPONENT_DEV_HIDDEN "FALSE")
set(CPACK_COMPONENT_DEV_REQUIRED "FALSE")
set(CPACK_COMPONENT_GLLOADTESTAPPS_DISABLED "TRUE")
set(CPACK_COMPONENT_GLLOADTESTAPPS_DISPLAY_NAME "OpenGL Test Applications")
set(CPACK_COMPONENT_GLLOADTESTAPPS_DOWNLOADED "FALSE")
set(CPACK_COMPONENT_GLLOADTESTAPPS_GROUP "LoadTestApps")
set(CPACK_COMPONENT_GLLOADTESTAPPS_HIDDEN "FALSE")
set(CPACK_COMPONENT_GLLOADTESTAPPS_REQUIRED "FALSE")
set(CPACK_COMPONENT_GROUP_LOADTESTAPPS_BOLD_TITLE "FALSE")
set(CPACK_COMPONENT_GROUP_LOADTESTAPPS_DISPLAY_NAME "Load Test Applications")
set(CPACK_COMPONENT_GROUP_LOADTESTAPPS_EXPANDED "FALSE")
set(CPACK_COMPONENT_JNI_DEPENDS "library")
set(CPACK_COMPONENT_JNI_DESCRIPTION "Java wrapper and native interface for KTX library.")
set(CPACK_COMPONENT_JNI_DISABLED "TRUE")
set(CPACK_COMPONENT_JNI_DISPLAY_NAME "Java wrapper")
set(CPACK_COMPONENT_JNI_DOWNLOADED "FALSE")
set(CPACK_COMPONENT_JNI_HIDDEN "FALSE")
set(CPACK_COMPONENT_JNI_REQUIRED "FALSE")
set(CPACK_COMPONENT_LIBRARY_DESCRIPTION "Main KTX library.")
set(CPACK_COMPONENT_LIBRARY_DISABLED "FALSE")
set(CPACK_COMPONENT_LIBRARY_DISPLAY_NAME "Library")
set(CPACK_COMPONENT_LIBRARY_DOWNLOADED "FALSE")
set(CPACK_COMPONENT_LIBRARY_HIDDEN "FALSE")
set(CPACK_COMPONENT_LIBRARY_REQUIRED "TRUE")
set(CPACK_COMPONENT_TOOLS_DEPENDS "library")
set(CPACK_COMPONENT_TOOLS_DESCRIPTION "Command line tools for creating, converting and inspecting KTX files.")
set(CPACK_COMPONENT_TOOLS_DISABLED "FALSE")
set(CPACK_COMPONENT_TOOLS_DISPLAY_NAME "Command line tools")
set(CPACK_COMPONENT_TOOLS_DOWNLOADED "FALSE")
set(CPACK_COMPONENT_TOOLS_HIDDEN "FALSE")
set(CPACK_COMPONENT_TOOLS_REQUIRED "FALSE")
set(CPACK_COMPONENT_UNSPECIFIED_HIDDEN "TRUE")
set(CPACK_COMPONENT_UNSPECIFIED_REQUIRED "TRUE")
set(CPACK_COMPONENT_VKLOADTESTAPP_DISABLED "TRUE")
set(CPACK_COMPONENT_VKLOADTESTAPP_DISPLAY_NAME "Vulkan Test Application")
set(CPACK_COMPONENT_VKLOADTESTAPP_DOWNLOADED "FALSE")
set(CPACK_COMPONENT_VKLOADTESTAPP_GROUP "LoadTestApps")
set(CPACK_COMPONENT_VKLOADTESTAPP_HIDDEN "FALSE")
set(CPACK_COMPONENT_VKLOADTESTAPP_REQUIRED "FALSE")
set(CPACK_DEFAULT_PACKAGE_DESCRIPTION_FILE "/usr/share/cmake-3.16/Templates/CPack.GenericDescription.txt")
set(CPACK_GENERATOR "TBZ2;TGZ;TXZ;TZ")
set(CPACK_IGNORE_FILES "/CVS/;/\\.svn/;/\\.bzr/;/\\.hg/;/\\.git/;\\.swp\$;\\.#;/#")
set(CPACK_INSTALLED_DIRECTORIES "/home/mica/Desktop/Vulkan/Vulkan-Engine;/")
set(CPACK_INSTALL_CMAKE_PROJECTS "")
set(CPACK_INSTALL_PREFIX "/usr/local")
set(CPACK_MODULE_PATH "/home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/KTX-Software/cmake/modules/")
set(CPACK_NSIS_DISPLAY_NAME "KTX-Software 0.1.1")
set(CPACK_NSIS_INSTALLER_ICON_CODE "")
set(CPACK_NSIS_INSTALLER_MUI_ICON_CODE "")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
set(CPACK_NSIS_PACKAGE_NAME "KTX-Software 0.1.1")
set(CPACK_OUTPUT_CONFIG_FILE "/home/mica/Desktop/Vulkan/Vulkan-Engine/build/CPackConfig.cmake")
set(CPACK_PACKAGE_CHECKSUM "SHA1")
set(CPACK_PACKAGE_CONTACT "khronos@callow.im")
set(CPACK_PACKAGE_DEFAULT_LOCATION "/")
set(CPACK_PACKAGE_DESCRIPTION_FILE "/home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/KTX-Software/cmake/ReadMe.txt")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "engine built using CMake")
set(CPACK_PACKAGE_FILE_NAME "KTX-Software-0.1.1-Source")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.khronos.org/KTX-Software")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "KTX-Software 0.1.1")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "KTX-Software 0.1.1")
set(CPACK_PACKAGE_NAME "KTX-Software")
set(CPACK_PACKAGE_RELOCATABLE "true")
set(CPACK_PACKAGE_VENDOR "Khronos Group")
set(CPACK_PACKAGE_VERSION "0.1.1")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "1")
set(CPACK_RESOURCE_FILE_LICENSE "/home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/KTX-Software/cmake/License.rtf")
set(CPACK_RESOURCE_FILE_README "/home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/KTX-Software/cmake/ReadMe.rtf")
set(CPACK_RESOURCE_FILE_WELCOME "/home/mica/Desktop/Vulkan/Vulkan-Engine/third-party/KTX-Software/cmake/Welcome.rtf")
set(CPACK_RPM_PACKAGE_SOURCES "ON")
set(CPACK_SET_DESTDIR "OFF")
set(CPACK_SOURCE_7Z "")
set(CPACK_SOURCE_CYGWIN "")
set(CPACK_SOURCE_GENERATOR "TBZ2;TGZ;TXZ;TZ")
set(CPACK_SOURCE_IGNORE_FILES "/CVS/;/\\.svn/;/\\.bzr/;/\\.hg/;/\\.git/;\\.swp\$;\\.#;/#")
set(CPACK_SOURCE_INSTALLED_DIRECTORIES "/home/mica/Desktop/Vulkan/Vulkan-Engine;/")
set(CPACK_SOURCE_OUTPUT_CONFIG_FILE "/home/mica/Desktop/Vulkan/Vulkan-Engine/build/CPackSourceConfig.cmake")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "KTX-Software-0.1.1-Source")
set(CPACK_SOURCE_RPM "OFF")
set(CPACK_SOURCE_TBZ2 "ON")
set(CPACK_SOURCE_TGZ "ON")
set(CPACK_SOURCE_TOPLEVEL_TAG "Linux-Source")
set(CPACK_SOURCE_TXZ "ON")
set(CPACK_SOURCE_TZ "ON")
set(CPACK_SOURCE_ZIP "OFF")
set(CPACK_STRIP_FILES "")
set(CPACK_SYSTEM_NAME "Linux")
set(CPACK_TOPLEVEL_TAG "Linux-Source")
set(CPACK_WIX_SIZEOF_VOID_P "8")

if(NOT CPACK_PROPERTIES_FILE)
  set(CPACK_PROPERTIES_FILE "/home/mica/Desktop/Vulkan/Vulkan-Engine/build/CPackProperties.cmake")
endif()

if(EXISTS ${CPACK_PROPERTIES_FILE})
  include(${CPACK_PROPERTIES_FILE})
endif()
