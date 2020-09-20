
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
#include "GeometryEngine.h"
#include "Graphic.h"
#include "IdentifyGraphicsOverlayResult.h"
#include "Map.h"
#include "MapQuickView.h"
#include "Point.h"

#include "GdeltCalloutData.h"
#include "GdeltEventLayer.h"
#include "WikimapiaPlaceLayer.h"

#include <QDir>
#include <QRegularExpression>
#include <QStringBuilder>
#include <QUrl>

using namespace Esri::ArcGISRuntime;

GEOINTMonitor::GEOINTMonitor(QObject* parent /* = nullptr */):
    QObject(parent),
    m_map(new Map(Basemap::openStreetMap(this), this)),
    m_lastCalloutData(new GdeltCalloutData(this)),
    m_gdeltLayer(new GdeltEventLayer(this)),
    m_wikimapiaPlaceLayer(new WikimapiaPlaceLayer(this))
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
    connect(m_mapView, &MapQuickView::mouseClicked, this, &GEOINTMonitor::mouseClicked);
    connect(m_mapView, &MapQuickView::identifyGraphicsOverlayCompleted, this, &GEOINTMonitor::identifyGraphicsOverlayCompleted);

    // Add the GDELT query layer
    GraphicsOverlay* gdeltOverlay = m_gdeltLayer->overlay();
    m_mapView->graphicsOverlays()->append(gdeltOverlay);

    emit mapViewChanged();
}

QString GEOINTMonitor::lastMapImageFilePath() const
{
    return m_lastMapImageFilePath;
}

QPoint GEOINTMonitor::lastMouseClickLocation() const
{
    return m_lastMouseClickLocation;
}

GdeltCalloutData* GEOINTMonitor::lastCalloutData() const
{
    return m_lastCalloutData;
}

void GEOINTMonitor::activateHeatmapRendering() const
{
    m_gdeltLayer->setHeatmapRendering(true);
}

void GEOINTMonitor::activateSimpleRendering() const
{
    m_gdeltLayer->setHeatmapRendering(false);
}

void GEOINTMonitor::exportMapImage() const
{
    if (!m_mapView)
    {
        return;
    }

    m_mapView->exportImage();
}

void GEOINTMonitor::identifyGraphicsOverlayCompleted(QUuid taskId, Esri::ArcGISRuntime::IdentifyGraphicsOverlayResult* identifyResult)
{
    Q_UNUSED(taskId);
    if (!identifyResult->error().isEmpty())
    {
        return;
    }

    QRegularExpression titlePattern("title=\"(?<title>[^\"]+)\"");
    QRegularExpression hrefPattern("href=\"(?<href>[^\"]+)\"");
    QList<Graphic*> identifiedGraphics = identifyResult->graphics();
    foreach (const Graphic* graphic, identifiedGraphics)
    {
        m_mapView->calloutData()->setTitle("GDELT Graphic");

        AttributeListModel* gdeltAttributesModel = graphic->attributes();
        QStringList gdeltAttributeNames = gdeltAttributesModel->attributeNames();
        foreach (const QString& gdeltAttributeName, gdeltAttributeNames)
        {
            QVariant gdeltAttributeValue = gdeltAttributesModel->attributeValue(gdeltAttributeName);
            QString gdeltAttributeValueAsString = gdeltAttributeValue.toString();
            if (0 == gdeltAttributeName.compare("name"))
            {
                m_mapView->calloutData()->setTitle(gdeltAttributeValueAsString);
                m_lastCalloutData->setTitle(gdeltAttributeValueAsString);
            }
            else if (0 == gdeltAttributeName.compare("html"))
            {
                QRegularExpressionMatch titleMatch = titlePattern.match(gdeltAttributeValueAsString);
                if (titleMatch.hasMatch())
                {
                    QString title = titleMatch.captured("title");
                    m_mapView->calloutData()->setDetail(title);
                    m_lastCalloutData->setDetail(title);
                }
                else
                {
                    m_mapView->calloutData()->setDetail(gdeltAttributeValueAsString);
                    m_lastCalloutData->setDetail(gdeltAttributeValueAsString);
                }

                QRegularExpressionMatch hrefMatch = hrefPattern.match(gdeltAttributeValueAsString);
                if (hrefMatch.hasMatch())
                {
                    // Set the news url
                    QString href = hrefMatch.captured("href");
                    m_lastCalloutData->setLink(href);
                }
            }
            else if (0 == gdeltAttributeName.compare("shareimage"))
            {
                m_mapView->calloutData()->setImageUrl(QUrl(gdeltAttributeValueAsString));
            }
            else if (0 == gdeltAttributeName.compare("uid"))
            {
                m_lastCalloutData->setUniqueId(gdeltAttributeValueAsString);
            }
        }

        // TODO: Ugly UI do not show
        //m_mapView->calloutData()->setVisible(true);
    }

    if (!identifiedGraphics.empty())
    {
        // Select last identified graphic
        Graphic* gdeltGraphic = m_gdeltLayer->findGraphic(m_lastCalloutData->uniqueId());
        if (nullptr != gdeltGraphic)
        {
            gdeltGraphic->setSelected(true);
        }

        // Only when there is at least one identified graphics
        emit calloutDataChanged();
    }
    else
    {
        // No graphic identified
        Viewpoint boundingViewpoint = m_mapView->currentViewpoint(ViewpointType::BoundingGeometry);
        Envelope boundingBox = boundingViewpoint.targetGeometry();
        Envelope boundingBoxWgs84 = GeometryEngine::project(boundingBox, SpatialReference::wgs84()).extent();
        m_wikimapiaPlaceLayer->setSpatialFilter(boundingBoxWgs84);
        m_wikimapiaPlaceLayer->query();
    }

    emit identifyCompleted();
}

void GEOINTMonitor::mouseClicked(QMouseEvent& mouseEvent)
{
    if (!m_mapView)
    {
        return;
    }

    // Update the last received mouse event
    m_lastMouseClickLocation = mouseEvent.pos();
    emit mouseClickLocationChanged();

    // Update the last received map location
    Point mapClickLocation = m_mapView->screenToLocation(m_lastMouseClickLocation.x(), m_lastMouseClickLocation.y());

    // Update the callout data
    if (m_mapView->calloutData()->isVisible())
    {
        m_mapView->calloutData()->setVisible(false);
    }
    m_mapView->calloutData()->setLocation(mapClickLocation);

    // Identify the graphics
    const double pixelTolerance = 12;
    bool onlyPopups = false;
    GraphicsOverlay* gdeltOverlay = m_gdeltLayer->overlay();
    gdeltOverlay->clearSelection();
    m_mapView->identifyGraphicsOverlay(gdeltOverlay, mouseEvent.x(), mouseEvent.y(), pixelTolerance, onlyPopups);
}

void GEOINTMonitor::queryGdelt(const QString &queryText) const
{
    m_gdeltLayer->setQueryFilter(queryText);
    m_gdeltLayer->query();
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

void GEOINTMonitor::selectGraphic(const QString &graphicUid) const
{
    Graphic* gdeltGraphic = m_gdeltLayer->findGraphic(graphicUid);
    if (nullptr == gdeltGraphic)
    {
        return;
    }

    // Select and pan
    m_gdeltLayer->overlay()->clearSelection();
    gdeltGraphic->setSelected(true);

    Geometry gdeltGeometry = gdeltGraphic->geometry();
    switch (gdeltGeometry.geometryType())
    {
    case GeometryType::Point:
        {
            Point gdeltLocation = (Point)gdeltGeometry;
            m_mapView->setViewpointCenter(gdeltLocation);
        }
        break;

    default:
        return;
    }
}
