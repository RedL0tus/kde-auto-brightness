#include "AmbientLightSensor.h"

#include <QDBusConnection>

#include <cmath>
// #include <sstream>
// #include <cstring>
#include <iostream>

#include "Utils.h"

QDBusMessage AmbientLightSensor::getProperty(const QString &property)
{
    return this->m_properties->call(PROPERTIES_GET, SENSOR_PROXY_INTERFACE, property);
}

std::optional<std::pair<double, double>> AmbientLightSensor::stat()
{
    auto window = &this->m_window;
    if (window->empty()) {
        return std::nullopt;
    }
    auto windowSize = (double)window->size();
    auto sum = std::reduce(window->begin(), window->end());
    auto mean = sum / windowSize;
    auto sq_sum = std::reduce(window->begin(), window->end(), 0.0, [=](double prev, double num) {
        auto diff = num - mean;
        return prev + diff * diff;
    });
    auto stddev = std::sqrt(sq_sum / windowSize);
    return std::pair(mean, stddev);
}

void AmbientLightSensor::fillBuffer()
{
    auto lightLevel = this->getLightLevel();
    if (lightLevel == std::nullopt) {
        return;
    }
    auto ll = lightLevel.value();

    //    std::stringstream window_content;
    //    for (double i: this->m_window) window_content << std::to_string(i) << ' ';
    //    QString win = QString::fromStdString(window_content.str());
    //    qDebug() << win;

    auto window = &this->m_window;
    auto windowSize = window->size();
    if (windowSize < (unsigned long)this->m_lag) {
        window->emplace_back(ll);
        //        qDebug() << "Filling up window: ll = " << ll;
        return;
    }

    window->pop_front();
    auto stats = this->stat();
    if (stats == std::nullopt) {
        return;
    }

    auto [mean, stddev] = stats.value();
    if (std::abs(ll - mean) <= (this->m_threshold * stddev)) {
        window->emplace_back(ll);
        //        qDebug() << "Light level remained the same: ll = " << ll;
        return;
    }

    auto last = window->back();
    auto next_value = (ll * this->m_influence) + ((1.0 - this->m_influence) * last);
    window->emplace_back(next_value);
    //    qDebug() << "Light level changed: last = " << last << ", next = " << next_value << ", ll = " << ll;
    Q_EMIT this->LightLevelChanged(ll);
}

AmbientLightSensor::AmbientLightSensor(QObject *parent, int interval, double threshold, double influence, int lag)
    : QObject(parent)
    , m_window()
{
    // Peak detection
    this->m_interval = interval;
    this->m_threshold = threshold;
    this->m_influence = influence;
    this->m_lag = lag;

    // DBus interfaces
    this->m_control = new QDBusInterface(SENSOR_PROXY_DESTINATION, SENSOR_PROXY_PATH, SENSOR_PROXY_INTERFACE, QDBusConnection::systemBus(), this);
    this->m_properties = new QDBusInterface(SENSOR_PROXY_DESTINATION, SENSOR_PROXY_PATH, PROPERTIES_INTERFACE, QDBusConnection::systemBus(), this);
    this->m_timer = new QTimer(this);
    this->m_started = false;
    connect(this->m_timer, &QTimer::timeout, this, &AmbientLightSensor::fillBuffer);
}

AmbientLightSensor::~AmbientLightSensor()
{
    if (this->m_started) {
        this->stop();
    }
}

std::optional<bool> AmbientLightSensor::hasSensor()
{
    return extractValue<bool>(this->getProperty(SENSOR_PROXY_PROPERTY_HAS_AMBIENT_LIGHT));
}

std::optional<double> AmbientLightSensor::getLightLevel()
{
    return extractValue<double>(this->getProperty(SENSOR_PROXY_PROPERTY_LIGHT_LEVEL));
}

int AmbientLightSensor::getInterval() const
{
    return this->m_interval;
}

void AmbientLightSensor::setInterval(int interval)
{
    if (this->m_started && (this->m_interval != interval)) {
        this->m_timer->stop();
        this->m_timer->start(interval);
    }
    this->m_interval = interval;
}

void AmbientLightSensor::start()
{
    this->m_control->call(SENSOR_PROXY_CLAIM);
    this->m_timer->start(this->m_interval);
    this->m_started = true;
}

void AmbientLightSensor::stop()
{
    if (this->m_started) {
        this->m_control->call(SENSOR_PROXY_RELEASE);
        this->m_timer->stop();
    }
    this->m_window.clear();
    this->m_started = false;
}

bool AmbientLightSensor::isStarted() const
{
    return this->m_started;
}
