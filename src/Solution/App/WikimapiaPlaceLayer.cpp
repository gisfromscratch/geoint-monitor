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
#include "WikimapiaPlaceLayer.h"

#include "Envelope.h"
#include "FeatureCollectionTable.h"
#include "GeometryEngine.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "HeatmapRenderer.h"
#include "Polygon.h"
#include "PolygonBuilder.h"
#include "SimpleFillSymbol.h"
#include "SimpleLineSymbol.h"
#include "SimpleRenderer.h"
#include "TextSymbol.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QProcessEnvironment>
#include <QUuid>

using namespace Esri::ArcGISRuntime;

WikimapiaPlaceLayer::WikimapiaPlaceLayer(QObject *parent) :
    QObject(parent),
    m_networkAccessManager(new QNetworkAccessManager(this)),
    m_overlay(new GraphicsOverlay(this)),
    m_labelOverlay(new GraphicsOverlay(this))
{
    connect(m_networkAccessManager, &QNetworkAccessManager::finished, this, &WikimapiaPlaceLayer::networkRequestFinished);
    QProcessEnvironment systemEnvironment = QProcessEnvironment::systemEnvironment();
    QString licenseKeyName = "wikimapia.key";
    if (systemEnvironment.contains(licenseKeyName))
    {
        m_wikimapiaLicenseKey = systemEnvironment.value(licenseKeyName);
    }

    SimpleRenderer* wikimapiaRenderer = new SimpleRenderer(this);
    SimpleFillSymbol* wikimapiaFillSymbol = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, Qt::yellow, this);
    wikimapiaFillSymbol->setOutline(new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::black, 4, this));
    wikimapiaRenderer->setSymbol(wikimapiaFillSymbol);
    m_simpleRenderer = wikimapiaRenderer;
    m_overlay->setRenderer(wikimapiaRenderer);
    m_overlay->setOpacity(0.35f);

    m_labelOverlay->setMinScale(5e4);
}

void WikimapiaPlaceLayer::setSpatialFilter(const Esri::ArcGISRuntime::Envelope &extent)
{
    m_spatialFilter = extent;
}

GraphicsOverlay* WikimapiaPlaceLayer::overlay() const
{
    return m_overlay;
}

GraphicsOverlay* WikimapiaPlaceLayer::labelOverlay() const
{
    return m_labelOverlay;
}

void WikimapiaPlaceLayer::query()
{
    if (m_spatialFilter.isEmpty())
    {
        return;
    }

    QString bboxString = "lon_min=" + QString::number(m_spatialFilter.xMin())
            + "&lat_min=" + QString::number(m_spatialFilter.yMin())
            + "&lon_max=" + QString::number(m_spatialFilter.xMax())
            + "&lat_max=" + QString::number(m_spatialFilter.yMax());

    QString wikimapiaQueryString = "http://api.wikimapia.org/?key="
            + m_wikimapiaLicenseKey
            + "&function=box&coordsby=latlon&"
            + bboxString
            + "&format=json&language=en&page=1&count=50&category=&categories_or=&categories_and=";
    qDebug() << wikimapiaQueryString;
    QUrl wikimapiaQueryUrl(wikimapiaQueryString);

    QNetworkRequest wikiMapiaRequest;
    wikiMapiaRequest.setUrl(wikimapiaQueryUrl);
    m_networkAccessManager->get(wikiMapiaRequest);
}

void WikimapiaPlaceLayer::networkRequestFinished(QNetworkReply *reply)
{
    if (reply->error())
    {
        qDebug() << reply->errorString();
        return;
    }

    QByteArray jsonResponse = reply->readAll();
    QJsonDocument wikiMapiaEventsDocument = QJsonDocument::fromJson(jsonResponse);
    if (wikiMapiaEventsDocument.isNull())
    {
        qDebug() << "JSON is invalid!";
        return;
    }
    if (!wikiMapiaEventsDocument.isObject())
    {
        qDebug() << "JSON document is not an object!";
        return;
    }

    QJsonObject wikimapiaEventsObject = wikiMapiaEventsDocument.object();
    QJsonArray wikimapiaEventsArray = wikimapiaEventsObject["folder"].toArray();
    foreach (const QJsonValue& wikimapiaEvent, wikimapiaEventsArray)
    {
        if (wikimapiaEvent.isObject())
        {
            QJsonObject wikimapiaEventRecord = wikimapiaEvent.toObject();
            if (wikimapiaEventRecord.contains("polygon"))
            {
                QJsonArray polygonArray = wikimapiaEventRecord["polygon"].toArray();
                PolygonBuilder polygonBuilder(SpatialReference::wgs84());
                foreach (const QJsonValue& coordinatePair, polygonArray)
                {
                    if (coordinatePair.isObject())
                    {
                        QJsonObject coordinates = coordinatePair.toObject();
                        double x = coordinates["x"].toDouble();
                        double y = coordinates["y"].toDouble();
                        polygonBuilder.addPoint(x, y);
                    }
                }

                Polygon wikimapiaPolygon = polygonBuilder.toPolygon();
                Graphic* wikimapiaGraphic = new Graphic(wikimapiaPolygon, this);
                m_overlay->graphics()->append(wikimapiaGraphic);

                QString wikimapiaEventName = wikimapiaEventRecord["name"].toString();
                QString wikimapiaUrl = wikimapiaEventRecord["url"].toString();
                wikimapiaGraphic->attributes()->insertAttribute("name", wikimapiaEventName);
                wikimapiaGraphic->attributes()->insertAttribute("url", wikimapiaUrl);

                /*
                Point wikimapiaPoint = GeometryEngine::labelPoint(wikimapiaPolygon);
                TextSymbol* wikimapiaTextSymbol = new TextSymbol(wikimapiaEventName, Qt::black, 15, HorizontalAlignment::Center, VerticalAlignment::Middle, this);
                Graphic* wikimapiaTextGraphic = new Graphic(wikimapiaPoint, wikimapiaTextSymbol, this);
                m_labelOverlay->graphics()->append(wikimapiaTextGraphic);
                */
            }
        }
    }
}
