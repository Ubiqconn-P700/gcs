/****************************************************************************
 *
 * (c) 2009-2024 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#ifndef DEVICE_BATTERY_CONTROLLER_H
#define DEVICE_BATTERY_CONTROLLER_H

#include <QObject>
#include <QTimer>

class DeviceBatteryController : public QObject
{
    Q_OBJECT

public:
    explicit DeviceBatteryController(QObject *parent = nullptr);

    Q_PROPERTY(int percentage READ percentage NOTIFY percentageChanged)
    Q_PROPERTY(bool isCharging READ isCharging NOTIFY isChargingChanged)

    int  percentage() const { return _percentage; }
    bool isCharging() const { return _isCharging; }

signals:
    void percentageChanged(int percentage);
    void isChargingChanged(bool isCharging);

private slots:
    void _updateBatteryStatus();

private:
    int     _percentage;
    bool    _isCharging;
    QTimer  _updateTimer;
};

#endif // DEVICE_BATTERY_CONTROLLER_H
