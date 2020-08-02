HEADERS += \
    $$PWD/GdeltEventLayer.h \
    AppInfo.h \
    GEOINTMonitor.h

SOURCES += \
    $$PWD/GdeltEventLayer.cpp \
    main.cpp \
    GEOINTMonitor.cpp

RESOURCES += \
    qml/qml.qrc \
    Resources/Resources.qrc

#-------------------------------------------------------------------------------

win32 {
    include (Win/Win.pri)
}

macx {
    include (Mac/Mac.pri)
}

ios {
    include (iOS/iOS.pri)
}

android {
    include (Android/Android.pri)
}
