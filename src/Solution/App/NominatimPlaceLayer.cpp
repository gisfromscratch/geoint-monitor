// GEOINT Monitor is a sample native desktop application for geospatial intelligence workflows.
// Copyright (C) 2020 Jan Tschada (gisfromscratch@live.de)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Additional permission under GNU LGPL version 3 section 4 and 5
// If you modify this Program, or any covered work, by linking or combining
// it with ArcGIS Runtime for Qt (or a modified version of that library),
// containing parts covered by the terms of ArcGIS Runtime for Qt,
// the licensors of this Program grant you additional permission to convey the resulting work.
// See <https://developers.arcgis.com/qt/> for further information.
//
#include "NominatimPlaceLayer.h"

#include "FeatureCollectionTable.h"
#include "GeometryEngine.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "HeatmapRenderer.h"
#include "Polygon.h"
#include "PolygonBuilder.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleRenderer.h"
#include "TextSymbol.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QUuid>

using namespace Esri::ArcGISRuntime;

NominatimPlaceLayer::NominatimPlaceLayer(QObject *parent) :
    QObject(parent),
    m_networkAccessManager(new QNetworkAccessManager(this)),
    m_overlay(new GraphicsOverlay(this)),
    m_pointOverlay(new GraphicsOverlay(this))
{
    connect(m_networkAccessManager, &QNetworkAccessManager::finished, this, &NominatimPlaceLayer::networkRequestFinished);

    SimpleRenderer* nominatimRenderer = new SimpleRenderer(this);
    SimpleFillSymbol* nominatimFillSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, Qt::yellow, this);
    nominatimFillSymbol->setOutline(new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::black, 4, this));
    nominatimRenderer->setSymbol(nominatimFillSymbol);
    m_overlay->setRenderer(nominatimRenderer);
    m_overlay->setOpacity(0.35f);

    SimpleRenderer* nominatimPointRenderer = new SimpleRenderer(this);
    SimpleMarkerSymbol* nominatimMarkerSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, Qt::yellow, 12, this);
    nominatimMarkerSymbol->setOutline(new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::black, 4, this));
    nominatimPointRenderer->setSymbol(nominatimMarkerSymbol);
    m_pointOverlay->setRenderer(nominatimPointRenderer);
}

GraphicsOverlay* NominatimPlaceLayer::overlay() const
{
    return m_overlay;
}

GraphicsOverlay* NominatimPlaceLayer::pointOverlay() const
{
    return m_pointOverlay;
}

void NominatimPlaceLayer::setQueryFilter(const QString &filter)
{
    m_queryFilter = filter;
}

void NominatimPlaceLayer::query()
{
    if (m_queryFilter.isEmpty())
    {
        return;
    }

    QString nominatimQueryString = "https://nominatim.openstreetmap.org/search?q="
        + m_queryFilter
        + "&format=geojson&polygon_geojson=1";
    //qDebug() << nominatimQueryString;

    QNetworkRequest nominatimRequest;
    nominatimRequest.setUrl(nominatimQueryString);
    m_networkAccessManager->get(nominatimRequest);
}

void NominatimPlaceLayer::networkRequestFinished(QNetworkReply *reply)
{
    if (reply->error())
    {
        qDebug() << reply->errorString();
        return;
    }

    QByteArray jsonResponse = reply->readAll();
    QJsonDocument nominatimPlacesDocument = QJsonDocument::fromJson(jsonResponse);
    if (nominatimPlacesDocument.isNull())
    {
        qDebug() << "JSON is invalid!";
        return;
    }
    if (!nominatimPlacesDocument.isObject())
    {
        qDebug() << "JSON document is not an object!";
        return;
    }

    QJsonObject nominatimPlacesObject = nominatimPlacesDocument.object();
    QJsonArray nominatimFeaturesArray = nominatimPlacesObject["features"].toArray();
    foreach(const QJsonValue& featureValue, nominatimFeaturesArray)
    {
        if (featureValue.isObject())
        {
            QJsonObject geojsonFeature = featureValue.toObject();
            QJsonObject geojsonFeatureGeometry = geojsonFeature["geometry"].toObject();
            QJsonValue geometryTypeValue = geojsonFeatureGeometry["type"];
            if (geometryTypeValue.isString())
            {
                QJsonValue coordinatesValue = geojsonFeatureGeometry["coordinates"];
                if (coordinatesValue.isArray())
                {
                    QJsonObject properties = geojsonFeature["properties"].toObject();
                    QVariantMap propertyMap = properties.toVariantMap();

                    QJsonArray coordinatesArray = coordinatesValue.toArray();
                    QString geometryType = geometryTypeValue.toString();
                    if (0 == QString::compare("Point", geometryType))
                    {
                        if (1 < coordinatesArray.count())
                        {
                            double x = coordinatesArray[0].toDouble();
                            double y = coordinatesArray[1].toDouble();
                            Point location(x, y, SpatialReference::wgs84());
                            Graphic* geojsonGraphic = new Graphic(location, propertyMap, this);
                            QUuid uniqueId = QUuid::createUuid();
                            geojsonGraphic->attributes()->insertAttribute("uid", uniqueId.toString());
                            m_pointOverlay->graphics()->append(geojsonGraphic);
                        }
                    }
                    else if (0 == QString::compare("Polygon", geometryType))
                    {
                        PolygonBuilder polygonBuilder(SpatialReference::wgs84());
                        foreach (const QJsonValue& ringValue, coordinatesArray)
                        {
                            if (ringValue.isArray())
                            {
                                QJsonArray ringsArray = ringValue.toArray();
                                foreach (const QJsonValue& verticesValue, ringsArray)
                                {
                                    if (verticesValue.isArray())
                                    {
                                        QJsonArray verticesArray = verticesValue.toArray();
                                        if (1 < verticesArray.count())
                                        {
                                            double x = verticesArray[0].toDouble();
                                            double y = verticesArray[1].toDouble();
                                            polygonBuilder.addPoint(x, y);
                                        }
                                    }
                                }
                            }
                        }

                        Polygon polygon = polygonBuilder.toPolygon();
                        Graphic* geojsonGraphic = new Graphic(polygon, propertyMap, this);
                        QUuid uniqueId = QUuid::createUuid();
                        geojsonGraphic->attributes()->insertAttribute("uid", uniqueId.toString());
                        m_overlay->graphics()->append(geojsonGraphic);
                    }
                }
            }
        }
    }

    emit queryFinished();
}
