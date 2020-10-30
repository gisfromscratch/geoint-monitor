
// Copyright 2019 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

#include "DisplayMap.h"

#include "ArcGISRuntimeEnvironment.h"
#include "MapQuickView.h"

#include <QDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

//------------------------------------------------------------------------------

using namespace Esri::ArcGISRuntime;

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    // Before initializing ArcGIS Runtime, first set the
    // ArcGIS Runtime license setting required for your application.
    //ArcGISRuntimeEnvironment::setLicense("Place license string in here");

    // Register the map view for QML
    qmlRegisterType<MapQuickView>("Esri.DisplayMap", 1, 0, "MapView");

    // Register the DisplayMap (QQuickItem) for QML
    qmlRegisterType<DisplayMap>("Esri.DisplayMap", 1, 0, "DisplayMap");

    // Initialize application view
    QQmlApplicationEngine engine;

    // Add the import Path
    engine.addImportPath(QDir(QCoreApplication::applicationDirPath()).filePath("qml"));

#ifdef ARCGIS_TOOLKIT_IMPORT_PATH_2
    engine.addImportPath(ARCGIS_TOOLKIT_IMPORT_PATH_2);
#endif

    // Set the source
    engine.load(QUrl("qrc:/qml/main.qml"));

    return app.exec();
}

//------------------------------------------------------------------------------
