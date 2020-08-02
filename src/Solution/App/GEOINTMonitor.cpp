
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

#include "GEOINTMonitor.h"

#include "Basemap.h"
#include "Map.h"
#include "MapQuickView.h"

#include <QDir>
#include <QUrl>

using namespace Esri::ArcGISRuntime;

GEOINTMonitor::GEOINTMonitor(QObject* parent /* = nullptr */):
    QObject(parent),
    m_map(new Map(Basemap::openStreetMap(this), this))
{
}

GEOINTMonitor::~GEOINTMonitor()
{
}

MapQuickView* GEOINTMonitor::mapView() const
{
    return m_mapView;
}

// Set the view (created in QML)
void GEOINTMonitor::setMapView(MapQuickView* mapView)
{
    if (!mapView || mapView == m_mapView)
    {
        return;
    }

    m_mapView = mapView;
    m_mapView->setMap(m_map);
    connect(m_mapView, &MapQuickView::exportImageCompleted, this, &GEOINTMonitor::exportMapImageCompleted);

    emit mapViewChanged();
}

QString GEOINTMonitor::lastMapImageFilePath() const
{
    return m_lastMapImageFilePath;
}

void GEOINTMonitor::exportMapImage() const
{
    if (!m_mapView)
    {
        return;
    }

    m_mapView->exportImage();
}

void GEOINTMonitor::exportMapImageCompleted(QUuid taskId, QImage image)
{
    Q_UNUSED(taskId);

    QDateTime now = QDateTime::currentDateTime();
    QString nowAsString = now.toString("yyyy-MM-dd_HH.mm.ss");
    QString fileName = "GEOINT-Monitor_" + nowAsString + ".png";
    QDir imageDir = QDir::temp();
    QString absoluteFileName = imageDir.absoluteFilePath(fileName);
    if (image.save(absoluteFileName))
    {
        // Emit map image exported
        m_lastMapImageFilePath = absoluteFileName;
        emit mapImageExported();
    }
}
