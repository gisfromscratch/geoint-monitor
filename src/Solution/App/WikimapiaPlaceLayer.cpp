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

#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QUuid>

WikimapiaPlaceLayer::WikimapiaPlaceLayer(QObject *parent) :
    QObject(parent),
    m_networkAccessManager(new QNetworkAccessManager(this))
{
    connect(m_networkAccessManager, &QNetworkAccessManager::finished, this, &WikimapiaPlaceLayer::networkRequestFinished);
}

void WikimapiaPlaceLayer::setSpatialFilter(const Esri::ArcGISRuntime::Envelope &extent)
{
    m_spatialFilter = extent;
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

    QString wikimapiaQueryString = "http://api.wikimapia.org/?key=example&function=box&coordsby=latlon& "
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

    qDebug() << jsonResponse;
}
