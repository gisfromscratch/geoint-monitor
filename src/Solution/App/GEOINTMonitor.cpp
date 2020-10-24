
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

#include "GdeltCalloutData.h"
#include "GdeltEventLayer.h"
#include "NominatimPlaceLayer.h"
#include "SimpleGeoJsonLayer.h"
#include "WikimapiaPlaceLayer.h"

#include "Basemap.h"
#include "GeometryEngine.h"
#include "Graphic.h"
#include "IdentifyGraphicsOverlayResult.h"
#include "Map.h"
#include "MapQuickView.h"
#include "Point.h"

#include <QClipboard>
#include <QDesktopServices>
#include <QDir>
#include <QGuiApplication>
#include <QRegularExpression>
#include <QStringBuilder>
#include <QUrl>

using namespace Esri::ArcGISRuntime;

GEOINTMonitor::GEOINTMonitor(QObject* parent /* = nullptr */):
    QObject(parent),
    m_map(new Map(Basemap::openStreetMap(this), this)),
    m_gdeltLayer(new GdeltEventLayer(this)),
    m_nominatimPlaceLayer(new NominatimPlaceLayer(this)),
    m_geoJsonLayer(new SimpleGeoJsonLayer(this)),
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
    connect(m_mapView, &MapQuickView::navigatingChanged, this, &GEOINTMonitor::navigatingChanged);
    //connect(m_mapView, &MapQuickView::viewpointChanged, this, &GEOINTMonitor::viewpointChanged);

    // Add the nominatim layer
    GraphicsOverlay* nominatimOverlay = m_nominatimPlaceLayer->overlay();
    m_mapView->graphicsOverlays()->append(nominatimOverlay);
    GraphicsOverlay* nominatimLabelOverlay = m_nominatimPlaceLayer->pointOverlay();
    m_mapView->graphicsOverlays()->append(nominatimLabelOverlay);

    // Add the GeoJSON layer
    GraphicsOverlay* geoJsonPointsOverlay = m_geoJsonLayer->pointsOverlay();
    m_mapView->graphicsOverlays()->append(geoJsonPointsOverlay);
    GraphicsOverlay* geoJsonLinesOverlay = m_geoJsonLayer->linesOverlay();
    m_mapView->graphicsOverlays()->append(geoJsonLinesOverlay);
    GraphicsOverlay* geoJsonAreasOverlay = m_geoJsonLayer->areasOverlay();
    m_mapView->graphicsOverlays()->append(geoJsonAreasOverlay);

    // Add the wikimapia query layer
    GraphicsOverlay* wikimapiaOverlay = m_wikimapiaPlaceLayer->overlay();
    m_mapView->graphicsOverlays()->append(wikimapiaOverlay);
    GraphicsOverlay* wikimapiaLabelOverlay = m_wikimapiaPlaceLayer->labelOverlay();
    m_mapView->graphicsOverlays()->append(wikimapiaLabelOverlay);

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

QVariantList GEOINTMonitor::lastCalloutData() const
{
    return m_lastCalloutData;
}

bool GEOINTMonitor::queryWikimapiaEnabled() const
{
    return m_queryWikimapiaEnabled;
}

void GEOINTMonitor::activateHeatmapRendering() const
{
    m_gdeltLayer->setHeatmapRendering(true);
}

void GEOINTMonitor::activateSimpleRendering() const
{
    m_gdeltLayer->setHeatmapRendering(false);
}

void GEOINTMonitor::addGeoJsonLayerFromClipboard() const
{
    QClipboard* clipboard = QGuiApplication::clipboard();
    QString clipboardText = clipboard->text();
    QUrl geoJsonUrl(clipboardText);
    m_geoJsonLayer->query(geoJsonUrl);
}

void GEOINTMonitor::clearGeoJson() const
{
    if (!removeSelectedGraphics(m_geoJsonLayer->pointsOverlay()))
    {
        m_geoJsonLayer->pointsOverlay()->graphics()->clear();
    }
    if (!removeSelectedGraphics(m_geoJsonLayer->linesOverlay()))
    {
        m_geoJsonLayer->linesOverlay()->graphics()->clear();
    }
    if (!removeSelectedGraphics(m_geoJsonLayer->areasOverlay()))
    {
        m_geoJsonLayer->areasOverlay()->graphics()->clear();
    }
}

