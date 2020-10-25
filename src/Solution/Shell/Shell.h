
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

#ifndef SHELL_H
#define SHELL_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapGraphicsView;
}
}

class GdeltEventLayer;



#include <QMainWindow>
#include <QUuid>

class Shell : public QMainWindow
{
    Q_OBJECT
public:
    explicit Shell(QWidget* parent = nullptr);
    ~Shell() override;

private slots:
    void exportMapImage();
    void exportMapImageCompleted(QUuid taskId, QImage image);
    void queryGdelt();

private:
    Esri::ArcGISRuntime::Map* m_map = nullptr;
    Esri::ArcGISRuntime::MapGraphicsView* m_mapView = nullptr;
    GdeltEventLayer* m_gdeltEventLayer = nullptr;
};

#endif // SHELL_H
