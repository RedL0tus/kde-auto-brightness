#ifndef KDED_AUTO_BRIGHTNESS_UTILS_H
#define KDED_AUTO_BRIGHTNESS_UTILS_H

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusVariant>
#include <QDebug>

#include <optional>

const QString PROPERTIES_INTERFACE = QStringLiteral("org.freedesktop.DBus.Properties");
const QString PROPERTIES_GET = QStringLiteral("Get");

template<typename T>
std::optional<T> extractValue(QDBusMessage msg)
{
    if ((msg.type() == QDBusMessage::ErrorMessage) || (msg.type() == QDBusMessage::InvalidMessage)) {
        return {};
    }
    auto var = msg.arguments().at(0).value<QDBusVariant>();
    return var.variant().value<T>();
}

#endif // KDED_AUTO_BRIGHTNESS_UTILS_H
