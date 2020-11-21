
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

#ifndef DISPLAYMAP_H
#define DISPLAYMAP_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class Feature;
class FeatureTable;
class FeatureQueryResult;
}
}

#include <QObject>
#include <QHash>

class DisplayMap : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Esri::ArcGISRuntime::MapQuickView* mapView READ mapView WRITE setMapView NOTIFY mapViewChanged)

public:
    explicit DisplayMap(QObject* parent = nullptr);
    ~DisplayMap() override;

signals:
    void mapViewChanged();

private:
    Esri::ArcGISRuntime::MapQuickView* mapView() const;
    void setMapView(Esri::ArcGISRuntime::MapQuickView* mapView);

    void queryFeatures();
    void queryFeaturesCompleted(QUuid taskId, Esri::ArcGISRuntime::FeatureQueryResult* featureQueryResult);

    Esri::ArcGISRuntime::Map* m_map = nullptr;
    Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
    Esri::ArcGISRuntime::FeatureTable* m_featureTable = nullptr;
    QHash<qlonglong, Esri::ArcGISRuntime::Feature*> m_features;
};

#endif // DISPLAYMAP_H
