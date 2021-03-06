# The CMake version we require
cmake_minimum_required(VERSION 3.1)

# Setting the name of the main project
project(KMyMoney VERSION "5.0.80" LANGUAGES CXX)

# Determine the GIT reference (if we're based on GIT)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
  execute_process(COMMAND git rev-parse --short HEAD WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" OUTPUT_VARIABLE VERSION_SUFFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(VERSION_SUFFIX "-${VERSION_SUFFIX}")
  # Add variables which are similar to the build in names of cmake
  set(PROJECT_VERSION_SUFFIX "${VERSION_SUFFIX}")
  set(${PROJECT_NAME}_VERSION_SUFFIX "${VERSION_SUFFIX}")
endif()

# Automoc all sources
set(CMAKE_AUTOMOC TRUE)
list(APPEND CMAKE_AUTOMOC_MACRO_NAMES "K_PLUGIN_FACTORY" "K_PLUGIN_FACTORY_WITH_JSON")

if (POLICY CMP0063)
  cmake_policy(SET CMP0063 NEW) # Policy introduced in CMake version 3.3
endif()
if (POLICY CMP0071)
  # We do not require the old behaviour. It is only set to old, to prevent accidential use of
  # the new behavour. If the new behaviour becomes important, cmake_minimum_required has to be
  # set to "3.10".
  cmake_policy(SET CMP0071 OLD) # Policy introduced in CMake version 3.10
endif()


######################### General Requirements ##########################

if (CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 6.0.0)
    message(FATAL_ERROR "This version of KMyMoney requires at least gcc 6.0.0 to be built successfully")
endif()

find_package(ECM 5.10 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(FeatureSummary)
include(CMakeDependentOption)

include(GenerateExportHeader)
include(KMyMoneyMacros)

find_package(PkgConfig)

set (OPT_KF5_COMPONENTS DocTools Holidays Contacts Akonadi IdentityManagement Activities)
find_package(Gpgmepp)
if (Gpgmepp_FOUND)
  set(GPG_FOUND TRUE)
else()
  set(GPG_FOUND FALSE)
  list(APPEND OPT_KF5_COMPONENTS Gpgmepp)
endif()

if (PkgConfig_FOUND)
  pkg_check_modules(SQLCIPHER sqlcipher)
endif()

find_package(Qt5 5.6 REQUIRED
  COMPONENTS Core DBus Widgets Svg Xml Test PrintSupport
  OPTIONAL_COMPONENTS Sql Concurrent QuickWidgets)

find_package(KF5 5.2 REQUIRED
  COMPONENTS Archive CoreAddons Config ConfigWidgets I18n Completion KCMUtils ItemModels ItemViews Service Wallet IconThemes XmlGui TextWidgets Notifications KIO
  OPTIONAL_COMPONENTS ${OPT_KF5_COMPONENTS}
)
find_package(LibAlkimia5 7.0.0 REQUIRED)

# Recent changes to LibAlkimia should allow us to remove this construct
#if(CMAKE_SYSTEM_NAME MATCHES "Windows")
#  include_directories(${GMP_INCLUDE_DIR})
#endif()

find_package(KChart 2.6.0)

if(KF5Gpgmepp_FOUND)
  set(GPG_FOUND TRUE)
endif()

cmake_dependent_option(ENABLE_GPG "Enable GPG support." ON
                       "GPG_FOUND" OFF)

add_feature_info("Encryption" ENABLE_GPG "Allows to store your financial data using strong GPG encryption.")

add_definitions(-DQT_USE_QSTRINGBUILDER -DQT_NO_CAST_TO_ASCII -DQT_NO_URL_CAST_FROM_STRING)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# use DBus only on Linux
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  set(KMM_DBUS 1)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

# check for Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
  set(APIDOC_DIR ${CMAKE_CURRENT_BINARY_DIR}/apidocs)

  make_directory("${APIDOC_DIR}")
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/kmymoney.doxygen.in ${CMAKE_CURRENT_BINARY_DIR}/kmymoney.doxygen IMMEDIATE)

  add_custom_target(apidoc "${DOXYGEN}" "${CMAKE_CURRENT_BINARY_DIR}/kmymoney.doxygen")
endif(DOXYGEN_FOUND)
add_feature_info("Doxygen" DOXYGEN_FOUND "Generate API documentation with Doxygen (for devs only).")

# check some include files exists
set(CMAKE_REQUIRED_DEFINITIONS -D_XOPEN_SOURCE=500 -D_BSD_SOURCE)
include (CheckIncludeFileCXX)
check_include_file_cxx("unistd.h" HAVE_UNISTD_H)
check_include_file_cxx("pwd.h" HAVE_PWD_H)
check_include_file_cxx("windows.h" HAVE_WINDOWS_H)
check_include_file_cxx("lmcons.h" HAVE_LMCONS_H)
check_include_file_cxx("process.h" HAVE_PROCESS_H)

# include check for members in structs
include (CheckStructHasMember)

######################### Special Requirements ##########################

# This is needed for QtSqlite and QtDesigner
# (they'll install files to ${QT_INSTALL_DIR}/plugins/)
get_filename_component(QT_BIN_DIR "${QT_MOC_EXECUTABLE}" PATH)
get_filename_component(QT_DIR     ${QT_BIN_DIR}        PATH)
set(QT_INSTALL_DIR ${QT_DIR} CACHE PATH
  "Qt install prefix defaults to the Qt prefix: ${QT_DIR}")

cmake_dependent_option(ENABLE_ADDRESSBOOK "Enable addressbook support." ON
                       "KF5IdentityManagement_FOUND;KF5Akonadi_FOUND;KF5Contacts_FOUND" OFF)

add_feature_info("Address book" ENABLE_ADDRESSBOOK "Allows fetching payee information from KDE PIM system.")

cmake_dependent_option(ENABLE_HOLIDAYS "Enable addressbook support." ON
                       "KF5Holidays_FOUND" OFF)

add_feature_info("Holidays" ENABLE_HOLIDAYS "Allows fetching holidays from KDE PIM system.")

cmake_dependent_option(ENABLE_ACTIVITIES "Enable activities support." ON
                       "KF5Activities_FOUND" OFF)

option(ENABLE_FORECASTVIEW "Enable forecast view" ON)
add_feature_info("Forecast view" ENABLE_FORECASTVIEW "Adds possibility to calculate forecasts.")
cmake_dependent_option(ENABLE_REPORTSVIEW "Enable reports view." ON
                       "KChart_FOUND" OFF)
add_feature_info("Reports view" ENABLE_REPORTSVIEW "Adds possibility to display chart and table reports.")
option(ENABLE_BUDGETVIEW "Enable budget view" ON)
add_feature_info("Budget view" ENABLE_BUDGETVIEW "Adds possibility to plan a budget.")
option(ENABLE_ONLINEJOBOUTBOXVIEW "Enable online job outbox view" ON)
add_feature_info("Online job outbox view" ENABLE_ONLINEJOBOUTBOXVIEW "Adds outbox for sending online jobs.")

cmake_dependent_option(ENABLE_SQLSTORAGE "Enable SQL storage support." ON
                       "Qt5Sql_FOUND" OFF)

add_feature_info("SQL Storage" ENABLE_SQLSTORAGE "Allows storing your financial data in SQL database.")

cmake_dependent_option(ENABLE_SQLCIPHER "Enable SQLCipher support." ON
                       "SQLCIPHER_FOUND" OFF)

# Otherwise compilers halt on something like that:
# ld: library not found for -lsqlcipher
# on MS Windows, FreeBSD, macOS, and Ubuntu 14.04 (Ubuntu has CMake 3.5.1 but I'm not sure if it's the one to blame).
if(ENABLE_SQLCIPHER AND (NOT CMAKE_SYSTEM_NAME MATCHES "Linux" OR CMAKE_VERSION VERSION_LESS 3.5.2))
  link_directories(${SQLCIPHER_LIBRARY_DIRS})
endif()

add_feature_info("SQLCipher" ENABLE_SQLCIPHER "Allows encrypting your SQLite3 database.")

cmake_dependent_option(ENABLE_IBANBICDATA "Enable IBAN/BIC data support." OFF
                       "Qt5Sql_FOUND" OFF)

add_feature_info("IBAN/BIC data" ENABLE_IBANBICDATA "Adds predefined IBAN/BIC numbers to KMyMoney (note: doesn't work yet).")

# check for optional QWebEngine
option(ENABLE_WEBENGINE "Enable QWebEngine" OFF)
if(ENABLE_WEBENGINE)
  find_package(Qt5WebEngineWidgets 5.8 REQUIRED)
  if(Qt5WebEngineWidgets_VERSION VERSION_GREATER 5.8.99 AND Qt5WebEngineWidgets_VERSION VERSION_LESS 5.9.3)
    message(WARNING "QWebEngine version ${Qt5WebEngineWidgets_VERSION} is known to be unstable with KMyMoney")
  endif()
else(ENABLE_WEBENGINE)
  find_package(KF5WebKit REQUIRED)
endif(ENABLE_WEBENGINE)

# check for optional LibOFX support
find_package(LibOfx)
cmake_dependent_option(ENABLE_OFXIMPORTER "Enable OFX Importer" ON
                       "LIBOFX_FOUND" OFF)

if(ENABLE_OFXIMPORTER)
  if(NOT LIBOFX_HAVE_CLIENTUID)
    set(PATH_TO_LIBOFX_HEADER "${LIBOFX_INCLUDE_DIR}/libofx/libofx.h")
    unset(LIBOFX_HAVE_CLIENTUID)
    unset(LIBOFX_HAVE_CLIENTUID CACHE) #not doing this will prevent updating below check
    check_struct_has_member("struct OfxFiLogin" clientuid ${PATH_TO_LIBOFX_HEADER} LIBOFX_HAVE_CLIENTUID LANGUAGE CXX)
  endif()

  if (LIBOFX_HAVE_CLIENTUID)
    set (nice_LIBOFX_HAVE_CLIENTUID "yes")
  else()
    set (nice_LIBOFX_HAVE_CLIENTUID "no")
  endif()

else()
  set (nice_LIBOFX_HAVE_CLIENTUID "unknown")
  unset(LIBOFX_HAVE_CLIENTUID)
  unset(LIBOFX_HAVE_CLIENTUID CACHE)
endif(ENABLE_OFXIMPORTER)

add_feature_info("OFX Importer" ENABLE_OFXIMPORTER "Allows importing OFX files (have client uid version: ${nice_LIBOFX_HAVE_CLIENTUID})" )

# check for optional KBanking support
find_package(AQBANKING 5.6.5)
find_package(gwenhywfar 4.15.3)
find_package(gwengui-cpp)
find_package(gwengui-qt5)

cmake_dependent_option(ENABLE_KBANKING "Enable KBanking plugin" ON
                       "AQBANKING_FOUND;gwengui-cpp_FOUND;gwengui-qt5_FOUND;Qt5QuickWidgets_FOUND" OFF)

add_feature_info(KBanking ENABLE_KBANKING "Interface for the following online banking protocols: HBCI, EBICS, OFX Direct Connect, Paypal")

# check for optional Weboob support
set(Python_ADDITIONAL_VERSIONS 2.7 2.6)
find_package(PythonInterp 2.6)
find_package(PythonLibs ${PYTHON_VERSION_STRING})

if(PYTHONINTERP_FOUND AND PYTHONLIBS_FOUND)
  if(NOT PYTHON_VERSION_MAJOR VERSION_LESS 3)
    unset(PYTHONLIBS_FOUND)
    unset(PYTHONINTERP_FOUND)
    message(WARNING "Python 2 required, but Python 3 found.")
  else()
    include(FindPythonModule)
    find_python_module(weboob REQUIRED)
  endif()
endif()

cmake_dependent_option(ENABLE_WEBOOB "Enable Weboob plugin" ON
                       "PYTHONLIBS_FOUND;PYTHONINTERP_FOUND;PY_WEBOOB;Qt5Concurrent_FOUND" OFF)
add_feature_info(Weboob ENABLE_WEBOOB "Online banking interface using Weboob.")


# check for optional ical support
find_package(Libical)
cmake_dependent_option(ENABLE_LIBICAL "Enable Calendar plugin" ON
                       "LIBICAL_FOUND" OFF)
add_feature_info(iCalendar ENABLE_LIBICAL "iCalendar integration.")


option(ENABLE_QIFIMPORTER "Enable QIF Importer" ON)
option(ENABLE_QIFEXPORTER "Enable QIF Exporter" ON)
add_feature_info("QIF Importer" ENABLE_QIFIMPORTER "Allows importing QIF files.")
add_feature_info("QIF Exporter" ENABLE_QIFEXPORTER "Allows exporting QIF files.")

option(ENABLE_GNCIMPORTER "Enable GNC Importer" ON)
add_feature_info("GNC Importer" ENABLE_GNCIMPORTER "Allows importing GNUCash files.")

option(ENABLE_CSVIMPORTER "Enable CSV Importer" ON)
option(ENABLE_CSVEXPORTER "Enable CSV Exporter" ON)
add_feature_info("CSV Importer" ENABLE_CSVIMPORTER "Allows importing CSV files.")
add_feature_info("CSV Exporter" ENABLE_CSVEXPORTER "Allows exporting CSV files.")

option(ENABLE_UNFINISHEDFEATURES "For devs only" OFF)

# TODO: this should be removed
enable_testing()

######################### Settings ##########################

# If no build type is set, use "Release with Debug Info"
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo)
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_BUILD_TYPE "${CMAKE_BUILD_TYPE}" CACHE STRING
  "Choose the type of build.
