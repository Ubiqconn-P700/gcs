/****************************************************************************
 *
 * Copyright (C) 2018 Pinecone Inc. All rights reserved.
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/

#include <QtAndroidExtras/QtAndroidExtras>
#include <QtAndroidExtras/QAndroidJniObject>
#include "QGCApplication.h"
#include "AndroidInterface.h"
#include <QAndroidJniObject>
#include <QtAndroid>
#include "VideoManager.h"
#include "QGCToolbox.h"

extern "C" {
    JNIEXPORT void JNICALL Java_org_mavlink_qgroundcontrol_QGCActivity_nativeHandleKey(JNIEnv* env, jobject obj, jint keyCode, jint action) {
        Q_UNUSED(env);
        Q_UNUSED(obj);
        Q_UNUSED(action);
        
        qDebug() << "JNI: nativeHandleKey received keyCode:" << keyCode;
        
        // 透過 QGCApplication 取得 VideoManager
        if (qgcApp() && qgcApp()->toolbox() && qgcApp()->toolbox()->videoManager()) {
            // 由於 JNI 是在 Java 執行緒呼叫的，我們必須切換到 Qt 的主執行緒執行
            QMetaObject::invokeMethod(qgcApp()->toolbox()->videoManager(), "handleKeyAction", Q_ARG(int, keyCode));
        }
    }
}

QString AndroidInterface::getSDCardPath()
{
    QAndroidJniObject value = QAndroidJniObject::callStaticObjectMethod("org/mavlink/qgroundcontrol/QGCActivity", "getSDCardPath",
                            "()Ljava/lang/String;");
    QString sdCardPath = value.toString();

    QString readPermission("android.permission.READ_EXTERNAL_STORAGE");
    QString writePermission("android.permission.WRITE_EXTERNAL_STORAGE");

    if (QtAndroid::checkPermission(readPermission) == QtAndroid::PermissionResult::Denied ||
            QtAndroid::checkPermission(writePermission) == QtAndroid::PermissionResult::Denied) {
        QtAndroid::PermissionResultMap resultHash = QtAndroid::requestPermissionsSync(QStringList({ readPermission, writePermission }));
        if (resultHash[readPermission] == QtAndroid::PermissionResult::Denied ||
                resultHash[writePermission] == QtAndroid::PermissionResult::Denied) {
            return QString();
        }
    }

    return sdCardPath;
}
