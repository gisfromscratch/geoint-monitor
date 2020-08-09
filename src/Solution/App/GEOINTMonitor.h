
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
class CalloutData;
class IdentifyGraphicsOverlayResult;
class Map;
class MapQuickView;
}
}

class GdeltEventLayer;

#include <QImage>
#include <QObject>
#include <QMouseEvent>
#include <QPoint>
#include <QUuid>

class GEOINTMonitor : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)
    Q_PROPERTY(QString lastMapImageFilePath READ lastMapImageFilePath NOTIFY mapImageExported)
    Q_PROPERTY(QPoint lastMouseClickLocation READ lastMouseClickLocation NOTIFY mouseClickLocationChanged)
    Q_PROPERTY(Esri::ArcGISRuntime::CalloutData* lastCalloutData READ lastCalloutData NOTIFY calloutDataChanged)

public:
    explicit GEOINTMonitor(QObject* parent = nullptr);
    ~GEOINTMonitor() override;

    Q_INVOKABLE void exportMapImage() const;
    Q_INVOKABLE void queryGdelt() const;

signals:
    void identifyCompleted();
    void mapImageExported();
    void mapViewChanged();
    void mouseClickLocationChanged();
    void calloutDataChanged();

private slots:
    void exportMapImageCompleted(QUuid taskId, QImage image);
    void identifyGraphicsOverlayCompleted(QUuid taskId, Esri::ArcGISRuntime::IdentifyGraphicsOverlayResult* identifyResult);
    void mouseClicked(QMouseEvent& mouseEvent);

private:    
    Esri::ArcGISRuntime::MapQuickView* mapView() const;
    void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

    QString lastMapImageFilePath() const;
    QPoint lastMouseClickLocation() const;
    Esri::ArcGISRuntime::CalloutData* lastCalloutData() const;

    Esri::ArcGISRuntime::Map* m_map = nullptr;
    Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
    QString m_lastMapImageFilePath;
    QPoint m_lastMouseClickLocation;

    GdeltEventLayer* m_gdeltLayer = nullptr;
};

#endif // GEOINTMONITOR_H