Possible values are: 'Release' 'RelWithDebInfo' 'Debug' 'DebugKMM' 'Debugfull' 'Profile'
The default value is: 'RelWithDebInfo'" FORCE)

# tells gcc to enable exception handling
include(KDECompilerSettings)
kde_enable_exceptions()

# Set linker flags

# There is no way to detect linker in cmake (see https://gitlab.kitware.com/cmake/cmake/issues/17596)
# and linkers aren't compatible with each other, so we need to assume something
if(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,-dead_strip -Wl,-undefined,error")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-dead_strip -Wl,-undefined,error -Wl,-mark_dead_strippable_dylib")
  set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS}    -Wl,-dead_strip -Wl,-undefined,error")

elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--as-needed")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--as-needed")
  # TODO: remove multiple definitions of payeeIdentifierLoader::createPayeeIdentifierFromSqlDatabase
  set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS} -Wl,--as-needed -Wl,--allow-multiple-definition")
  # CI builds are crashing on FreeBSD with --no-undefined. Probably because -DECM_ENABLE_SANITIZERS='address'
  # more can be read on the following site https://bugs.freedesktop.org/show_bug.cgi?id=100120
  if(NOT CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--no-undefined")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
    set(CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS}    -Wl,--no-undefined")
  endif()

  # TODO: remove multiple definitions of payeeIdentifierLoader::hasItemEditDelegate
  if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--allow-multiple-definition")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--allow-multiple-definition")
  endif()

elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /FORCE:Multiple")
endif()

# Set compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wlogical-op -Wno-null-dereference -Wshadow -Wunused -Wno-misleading-indentation -Wsuggest-override -Wcast-qual -Wformat=2 -fno-common")

elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wcast-qual -Wformat=2 -Wunreachable-code -fno-common")

elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /experimental:external /external:anglebrackets /external:W0 /W3")
endif()


# IDEA: Set on a per target base
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
  # DebugKMM, Debugfull, Profile
  set(CMAKE_CXX_FLAGS_DEBUGKMM
      "-g -O2 -fno-reorder-blocks -fno-schedule-insns -fno-inline")
  set(CMAKE_CXX_FLAGS_DEBUGFULL
      "-g3 -fno-inline")
  set(CMAKE_CXX_FLAGS_PROFILE
      "-g3 -fno-inline -ftest-coverage -fprofile-arcs")

  # preprocessor definitions in case this is a debug build
  set(CMAKE_CXX_FLAGS_DEBUGFULL "${CMAKE_CXX_FLAGS_DEBUGFULL} -DQT_STRICT_ITERATORS -DKMM_DEBUG -DQT_FORCE_ASSERTS")
  set(CMAKE_CXX_FLAGS_DEBUGKMM  "${CMAKE_CXX_FLAGS_DEBUGFULL} -DKMM_DEBUG -DQT_FORCE_ASSERTS")
