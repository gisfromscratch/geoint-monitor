
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

#include "Envelope.h"

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

class GdeltCalloutData;
class GdeltEventLayer;
class NominatimPlaceLayer;
class WikimapiaPlaceLayer;

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
    Q_PROPERTY(GdeltCalloutData* lastCalloutData READ lastCalloutData NOTIFY calloutDataChanged)
    Q_PROPERTY(bool queryWikimapiaEnabled READ queryWikimapiaEnabled NOTIFY wikimapiaStateChanged)

public:
    explicit GEOINTMonitor(QObject* parent = nullptr);
    ~GEOINTMonitor() override;

    Q_INVOKABLE void activateHeatmapRendering() const;
    Q_INVOKABLE void activateSimpleRendering() const;
    Q_INVOKABLE void clearGdelt() const;
    Q_INVOKABLE void clearNominatim() const;
    Q_INVOKABLE void clearWikimapia() const;
    Q_INVOKABLE void exportMapImage() const;
    Q_INVOKABLE void queryGdelt(const QString& queryText) const;
    Q_INVOKABLE void queryNominatim(const QString& queryText) const;
    Q_INVOKABLE void queryWikimapia();
    Q_INVOKABLE void selectGraphic(const QString& graphicUid) const;

signals:
    void identifyCompleted();
    void mapImageExported();
    void mapViewChanged();
    void mouseClickLocationChanged();
    void calloutDataChanged();
    void wikimapiaStateChanged();

private slots:
    void exportMapImageCompleted(QUuid taskId, QImage image);
    void identifyGraphicsOverlayCompleted(QUuid taskId, Esri::ArcGISRuntime::IdentifyGraphicsOverlayResult* identifyResult);
    void mouseClicked(QMouseEvent& mouseEvent);
    void navigatingChanged();
    void viewpointChanged();

private:    
    Esri::ArcGISRuntime::MapQuickView* mapView() const;
    void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

    QString lastMapImageFilePath() const;
    QPoint lastMouseClickLocation() const;
    GdeltCalloutData* lastCalloutData() const;

    bool queryWikimapiaEnabled() const;

    Esri::ArcGISRuntime::Map* m_map = nullptr;
    Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
    QString m_lastMapImageFilePath;
    QPoint m_lastMouseClickLocation;

    GdeltCalloutData* m_lastCalloutData = nullptr;
    GdeltEventLayer* m_gdeltLayer = nullptr;
    NominatimPlaceLayer* m_nominatimPlaceLayer = nullptr;
    WikimapiaPlaceLayer* m_wikimapiaPlaceLayer = nullptr;
    bool m_queryWikimapiaEnabled = false;
    Esri::ArcGISRuntime::Envelope m_lastQueriedBoundingBox;

    bool m_navigating = false;
};

#endif // GEOINTMONITOR_H
