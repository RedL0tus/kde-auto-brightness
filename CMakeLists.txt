cmake_minimum_required(VERSION 3.23)
project(kde-auto-brightness)

# Use C++ 17
set(CMAKE_CXX_STANDARD 17)

# For iio-sensor-proxy support in QSensor
set(QT_MIN_VERSION "5.7.0")
set(KF5_MIN_VERSION "5.91.0")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ECM
find_package(ECM "${KF5_MIN_VERSION}" REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${ECM_MODULE_PATH}" "${ECM_KDE_MODULE_DIR}")

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(ECMMarkAsTest)
include(ECMMarkNonGuiExecutable)
include(ECMOptionalAddSubdirectory)
include(ECMQtDeclareLoggingCategory)
include(KDEClangFormat)
include(ECMDeprecationSettings)
include(KDEGitCommitHooks)
include(FeatureSummary)
include(CheckIncludeFiles)

find_package(Qt5 "${QT_MIN_VERSION}" CONFIG REQUIRED COMPONENTS
        Core
        DBus)

find_package(KF5 "${KF5_MIN_VERSION}" REQUIRED COMPONENTS
        Config
        CoreAddons
        DBusAddons
        I18n
        KCMUtils
        Package)


find_package(KDED "${KF5_MIN_VERSION}" CONFIG REQUIRED)

add_subdirectory("kded-auto-brightness")

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
