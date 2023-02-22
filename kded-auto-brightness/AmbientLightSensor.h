#ifndef KDED_AUTO_BRIGHTNESS_AMBIENT_LIGHT_SENSOR_H
#define KDED_AUTO_BRIGHTNESS_AMBIENT_LIGHT_SENSOR_H

#include <QDBusInterface>
#include <QObject>
#include <QTimer>

#include <deque>
#include <optional>
#include <utility>

#define SENSOR_PROXY_DESTINATION QStringLiteral("net.hadess.SensorProxy")
#define SENSOR_PROXY_PATH QStringLiteral("/net/hadess/SensorProxy")
#define SENSOR_PROXY_INTERFACE QStringLiteral("net.hadess.SensorProxy")
#define SENSOR_PROXY_PROPERTY_HAS_AMBIENT_LIGHT QStringLiteral("HasAmbientLight")
#define SENSOR_PROXY_PROPERTY_LIGHT_LEVEL QStringLiteral("LightLevel")
#define SENSOR_PROXY_CLAIM QStringLiteral("ClaimLight")
#define SENSOR_PROXY_RELEASE QStringLiteral("ReleaseLight")

#define DEFAULT_INTERVAL 1000

// For peak-detection algorithm
#define DEFAULT_THRESHOLD 2.0
#define DEFAULT_INFLUENCE 0.3
#define DEFAULT_LAG 10

class AmbientLightSensor : public QObject
{
    Q_OBJECT

private:
    QDBusInterface *m_control;
    QDBusInterface *m_properties;
    QTimer *m_timer;
    int m_interval;
    double m_threshold;
    double m_influence;
    int m_lag;
    bool m_started;
    std::deque<double> m_window;

    QDBusMessage getProperty(const QString &property);

    std::optional<std::pair<double, double>> stat();

public Q_SLOTS:

    void fillBuffer();

Q_SIGNALS:

    void LightLevelChanged(double lightLevel);

public:
    explicit AmbientLightSensor(QObject *parent = nullptr,
                                int interval = DEFAULT_INTERVAL,
                                double threshold = DEFAULT_THRESHOLD,
                                double influence = DEFAULT_INFLUENCE,
                                int lag = DEFAULT_LAG);

    ~AmbientLightSensor() override;

    std::optional<bool> hasSensor();

    std::optional<double> getLightLevel();

    [[nodiscard]] int getInterval() const;

    void setInterval(int interval);

    void start();

    void stop();

    bool isStarted();
};

#endif // KDED_AUTO_BRIGHTNESS_AMBIENT_LIGHT_SENSOR_H
