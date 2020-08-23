
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

import QtQuick 2.6
import QtQuick.Controls 2.2
import Esri.ArcGISExtras 1.1
import Esri.GEOINTMonitor 1.0

Item {
    id: mapForm

    function exportMapImage() {
        model.exportMapImage();
    }

    function selectGraphic(graphicUid) {
        model.selectGraphic(graphicUid);
    }

    function queryGdelt() {
        model.queryGdelt();
    }

    signal mapNotification(string message);
    signal calloutDataChanged(GdeltCalloutData calloutData);

    // Create MapQuickView here, and create its Map etc. in C++ code
    MapView {
        id: focusMapView
        anchors.fill: parent
        // set focus to enable keyboard navigation
        focus: true

        // initialize Callout
        /*
        Callout {
            id: callout
            calloutData: model.lastCalloutData
            leaderPosition: leaderPositionEnum.Automatic
        }
        */

        onMouseClicked: {
            // Close popup if necessary
            if (popup.opened) {
                popup.close();
            }
        }
    }

    Popup {
        id: popup
        x: 1
        y: 1
        width: 200
        height: 300
        modal: false
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    }

    // Declare the C++ instance which creates the map etc. and supply the view
    GEOINTMonitor {
        id: model
        mapView: focusMapView

        onIdentifyCompleted: {
            // Update the popup location
            // popup is horizontal restriced on left margin
            // popup is vertical restricted
            var maxX = parent.width - 0.5 * popup.width;
            var nextX = model.lastMouseClickLocation.x - 0.5 * popup.width;
            if (maxX < nextX) {
                nextX = maxX;
            }
            popup.x = nextX;
            popup.y = model.lastMouseClickLocation.y - popup.height - 5;
            //popup.open();
        }

        onCalloutDataChanged: {
            mapForm.calloutDataChanged(model.lastCalloutData);
        }

        onMapImageExported: {
            mapForm.mapNotification(model.lastMapImageFilePath);
        }
    }
}
