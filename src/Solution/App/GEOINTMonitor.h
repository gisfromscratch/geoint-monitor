
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

#ifndef GEOINTMONITOR_H
#define GEOINTMONITOR_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
}
}

#include <QImage>
#include <QObject>
#include <QUuid>

class GEOINTMonitor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
    Q_PROPERTY(QString lastMapImageFilePath READ lastMapImageFilePath NOTIFY mapImageExported)

public:
    explicit GEOINTMonitor(QObject* parent = nullptr);
    ~GEOINTMonitor() override;

    Q_INVOKABLE void exportMapImage() const;

signals:
    void mapImageExported();
    void mapViewChanged();

private slots:
    void exportMapImageCompleted(QUuid taskId, QImage image);

private:    
    Esri::ArcGISRuntime::MapQuickView* mapView() const;
    void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

    QString lastMapImageFilePath() const;

    Esri::ArcGISRuntime::Map* m_map = nullptr;
    Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
    QString m_lastMapImageFilePath;
};

#endif // GEOINTMONITOR_H
