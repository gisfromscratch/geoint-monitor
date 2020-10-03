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
#include "GdeltEventLayer.h"

#include "FeatureCollectionTable.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "HeatmapRenderer.h"
#include "SimpleMarkerSymbol.h"
#include "SimpleRenderer.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QUuid>

using namespace Esri::ArcGISRuntime;

GdeltEventLayer::GdeltEventLayer(QObject *parent) :
    QObject(parent),
    m_networkAccessManager(new QNetworkAccessManager(this)),
    m_overlay(new GraphicsOverlay(this))
{
    connect(m_networkAccessManager, &QNetworkAccessManager::finished, this, &GdeltEventLayer::networkRequestFinished);

    SimpleRenderer* gdeltRenderer = new SimpleRenderer(this);
    SimpleMarkerSymbol* gdeltSymbol = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, Qt::gray, 12, this);
    gdeltSymbol->setOutline(new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, Qt::black, 4, this));
    gdeltRenderer->setSymbol(gdeltSymbol);
    m_simpleRenderer = gdeltRenderer;
    m_overlay->setRenderer(gdeltRenderer);

    QJsonObject rendererJson;
    rendererJson.insert("type", "heatmap");
    rendererJson.insert("blurRadius", 10);

    QJsonObject firstColorStop;
    firstColorStop.insert("ratio", 0);
    QJsonArray firstColorArray;
    firstColorArray.push_back(133);
    firstColorArray.push_back(193);
    firstColorArray.push_back(200);
    firstColorArray.push_back(0);
    firstColorStop.insert("colorStops", firstColorArray);

    QJsonObject secondColorStop;
    secondColorStop.insert("ratio", 0.01);
    QJsonArray secondColorArray;
    secondColorArray.push_back(133);
    secondColorArray.push_back(193);
    secondColorArray.push_back(200);
    secondColorArray.push_back(0);
    secondColorStop.insert("colorStops", secondColorArray);

    QJsonArray colorStopsArray;
    colorStopsArray.push_back(firstColorStop);
    colorStopsArray.push_back(secondColorStop);

    rendererJson.insert("maxPixelIntensity", 256);
    rendererJson.insert("minPixelIntensity", 0);

    QJsonDocument rendererDocument(rendererJson);
    QString rendererAsJson = rendererDocument.toJson();
    HeatmapRenderer* gdeltHeatmapRenderer = dynamic_cast<HeatmapRenderer*>(Renderer::fromJson(rendererAsJson));
    m_heatMapRenderer = gdeltHeatmapRenderer;

    m_overlay->setPopupEnabled(true);
}

void GdeltEventLayer::setHeatmapRendering(bool enabled)
{
    if (enabled)
    {
        m_overlay->setRenderer(m_heatMapRenderer);
    }
    else
    {
        m_overlay->setRenderer(m_simpleRenderer);
    }
}

void GdeltEventLayer::setQueryFilter(const QString &filter)
{
    m_queryFilter = filter;
}

GraphicsOverlay* GdeltEventLayer::overlay() const
{
    return m_overlay;
}

Graphic* GdeltEventLayer::findGraphic(const QString &graphicUid) const
{
    GraphicListModel* graphics = m_overlay->graphics();
    int graphicCount = graphics->size();
    for (int graphicIndex = 0; graphicIndex < graphicCount; graphicIndex++)
    {
        Graphic* graphic = graphics->at(graphicIndex);
        QVariant uniqueId = graphic->attributes()->attributeValue("uid");
        if (!uniqueId.isNull() && 0 == uniqueId.toString().compare(graphicUid))
        {
            return graphic;
        }
    }

    return nullptr;
}

void GdeltEventLayer::query()
{
    QString gdeltQueryString = "https://api.gdeltproject.org/api/v2/geo/geo?query=" + m_queryFilter + "&format=geojson";
    QUrl gdeltQueryUrl(gdeltQueryString);

    QNetworkRequest gdeltRequest;
    gdeltRequest.setUrl(gdeltQueryUrl);
    m_networkAccessManager->get(gdeltRequest);
}

void GdeltEventLayer::networkRequestFinished(QNetworkReply* reply)
{
    if (reply->error())
    {
        qDebug() << reply->errorString();
        return;
    }

    QByteArray jsonResponse = reply->readAll();
    QJsonDocument gdeltEventsDocument = QJsonDocument::fromJson(jsonResponse);
    if (gdeltEventsDocument.isNull())
    {
        qDebug() << "JSON is invalid!";
        return;
    }
    if (!gdeltEventsDocument.isObject())
    {
        qDebug() << "JSON document is not an object!";
        return;
    }

    QJsonObject gdeltEventsObject = gdeltEventsDocument.object();
    QJsonArray gdeltFeaturesArray = gdeltEventsObject["features"].toArray();
    foreach (const QJsonValue& featureValue, gdeltFeaturesArray)
    {
        if (featureValue.isObject())
        {
            QJsonObject gdeltFeature = featureValue.toObject();
            QJsonObject gdeltFeatureGeometry = gdeltFeature["geometry"].toObject();
            QJsonValue geometryTypeValue = gdeltFeatureGeometry["type"];
            if (geometryTypeValue.isString())
            {
                QString geometryType = geometryTypeValue.toString();
                if (0 == QString::compare("Point", geometryType))
                {
                    QJsonValue coordinatesValue = gdeltFeatureGeometry["coordinates"];
                    if (coordinatesValue.isArray())
                    {
                        QJsonArray coordinatesArray = coordinatesValue.toArray();
                        if (1 < coordinatesArray.count())
                        {
                            double x = coordinatesArray[0].toDouble();
                            double y = coordinatesArray[1].toDouble();
                            QJsonObject properties = gdeltFeature["properties"].toObject();
                            QVariantMap propertyMap = properties.toVariantMap();

                            Point location(x, y, SpatialReference::wgs84());
                            Graphic* gdeltGraphic = new Graphic(location, propertyMap, this);
                            QUuid uniqueId = QUuid::createUuid();
                            gdeltGraphic->attributes()->insertAttribute("uid", uniqueId.toString());
                            m_overlay->graphics()->append(gdeltGraphic);
                        }
                    }
                }
            }
        }
    }
}

FeatureCollectionTable* GdeltEventLayer::createTable()
{
    QList<Field> gdeltFields;
    Domain emptyDomain;
    Field nameField(FieldType::Text, "name", "name", 1024, emptyDomain, true, true);
    gdeltFields.append(nameField);
    Field countField(FieldType::Int32, "count", "count", 0, emptyDomain, true, true);
    gdeltFields.append(countField);
    FeatureCollectionTable* newTable = new FeatureCollectionTable(gdeltFields, GeometryType::Point, SpatialReference::wgs84(), this);
    return newTable;
}
