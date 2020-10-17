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
#include "GraphicsFactory.h"

#include "GeometryEngine.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "PolygonBuilder.h"

#include <QJsonObject>

using namespace Esri::ArcGISRuntime;

GraphicsFactory::GraphicsFactory(QObject *parent) : QObject(parent)
{

}

bool GraphicsFactory::createGraphics(const QJsonArray &featuresArray,
                                     Esri::ArcGISRuntime::GraphicsOverlay *pointsOverlay,
                                     Esri::ArcGISRuntime::GraphicsOverlay *linesOverlay,
                                     Esri::ArcGISRuntime::GraphicsOverlay *areasOverlay)
{
    bool added = false;
    foreach (const QJsonValue& featureValue, featuresArray)
    {
        if (featureValue.isObject())
        {
            QJsonObject geojsonFeature = featureValue.toObject();
            QJsonObject geojsonGeometry = geojsonFeature["geometry"].toObject();
            QJsonValue geometryTypeValue = geojsonGeometry["type"];
            if (geometryTypeValue.isString())
            {
                QJsonValue coordinatesValue = geojsonGeometry["coordinates"];
                if (coordinatesValue.isArray())
                {
                    QJsonObject properties = geojsonFeature["properties"].toObject();
                    QVariantMap propertyMap = properties.toVariantMap();

                    QJsonArray coordinatesArray = coordinatesValue.toArray();
                    QString geometryType = geometryTypeValue.toString();
                    if (0 == QString::compare("Point", geometryType))
                    {
                        QJsonArray coordinatesArray = coordinatesValue.toArray();
                        if (1 < coordinatesArray.count())
                        {
                            double x = coordinatesArray[0].toDouble();
                            double y = coordinatesArray[1].toDouble();
                            Point location(x, y, SpatialReference::wgs84());
                            Graphic* graphic = new Graphic(location, propertyMap, this);
                            pointsOverlay->graphics()->append(graphic);
                            added = true;
                        }
                    }
                    // TODO: MultiPoint, Polyline and so on implementations
                    else if (0 == QString::compare("Polygon", geometryType))
                    {
                        Polygon polygon = createPolygon(coordinatesArray);
                        Graphic* geojsonGraphic = new Graphic(polygon, propertyMap, this);
                        areasOverlay->graphics()->append(geojsonGraphic);
                        added = true;
                    }
                    else if (0 == QString::compare("MultiPolygon", geometryType))
                    {
                        // Coordinates represents arrays of polygons
                        foreach (const QJsonValue& polygonValue, coordinatesArray)
                        {
                            if (polygonValue.isArray())
                            {
                                QJsonArray polygonCoordinatesArray = polygonValue.toArray();
                                Polygon polygon = createPolygon(polygonCoordinatesArray);
                                Graphic* geojsonGraphic = new Graphic(polygon, propertyMap, this);
                                areasOverlay->graphics()->append(geojsonGraphic);
                                added = true;
                            }
                        }
                    }
                }
            }
        }
    }

    return added;
}

Polygon GraphicsFactory::createPolygon(const QJsonArray &coordinatesArray)
{
    PolygonBuilder polygonBuilder(SpatialReference::wgs84());
    bool interiorRing = false;
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

            if (!interiorRing)
            {
                // The first ring must be the exterior ring
                interiorRing = true;

                // TODO: Implement polygon holes
                if (1 < coordinatesArray.count())
                {
                    qDebug() << (coordinatesArray.count() - 1) << " interior rings are thrown away!";
                }
                break;
            }
        }
    }

    return polygonBuilder.toPolygon();
}