void GEOINTMonitor::clearGdelt() const
{
    if (!removeSelectedGraphics(m_gdeltLayer->overlay()))
    {
        m_gdeltLayer->overlay()->graphics()->clear();
    }
}

void GEOINTMonitor::clearNominatim() const
{
    if (!removeSelectedGraphics(m_nominatimPlaceLayer->overlay()))
    {
        m_nominatimPlaceLayer->overlay()->graphics()->clear();
    }
    if (!removeSelectedGraphics(m_nominatimPlaceLayer->pointOverlay()))
    {
        m_nominatimPlaceLayer->pointOverlay()->graphics()->clear();
    }
}

void GEOINTMonitor::clearWikimapia() const
{
    if (!removeSelectedGraphics(m_wikimapiaPlaceLayer->overlay()))
    {
        m_wikimapiaPlaceLayer->overlay()->graphics()->clear();
    }
    if (!removeSelectedGraphics(m_wikimapiaPlaceLayer->labelOverlay()))
    {
        m_wikimapiaPlaceLayer->labelOverlay()->graphics()->clear();
    }
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

    if (m_wikimapiaPlaceLayer->overlay() == identifyResult->graphicsOverlay())
    {
        // Wikimapia overlay results
        QList<Graphic*> identifiedWikimapiaGraphics = identifyResult->graphics();
        foreach (Graphic* graphic, identifiedWikimapiaGraphics)
        {
            graphic->setSelected(true);
            QString wikimapiaUrl = graphic->attributes()->attributeValue("url").toString();
            QDesktopServices::openUrl(QUrl(wikimapiaUrl));
        }
        return;
    }

    if (m_gdeltLayer->overlay() == identifyResult->graphicsOverlay())
    {
        QRegularExpression titlePattern("title=\"(?<title>[^\"]+)\"");
        QRegularExpression hrefPattern("href=\"(?<href>[^\"]+)\"");
        QList<Graphic*> identifiedGraphics = identifyResult->graphics();
        foreach (Graphic* graphic, identifiedGraphics)
        {
            m_mapView->calloutData()->setTitle("GDELT Graphic");

            //TODO: Liftetime is bound to the underlying graphic, could be serious!
            GdeltCalloutData* calloutData = new GdeltCalloutData(graphic);

            AttributeListModel* gdeltAttributesModel = graphic->attributes();
            QStringList gdeltAttributeNames = gdeltAttributesModel->attributeNames();
            foreach (const QString& gdeltAttributeName, gdeltAttributeNames)
            {
                QVariant gdeltAttributeValue = gdeltAttributesModel->attributeValue(gdeltAttributeName);
                QString gdeltAttributeValueAsString = gdeltAttributeValue.toString();
                if (0 == gdeltAttributeName.compare("name"))
                {
                    m_mapView->calloutData()->setTitle(gdeltAttributeValueAsString);
                    calloutData->setTitle(gdeltAttributeValueAsString);
                }
                else if (0 == gdeltAttributeName.compare("html"))
                {
                    QRegularExpressionMatch titleMatch = titlePattern.match(gdeltAttributeValueAsString);
                    if (titleMatch.hasMatch())
                    {
                        QString title = titleMatch.captured("title");
                        m_mapView->calloutData()->setDetail(title);
                        calloutData->setDetail(title);
                    }
                    else
                    {
                        m_mapView->calloutData()->setDetail(gdeltAttributeValueAsString);
                        calloutData->setDetail(gdeltAttributeValueAsString);
                    }

                    QRegularExpressionMatch hrefMatch = hrefPattern.match(gdeltAttributeValueAsString);
                    if (hrefMatch.hasMatch())
                    {
                        // Set the news url
                        QString href = hrefMatch.captured("href");
                        calloutData->setLink(href);
                    }
                }
                else if (0 == gdeltAttributeName.compare("shareimage"))
                {
                    m_mapView->calloutData()->setImageUrl(QUrl(gdeltAttributeValueAsString));
                }
                else if (0 == gdeltAttributeName.compare("uid"))
                {
                    calloutData->setUniqueId(gdeltAttributeValueAsString);
                }
            }

            // Select the graphic and add the callout data
            graphic->setSelected(true);
            m_lastCalloutData.append(QVariant::fromValue(calloutData));

            // TODO: Ugly UI do not show
            //m_mapView->calloutData()->setVisible(true);
        }

        if (!identifiedGraphics.empty())
        {
            // Only when there is at least one identified graphics
            emit calloutDataChanged();
        }

        // Only for the popup listeners
        emit identifyCompleted();
        return;
    }

    // Just select the identified graphics
    identifyResult->graphicsOverlay()->selectGraphics(identifyResult->graphics());
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
    const double pixelTolerance = 3;
    const int maxResults = 50;
    bool onlyPopups = false;

    GraphicsOverlay* gdeltOverlay = m_gdeltLayer->overlay();
    gdeltOverlay->clearSelection();
    m_mapView->identifyGraphicsOverlay(gdeltOverlay, mouseEvent.x(), mouseEvent.y(), pixelTolerance, onlyPopups, maxResults);

    GraphicsOverlay* geoJsonPointsOverlay = m_geoJsonLayer->pointsOverlay();
    geoJsonPointsOverlay->clearSelection();
    m_mapView->identifyGraphicsOverlay(geoJsonPointsOverlay, mouseEvent.x(), mouseEvent.y(), pixelTolerance, onlyPopups, maxResults);
    GraphicsOverlay* geoJsonLinesOverlay = m_geoJsonLayer->linesOverlay();
    geoJsonLinesOverlay->clearSelection();
    m_mapView->identifyGraphicsOverlay(geoJsonLinesOverlay, mouseEvent.x(), mouseEvent.y(), pixelTolerance, onlyPopups, maxResults);
    GraphicsOverlay* geoJsonAreasOverlay = m_geoJsonLayer->areasOverlay();
    geoJsonAreasOverlay->clearSelection();
    m_mapView->identifyGraphicsOverlay(geoJsonAreasOverlay, mouseEvent.x(), mouseEvent.y(), pixelTolerance, onlyPopups, maxResults);

    GraphicsOverlay* wikimapiaOverlay = m_wikimapiaPlaceLayer->overlay();
    wikimapiaOverlay->clearSelection();
    m_mapView->identifyGraphicsOverlay(wikimapiaOverlay, mouseEvent.x(), mouseEvent.y(), pixelTolerance, onlyPopups, maxResults);

    // Clear the callout data
    m_lastCalloutData.clear();

    // Query wikimapia
    /*
    Point lowerLeftLocation = m_mapView->screenToLocation(m_lastMouseClickLocation.x() - pixelTolerance, m_lastMouseClickLocation.y() - pixelTolerance);
    Point upperRightLocation = m_mapView->screenToLocation(m_lastMouseClickLocation.x() + pixelTolerance, m_lastMouseClickLocation.y() + pixelTolerance);
    Envelope boundingBox(lowerLeftLocation, upperRightLocation);
    Envelope boundingBoxWgs84 = GeometryEngine::project(boundingBox, SpatialReference::wgs84()).extent();
    m_wikimapiaPlaceLayer->setSpatialFilter(boundingBoxWgs84);
    m_wikimapiaPlaceLayer->query();
    */
}

