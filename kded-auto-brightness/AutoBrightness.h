#ifndef KDED_AUTO_BRIGHTNESS_AUTO_BRIGHTNESS_H
#define KDED_AUTO_BRIGHTNESS_AUTO_BRIGHTNESS_H

#include <KDEDModule>
#include <KPluginFactory>
#include <QDBusInterface>
#include <QProcess>

#include <optional>

// KConfigXT
#include <config.h>

#include "AmbientLightSensor.h"

const QString POWERDEVIL_DESTINATION = QStringLiteral("local.org_kde_powerdevil");
const QString POWERDEVIL_PATH = QStringLiteral("/org/kde/Solid/PowerManagement/Actions/BrightnessControl");
const QString POWERDEVIL_INTERFACE = QStringLiteral("org.kde.Solid.PowerManagement.Actions.BrightnessControl");
const QString POWERDEVIL_GET_BRIGHTNESS = QStringLiteral("brightness");
const QString POWERDEVIL_GET_MAX_BRIGHTNESS = QStringLiteral("brightnessMax");
const QString POWERDEVIL_SET_BRIGHTNESS = QStringLiteral("setBrightness");
const QString POWERDEVIL_SET_BRIGHTNESS_SILENT = QStringLiteral("setBrightnessSilent");

// Default state
const int DEFAULT_TIMEOUT = 5000;

class Q_DECL_EXPORT AutoBrightness : public KDEDModule
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.ambient-light")

private:
    QDBusInterface *m_control;
    AmbientLightSensor *m_sensor;
    AutoBrightnessConfig::Config *m_config;

public:
    AutoBrightness(QObject *parent, const QVariantList &);
    ~AutoBrightness() override;
    void enable();
    void disable();
    std::optional<int> getBrightness();
    std::optional<int> getMaxBrightness();

public Q_SLOTS:
    void setBrightness(double lightLevel);
    void handleEnabledChanged();
    void handleIntervalChanged();
};

#endif // KDED_AUTO_BRIGHTNESS_AUTO_BRIGHTNESS_H
