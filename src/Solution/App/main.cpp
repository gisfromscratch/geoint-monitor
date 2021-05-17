
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

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include "AppInfo.h"

#include "GEOINTMonitor.h"
#include "GdeltCalloutData.h"

#include "ArcGISRuntimeEnvironment.h"
#include "MapQuickView.h"

#include <QDir>
#include <QGuiApplication>
#include <QMessageBox>
#include <QProcessEnvironment>
#include <QSettings>
//#include <QSslSocket>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

//------------------------------------------------------------------------------

#define kSettingsFormat                 QSettings::IniFormat

//------------------------------------------------------------------------------

#define STRINGIZE(x) #x
#define QUOTE(x) STRINGIZE(x)

//------------------------------------------------------------------------------

using namespace Esri::ArcGISRuntime;

int main(int argc, char *argv[])
{
    //qDebug() << QSslSocket::sslLibraryBuildVersionString();
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    QCoreApplication::setApplicationName(kApplicationName);
    QCoreApplication::setApplicationVersion(kApplicationVersion);
    QCoreApplication::setOrganizationName(kOrganizationName);
#ifdef Q_OS_MAC
    QCoreApplication::setOrganizationDomain(kOrganizationName);
#else
    QCoreApplication::setOrganizationDomain(kOrganizationDomain);
#endif
    QSettings::setDefaultFormat(kSettingsFormat);

    // Before initializing ArcGIS Runtime, first set the
    // ArcGIS Runtime license setting required for your application.

    QString licenseKeyName = "ARCGIS_RUNTIME_LICENSE";
    QProcessEnvironment systemEnvironment = QProcessEnvironment::systemEnvironment();
    if (systemEnvironment.contains(licenseKeyName))
    {
        QString licenseKeyValue = systemEnvironment.value(licenseKeyName);
        ArcGISRuntimeEnvironment::setLicense(licenseKeyValue);
    }

    //  use this code to check for initialization errors
    //  QObject::connect(ArcGISRuntimeEnvironment::instance(), &ArcGISRuntimeEnvironment::errorOccurred, [](const Error& error){
    //    QMessageBox msgBox;
    //    msgBox.setText(error.message);
    //    msgBox.exec();
    //  });

    //  if (ArcGISRuntimeEnvironment::initialize() == false)
    //  {
    //    application.quit();
    //    return 1;
    //  }

    // Register the map view for QML
    qmlRegisterType<MapQuickView>("Esri.GEOINTMonitor", 1, 0, "MapView");

    // Register the GEOINTMonitor (QQuickItem) for QML
    qmlRegisterType<GEOINTMonitor>("Esri.GEOINTMonitor", 1, 0, "GEOINTMonitor");

    // Register callout data
    qmlRegisterUncreatableType<CalloutData>("Esri.GEOINTMonitor", 1, 0, "CalloutData", "CalloutData is an uncreatable type");
    qmlRegisterUncreatableType<GdeltCalloutData>("Esri.GEOINTMonitor", 1, 0, "GdeltCalloutData", "GdeltCalloutData is an uncreatable type");

    // Activate the styling
    QQuickStyle::setStyle("Material");

    // Initialize application view
    QQmlApplicationEngine engine;

    // Add the import Path
    engine.addImportPath(QDir(QCoreApplication::applicationDirPath()).filePath("qml"));
    QString arcGISRuntimeImportPath = QUOTE(ARCGIS_RUNTIME_IMPORT_PATH);
    QString arcGISToolkitImportPath = QUOTE(ARCGIS_TOOLKIT_IMPORT_PATH);

#if defined(LINUX_PLATFORM_REPLACEMENT)
    // on some linux platforms the string 'linux' is replaced with 1
    // fix the replacement paths which were created
    QString replaceString = QUOTE(LINUX_PLATFORM_REPLACEMENT);
    arcGISRuntimeImportPath = arcGISRuntimeImportPath.replace(replaceString, "linux", Qt::CaseSensitive);
    arcGISToolkitImportPath = arcGISToolkitImportPath.replace(replaceString, "linux", Qt::CaseSensitive);
#endif

    // Add the Runtime and Extras path
    engine.addImportPath(arcGISRuntimeImportPath);
    // Add the Toolkit path
    engine.addImportPath(arcGISToolkitImportPath);

    // Set the source
    engine.load(QUrl(kApplicationSourceUrl));

    return app.exec();
}

//------------------------------------------------------------------------------
