#include "AutoBrightness.h"
#include <KPluginFactory>
#include <QLoggingCategory>

#include <cmath>
#include <iostream>

#include "Utils.h"

K_PLUGIN_FACTORY_WITH_JSON(AmbientLightFactory, "auto_brightness.json", registerPlugin<AutoBrightness>();)

Q_LOGGING_CATEGORY(AutoBrightness, "auto_brightness")

AutoBrightness::AutoBrightness(QObject *parent, const QVariantList &)
    : KDEDModule(parent)
{
    this->m_config = new AutoBrightnessConfig::Config(this);

    this->m_sensor = new AmbientLightSensor(this, this->m_config->interval());

    this->m_control = new QDBusInterface(POWERDEVIL_DESTINATION, POWERDEVIL_PATH, POWERDEVIL_INTERFACE, QDBusConnection::sessionBus(), this);

    // Setup slots
    connect(this->m_config, &AutoBrightnessConfig::Config::EnabledChanged, this, &AutoBrightness::handleEnabledChanged);
    connect(this->m_config, &AutoBrightnessConfig::Config::IntervalChanged, this, &AutoBrightness::handleIntervalChanged);

    this->handleEnabledChanged();
    //    this->enable();
}

AutoBrightness::~AutoBrightness()
{
    if (this->m_sensor->isStarted()) {
        this->m_sensor->stop();
    }
}

void AutoBrightness::enable()
{
    if (!this->m_sensor->hasSensor().value_or(false)) {
        return;
    }
    qDebug() << "Auto Brightness has been enabled";
    if (!this->m_sensor->isStarted()) {
        this->m_sensor->start();
    }
    connect(this->m_sensor, &AmbientLightSensor::LightLevelChanged, this, &AutoBrightness::setBrightness);
}

void AutoBrightness::disable()
{
    disconnect(this->m_sensor, nullptr, nullptr, nullptr);
    this->m_sensor->stop();
}

std::optional<int> AutoBrightness::getBrightness()
{
    auto reply = this->m_control->call(POWERDEVIL_GET_BRIGHTNESS);
    return reply.arguments().at(0).value<int>();
}

std::optional<int> AutoBrightness::getMaxBrightness()
{
    auto reply = this->m_control->call(POWERDEVIL_GET_MAX_BRIGHTNESS);
    return reply.arguments().at(0).value<int>();
}

void AutoBrightness::setBrightness(double lightLevel)
{
    qDebug() << "New light level: " << lightLevel;
    auto method = this->m_config->disablePopup() ? POWERDEVIL_SET_BRIGHTNESS_SILENT : POWERDEVIL_SET_BRIGHTNESS;
    auto maxBrightness = this->getMaxBrightness().value_or(100);
    qDebug() << "Max brightness: " << maxBrightness;
    auto targetBrightness = static_cast<int>(std::sqrt(lightLevel / this->m_config->brightnessCutoff()) * maxBrightness);
    targetBrightness = std::min(targetBrightness, maxBrightness);
    qDebug() << "Target brightness: " << targetBrightness;
    auto currentBrightness = this->getBrightness();
    if (currentBrightness.value_or(targetBrightness) != targetBrightness) {
        this->m_control->call(method, targetBrightness);
    }
}

void AutoBrightness::handleEnabledChanged()
{
    if (this->m_config->enabled()) {
        this->enable();
    } else {
        this->disable();
    }
}

void AutoBrightness::handleIntervalChanged()
{
    this->m_sensor->setInterval(this->m_config->interval());
}

#include "AutoBrightness.moc"