void GEOINTMonitor::queryGdelt(const QString &queryText, bool useExtent) const
{
    // Query GDELT
    if (useExtent)
    {
        Viewpoint boundingViewpoint = m_mapView->currentViewpoint(ViewpointType::BoundingGeometry);
        Envelope boundingBox = boundingViewpoint.targetGeometry();
        Envelope boundingBoxWgs84 = GeometryEngine::project(boundingBox, SpatialReference::wgs84()).extent();
        m_gdeltLayer->setSpatialFilter(boundingBoxWgs84);
    }
    else
    {
        // Set an empty envelope as no spatial filter
        m_gdeltLayer->setSpatialFilter(Envelope());
    }
    m_gdeltLayer->setQueryFilter(queryText);
    m_gdeltLayer->query();
}

void GEOINTMonitor::queryNominatim(const QString &queryText) const
{
    // Query OSM Nominatim
    m_nominatimPlaceLayer->setQueryFilter(queryText);
    m_nominatimPlaceLayer->query();
}

void GEOINTMonitor::nextPlace()
{
    m_placeIndex++;
    GraphicListModel* nominatimGraphics = m_nominatimPlaceLayer->overlay()->graphics();
    GraphicListModel* nominatimPointGraphics = m_nominatimPlaceLayer->pointOverlay()->graphics();
    if (0 == nominatimGraphics->size() && 0 == nominatimPointGraphics->size())
    {
        return;
    }

    if (nominatimGraphics->size() + nominatimPointGraphics->size() <= m_placeIndex)
    {
        m_placeIndex = 0;
    }

    if (m_placeIndex < nominatimGraphics->size())
    {
        Graphic* nominatimGraphic = nominatimGraphics->at(m_placeIndex);
        m_mapView->setViewpointGeometry(nominatimGraphic->geometry());
    }
    else if (m_placeIndex < nominatimGraphics->size() + nominatimPointGraphics->size())
    {
        Graphic* nominatimPointGraphic = nominatimPointGraphics->at(m_placeIndex - nominatimGraphics->size());
        Point nominatimLocation = static_cast<Point>(nominatimPointGraphic->geometry());
        m_mapView->setViewpointCenter(nominatimLocation);
    }
}

