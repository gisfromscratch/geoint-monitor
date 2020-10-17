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
#ifndef SIMPLEGEOJSONLAYER_H
#define SIMPLEGEOJSONLAYER_H

class GraphicsFactory;

namespace Esri
{
namespace ArcGISRuntime
{
class FeatureCollectionTable;
class GraphicsOverlay;
class Graphic;
class Renderer;
}
}

class QNetworkReply;

#include <QNetworkAccessManager>

#include <QObject>

class SimpleGeoJsonLayer : public QObject
{
    Q_OBJECT
public:
    explicit SimpleGeoJsonLayer(QObject *parent = nullptr);

    Esri::ArcGISRuntime::GraphicsOverlay* pointsOverlay() const;
    Esri::ArcGISRuntime::GraphicsOverlay* linesOverlay() const;
    Esri::ArcGISRuntime::GraphicsOverlay* areasOverlay() const;

    void query(const QUrl& geoJsonUrl);

signals:

private slots:
    void networkRequestFinished(QNetworkReply* reply);

private:
    QNetworkAccessManager* m_networkAccessManager = nullptr;
    Esri::ArcGISRuntime::GraphicsOverlay* m_pointsOverlay = nullptr;
    Esri::ArcGISRuntime::GraphicsOverlay* m_linesOverlay = nullptr;
    Esri::ArcGISRuntime::GraphicsOverlay* m_areasOverlay = nullptr;
    GraphicsFactory* m_graphicsFactor = nullptr;
};

#endif // SIMPLEGEOJSONLAYER_H