endif()

option(USE_MODELTEST
  "Compile with ModelTest code (default=OFF)" OFF)

add_feature_info("Model test" USE_MODELTEST "Generate modeltest code (for devs only).")

option(USE_QT_DESIGNER
  "Install KMyMoney specific widget library for Qt-Designer (default=OFF)" OFF)

add_feature_info("QtDesigner" USE_QT_DESIGNER "Qt-Designer library support (for devs only).")

######################### The Actual Targets ##########################
add_subdirectory( tools )
add_subdirectory( kmymoney )
if(KF5DocTools_FOUND)
  add_subdirectory( doc )
endif()

######################### Output Results #############################

# create the config.h file out of the config.h.cmake
configure_file("config-kmymoney.h.cmake" "${CMAKE_CURRENT_BINARY_DIR}/config-kmymoney.h")
configure_file("config-kmymoney-version.h.cmake" "${CMAKE_CURRENT_BINARY_DIR}/config-kmymoney-version.h")

message("
Build type: ${CMAKE_BUILD_TYPE}")

feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND
                DESCRIPTION "The following REQUIRED packages have not been found:")

feature_summary(WHAT OPTIONAL_PACKAGES_NOT_FOUND
                DESCRIPTION "The following OPTIONAL packages have not been found:")

feature_summary(WHAT ENABLED_FEATURES
                DESCRIPTION "The following features have been enabled:")

feature_summary(WHAT DISABLED_FEATURES
                DESCRIPTION "The following features have been disabled:")
