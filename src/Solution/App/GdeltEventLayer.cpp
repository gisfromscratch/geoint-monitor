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

#include <QJsonDocument>
#include <QNetworkReply>

using namespace Esri::ArcGISRuntime;

GdeltEventLayer::GdeltEventLayer(QObject *parent) :
    QObject(parent),
    m_networkAccessManager(new QNetworkAccessManager(this))
{
    connect(m_networkAccessManager, &QNetworkAccessManager::finished, this, &GdeltEventLayer::networkRequestFinished);
}

void GdeltEventLayer::setQueryFilter(const QString &filter)
{
    m_queryFilter = filter;
}

void GdeltEventLayer::query()
{
    // TODO: Update the base url using the query filter
    QUrl gdeltQueryUrl("https://api.gdeltproject.org/api/v2/geo/geo?query=%22climate%20change%22&format=geojson");

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
