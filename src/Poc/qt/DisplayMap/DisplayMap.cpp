
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

#include "DisplayMap.h"

#include "Basemap.h"
#include "Map.h"
#include "MapQuickView.h"
#include "Portal.h"
#include "PortalItem.h"

#include <QUrl>

using namespace Esri::ArcGISRuntime;

DisplayMap::DisplayMap(QObject* parent /* = nullptr */):
    QObject(parent)
    //m_map(new Map(Basemap::topographic(this), this))
{
    auto onlinePortal = new Portal(this);
    auto portalItem = new PortalItem(onlinePortal, "27b0fc32b7954654bf9b7903ae782771");
    m_map = new Map(portalItem, this);
}

DisplayMap::~DisplayMap()
{
}

MapQuickView* DisplayMap::mapView() const
{
    return m_mapView;
}

// Set the view (created in QML)
void DisplayMap::setMapView(MapQuickView* mapView)
{
    if (!mapView || mapView == m_mapView)
    {
        return;
    }

    m_mapView = mapView;
    m_mapView->setMap(m_map);

    emit mapViewChanged();
}
