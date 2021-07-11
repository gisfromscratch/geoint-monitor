
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

// Qt headers
#include <QApplication>
#include <QMessageBox>
#include <QProcessEnvironment>


#include "ArcGISRuntimeEnvironment.h"

#include "Shell.h"

using namespace Esri::ArcGISRuntime;

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication application(argc, argv);

    // Before initializing ArcGIS Runtime, first set the
    // ArcGIS Runtime license setting required for your application.

    QString licenseKeyName = "ARCGIS_RUNTIME_LICENSE";
    QProcessEnvironment systemEnvironment = QProcessEnvironment::systemEnvironment();
    if (systemEnvironment.contains(licenseKeyName))
    {
        QString licenseKeyValue = systemEnvironment.value(licenseKeyName);
        ArcGISRuntimeEnvironment::setLicense(licenseKeyValue);
    }

    Shell applicationWindow;
    applicationWindow.setMinimumWidth(800);
    applicationWindow.setMinimumHeight(600);
    applicationWindow.setWindowState(Qt::WindowFullScreen);
    applicationWindow.show();

    return application.exec();
}
