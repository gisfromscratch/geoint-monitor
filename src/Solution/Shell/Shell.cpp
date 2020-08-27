
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

// C++ API headers
#include "Basemap.h"

// Other headers
#include "Shell.h"

#include "Map.h"
#include "MapGraphicsView.h"

#include "../App/GdeltEventLayer.h"

using namespace Esri::ArcGISRuntime;

Shell::Shell(QWidget* parent /*=nullptr*/):
    QMainWindow(parent),
    m_gdeltEventLayer(new GdeltEventLayer(this))
{

    // Create the Widget view
    m_mapView = new MapGraphicsView(this);

    // Create a map using the openStreetMap Basemap
    m_map = new Map(Basemap::openStreetMap(this), this);

    // Set map to map view
    m_mapView->setMap(m_map);

    // set the mapView as the central widget
    setCentralWidget(m_mapView);

    // Add the graphics overlay
    GraphicsOverlay* gdeltOverlay = m_gdeltEventLayer->overlay();
    m_mapView->graphicsOverlays()->append(gdeltOverlay);

    // Query the events
    m_gdeltEventLayer->setQueryFilter("climate change");
    m_gdeltEventLayer->query();
}

// destructor
Shell::~Shell()
{
}