void GEOINTMonitor::queryWikimapia()
{
    // Query wikimapia
    Viewpoint boundingViewpoint = m_mapView->currentViewpoint(ViewpointType::BoundingGeometry);
    Envelope boundingBox = boundingViewpoint.targetGeometry();
    Envelope boundingBoxWgs84 = GeometryEngine::project(boundingBox, SpatialReference::wgs84()).extent();
    /*
    if (!m_lastQueriedBoundingBox.isEmpty())
    {
        const double wgsCoordinateTolerance = 0.01;
        if (m_lastQueriedBoundingBox.equalsWithTolerance(boundingBoxWgs84, wgsCoordinateTolerance))
        {
            // Viewpoint did not changed that much
            return;
        }
    }
    m_lastQueriedBoundingBox = boundingBoxWgs84;
    */

    m_wikimapiaPlaceLayer->setSpatialFilter(boundingBoxWgs84);
    m_wikimapiaPlaceLayer->query();
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

bool GEOINTMonitor::removeSelectedGraphics(GraphicsOverlay* overlay) const
{
    bool removedGraphic = false;
    QList<Graphic*> selectedGraphics = overlay->selectedGraphics();
    if (0 < selectedGraphics.count())
    {
        foreach(Graphic* selectedGraphic, selectedGraphics)
        {
            overlay->graphics()->removeOne(selectedGraphic);
            removedGraphic = true;
        }
    }

    return removedGraphic;
}

void GEOINTMonitor::navigatingChanged()
{
    m_navigating = !m_navigating;
    if (m_navigating)
    {
        // Started navigating
        //qDebug() << "START";
    }
    else
    {
        // Stopped navigating
        //qDebug() << "STOPP";

        const double minScale = 1e5;
        if (m_mapView->mapScale() < minScale)
        {
            if (!m_queryWikimapiaEnabled)
            {
                m_queryWikimapiaEnabled = true;
                emit wikimapiaStateChanged();
            }
        }
        else
        {
            if (m_queryWikimapiaEnabled)
            {
                m_queryWikimapiaEnabled = false;
                emit wikimapiaStateChanged();
            }
        }
    }
}

void GEOINTMonitor::viewpointChanged()
{
    qDebug() << "viewpoint changed";
}
