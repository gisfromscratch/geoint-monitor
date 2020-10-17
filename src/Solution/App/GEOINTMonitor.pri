HEADERS += \
    $$PWD/GdeltCalloutData.h \
    $$PWD/GdeltEventLayer.h \
    $$PWD/AppInfo.h \
    $$PWD/GEOINTMonitor.h \
    $$PWD/GraphicsFactory.h \
    $$PWD/NominatimPlaceLayer.h \
    $$PWD/SimpleGeoJsonLayer.h \
    $$PWD/WikimapiaPlaceLayer.h

SOURCES += \
    $$PWD/GdeltCalloutData.cpp \
    $$PWD/GdeltEventLayer.cpp \
    $$PWD/GraphicsFactory.cpp \
    $$PWD/NominatimPlaceLayer.cpp \
    $$PWD/SimpleGeoJsonLayer.cpp \
    $$PWD/WikimapiaPlaceLayer.cpp \
    $$PWD/main.cpp \
    $$PWD/GEOINTMonitor.cpp

RESOURCES += \
    $$PWD/qml/qml.qrc \
    $$PWD/Resources/Resources.qrc

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
