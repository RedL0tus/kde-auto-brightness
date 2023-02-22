add_definitions(-DPROJECT_NAME=\"${PROJECT_NAME}\")

kcoreaddons_add_plugin(kded_auto_brightness INSTALL_NAMESPACE "kf${QT_MAJOR_VERSION}/kded")

set(kded_auto_brightness_SRCS
        AutoBrightness.cpp
        AutoBrightness.h
        AmbientLightSensor.cpp
        AmbientLightSensor.h
        Utils.cpp
        Utils.h)

kconfig_add_kcfg_files(kded_auto_brightness_SRCS GENERATE_MOC config.kcfgc)

target_sources(kded_auto_brightness PRIVATE "${kded_auto_brightness_SRCS}")

target_link_libraries(kded_auto_brightness
        KF5::CoreAddons
        KF5::DBusAddons
        KF5::ConfigCore
        KF5::ConfigGui
        Qt5::Core)

set_target_properties(kded_auto_brightness PROPERTIES OUTPUT_NAME auto_brightness)
install(TARGETS kded_auto_brightness DESTINATION "${KDE_INSTALL_PLUGINDIR}/kf5/kded")
