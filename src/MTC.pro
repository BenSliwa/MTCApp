QT += qml quick positioning

SOURCES += \
    main.cpp \
    appcontroller.cpp \
    uimanager.cpp \
    contextmanager.cpp

OTHER_FILES += \
    qml/main.qml \


RESOURCES += \
    main.qrc

HEADERS += \
    appcontroller.h \
    uimanager.h \
    contextmanager.h



include(app/App.pri)
include(connectivity/Connectivity.pri)
include(mobility/Mobility.pri)
include(net/Net.pri)
include(settings/Settings.pri)
include(sensor/Sensor.pri)
include(ui/UI.pri)



android {
    QT += androidextras

    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android-sources

    target.path = $$[QT_INSTALL_EXAMPLES]/androidextras/notification
    INSTALLS += target

    OTHER_FILES += \
    android-sources/AndroidManifest.xml

    DISTFILES += \
        android-sources/src/org/cni/CellularNetworkInfo/notification/NotificationClient.java
}

DISTFILES += \
    data/map.txt \
    pCAT_snr \
    pCAT_30
