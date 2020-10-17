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
#include "SimpleGeoJsonLayer.h"

#include "GraphicsFactory.h"

#include "GraphicsOverlay.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>

using namespace Esri::ArcGISRuntime;

SimpleGeoJsonLayer::SimpleGeoJsonLayer(QObject *parent) :
    QObject(parent),
    m_networkAccessManager(new QNetworkAccessManager(this)),
    m_overlay(new GraphicsOverlay(this)),
    m_graphicsFactor(new GraphicsFactory(this))
{
    connect(m_networkAccessManager, &QNetworkAccessManager::finished, this, &SimpleGeoJsonLayer::networkRequestFinished);
}

GraphicsOverlay* SimpleGeoJsonLayer::overlay() const
{
    return m_overlay;
}

void SimpleGeoJsonLayer::query(const QUrl &geoJsonUrl)
{
    QNetworkRequest geoJsonRequest(geoJsonUrl);
    m_networkAccessManager->get(geoJsonRequest);
}

void SimpleGeoJsonLayer::networkRequestFinished(QNetworkReply *reply)
{
    if (reply->error())
    {
        qDebug() << reply->errorString();
        return;
    }

    QByteArray jsonResponse = reply->readAll();
    QJsonDocument geoJsonDocument = QJsonDocument::fromJson(jsonResponse);
    if (geoJsonDocument.isNull())
    {
        qDebug() << "JSON is invalid!";
        return;
    }
    if (!geoJsonDocument.isObject())
    {
        qDebug() << "JSON document is not an object!";
        return;
    }

    QJsonObject geoJsonObject = geoJsonDocument.object();
    QJsonArray geoJsonFeaturesArray = geoJsonObject["features"].toArray();
    if (!m_graphicsFactor->createGraphics(geoJsonFeaturesArray, m_overlay))
    {
        qDebug() << "No GeoJSON feature was added!";
    }
}
