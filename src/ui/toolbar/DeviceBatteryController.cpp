/****************************************************************************
 *
 * (c) 2009-2024 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include "DeviceBatteryController.h"

#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include <QAndroidJniObject>
#endif

DeviceBatteryController::DeviceBatteryController(QObject *parent)
    : QObject(parent)
    , _percentage(100)
    , _isCharging(false)
{
    _updateBatteryStatus();

    connect(&_updateTimer, &QTimer::timeout, this, &DeviceBatteryController::_updateBatteryStatus);
    _updateTimer.start(10000); // 10 seconds
}

void DeviceBatteryController::_updateBatteryStatus()
{
#ifdef Q_OS_ANDROID
    QAndroidJniObject context = QtAndroid::androidContext();
    if (!context.isValid()) return;

    QAndroidJniObject actionName = QAndroidJniObject::fromString("android.intent.action.BATTERY_CHANGED");
    QAndroidJniObject intentFilter("android/content/IntentFilter", "(Ljava/lang/String;)V", actionName.object<jstring>());

    QAndroidJniObject batteryStatus = context.callObjectMethod(
        "registerReceiver",
        "(Landroid/content/BroadcastReceiver;Landroid/content/IntentFilter;)Landroid/content/Intent;",
        nullptr,
        intentFilter.object()
    );

    if (batteryStatus.isValid()) {
        QAndroidJniObject levelKey = QAndroidJniObject::fromString("level");
        int level = batteryStatus.callMethod<jint>("getIntExtra", "(Ljava/lang/String;I)I", levelKey.object<jstring>(), -1);

        QAndroidJniObject scaleKey = QAndroidJniObject::fromString("scale");
        int scale = batteryStatus.callMethod<jint>("getIntExtra", "(Ljava/lang/String;I)I", scaleKey.object<jstring>(), 100);

        QAndroidJniObject statusKey = QAndroidJniObject::fromString("status");
        int status = batteryStatus.callMethod<jint>("getIntExtra", "(Ljava/lang/String;I)I", statusKey.object<jstring>(), -1);

        if (level != -1 && scale > 0) {
            int newPct = (int)((float)level / (float)scale * 100.0f);
            if (newPct != _percentage) {
                _percentage = newPct;
                emit percentageChanged(_percentage);
            }
        }

        // 2 = Charging, 5 = Full
        bool newCharging = (status == 2 || status == 5);
        if (newCharging != _isCharging) {
            _isCharging = newCharging;
            emit isChargingChanged(_isCharging);
        }
    }
#endif
}
