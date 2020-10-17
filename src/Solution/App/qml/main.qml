
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

import QtQuick 2.2
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.2
import QtQuick.Layouts 1.11
import Esri.GEOINTMonitor 1.0

ApplicationWindow {
    visible: true
    width: 800
    height: 600

    Material.theme: Material.Dark
    Material.accent: "#a7ad6d"      // BW Hellgrün
    //Material.accent: "#616847"      // BW Helloliv
    Material.background: "#312d2a"  // BW Schwarz
    Material.foreground: "#d3c2a6"  // BW Beige
    Material.primary: "#434a39"     // BW Dunkelgrün

    header: ToolBar {
        ColumnLayout {
            anchors.fill: parent

            RowLayout {

                ButtonGroup {
                    buttons: [simpleButton, heatButton]
                }

                RadioButton {
                    id: simpleButton
                    checked: true
                    text: qsTr("Simple")
                    onClicked: {
                        monitorForm.activateSimpleRendering();
                    }
                }

                RadioButton {
                    id: heatButton
                    text: qsTr("Heat")
                    onClicked: {
                        monitorForm.activateHeatmapRendering();
                    }
                }

                TextField {
                    id: queryText
                    Layout.fillWidth: true
                    placeholderText: "<search for news>"
                }

                ToolButton {
                    text: qsTr("Query")
                    onClicked: {
                        gdeltListModel.clear();
                        monitorForm.queryGdelt(queryText.text, false);
                    }
                }

                ToolButton {
                    id: findEventsButton
                    text: qsTr("Find events")
                    onClicked: {
                        gdeltListModel.clear();
                        monitorForm.queryGdelt(queryText.text, true);
                    }
                }

                ToolButton {
                    text: qsTr("Clear")
                    onClicked: {
                        gdeltListModel.clear();
                        monitorForm.clearGdelt();
                    }
                }

                ToolButton {
                    text: qsTr("Add GeoJson")
                    onClicked: {
                        monitorForm.addGeoJsonLayerFromClipboard();
                    }
                }

                ToolButton {
                    text: qsTr("Clear")
                    onClicked: {
                        monitorForm.clearGeoJson();
                    }
                }
            }

            RowLayout {

                TextField {
                    id: placeText
                    Layout.fillWidth: true
                    placeholderText: "<place name>"
                }

                ToolButton {
                    text: qsTr("Add to map")
                    onClicked: {
                        monitorForm.queryNominatim(placeText.text);
                    }
                }

                ToolButton {
                    text: qsTr("Next place")
                    onClicked: {
                        monitorForm.nextPlace();
                    }
                }

                ToolButton {
                    text: qsTr("Clear")
                    onClicked: {
                        monitorForm.clearNominatim();
                    }
                }

                ToolButton {
                    id: findPlacesButton
                    enabled: false
                    text: qsTr("Find places")
                    onClicked: {
                        monitorForm.queryWikimapia();
                    }
                }

                ToolButton {
                    text: qsTr("Clear")
                    onClicked: {
                        monitorForm.clearWikimapia();
                    }
                }

                ToolButton {
                    text: qsTr("Export map")
                    onClicked: {
                        monitorForm.exportMapImage();
                    }
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent

        GEOINTMonitorForm {
            id: monitorForm

            Layout.preferredWidth: parent.width
            Layout.preferredHeight: parent.height - gdeltListView.height

            onMapNotification: {
                messageLabel.text = message;
                messageOn.start();
            }

            onCalloutDataChanged: {
                // Dictionary of { url: listIndex }
                var knownUrls = {};
                var elementCount = gdeltListModel.count;
                for (var elementIndex = 0; elementIndex < elementCount; elementIndex++) {
                    var gdeltListElement = gdeltListModel.get(elementIndex);
                    var url = gdeltListElement.url;
                    if (!(url in knownUrls)) {
                        knownUrls[url] = elementIndex;
                    }
                }

                // Add new urls as list elements
                var selectedIndex = -1;
                var dataLength = calloutData.length;
                for (var index = 0; index < dataLength; index++) {
                    var link = calloutData[index].link;
                    if (link in knownUrls) {
                        selectedIndex = knownUrls[link];
                    } else {
                        var listElement = {
                            uid: calloutData[index].uid,
                            title: calloutData[index].title,
                            detail: calloutData[index].detail,
                            url: calloutData[index].link
                        };
                        gdeltListModel.append(listElement);
                        knownUrls[link] = gdeltListModel.count - 1;
                        selectedIndex = knownUrls[link];
                    }
                }

                // Select the first new list element
                if (-1 !== selectedIndex) {
                    gdeltListView.currentIndex = selectedIndex;
                }
            }

            onWikimapiaStateChanged: {
                findPlacesButton.enabled = enabled;
            }
        }

        ListView {
            id: gdeltListView
            orientation: ListView.Horizontal
            anchors.margins: 20
            Layout.fillWidth: true

            height: 100
            highlight: Rectangle { color: "#434a39"; radius: 5 }
            focus: true

            model: ListModel {
                id: gdeltListModel
            }

            delegate: Item {
                id: listItem
                height: gdeltListView.height
                width: 200

                ColumnLayout {
                    anchors.fill: parent

                    Label {
                        id: titleLabel
                        Layout.fillWidth: true
                        horizontalAlignment: Qt.AlignHCenter
                        verticalAlignment: Qt.AlignTop
                        wrapMode: Text.Wrap
                        text: title
                    }

                    Label {
                        Layout.fillWidth: true
                        horizontalAlignment: Qt.AlignHCenter
                        verticalAlignment: Qt.AlignBottom
                        wrapMode: Text.Wrap
                        font.pointSize: titleLabel.font.pointSize - 2
                        text: detail
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    propagateComposedEvents: true

                    onEntered: {
                        if (index === gdeltListView.currentIndex) {
                            // Entered the current selected item
                            cursorShape = Qt.PointingHandCursor;
                        }
                    }

                    onExited: {
                        if (index === gdeltListView.currentIndex) {
                            // Exited the current selected item
                            cursorShape = Qt.ArrowCursor;
                        }
                    }

                    onClicked: {
                        var gdeltListElement = gdeltListModel.get(index);
                        if (index === gdeltListView.currentIndex) {
                            // Selected item clicked
                            // open the url
                            Qt.openUrlExternally(gdeltListElement.url);

                            mouse.accepted = false;
                        }
                        else {
                            // Select the clicked item
                            // navigate to graphic
                            gdeltListView.currentIndex = index;

                            monitorForm.selectGraphic(gdeltListElement.uid);
                        }
                    }
                }
            }
        }
    }

    footer: ToolBar {
        RowLayout {
            anchors.fill: parent

            Label {
                id: messageLabel
                text: "Ready"
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignRight
                verticalAlignment: Qt.AlignVCenter
                Layout.rightMargin: 5
                Layout.fillWidth: true

                opacity: 0

                OpacityAnimator on opacity {
                    id: messageOn
                    from: 0
                    to: 1
                    duration: 3000

                    onRunningChanged: {
                        if (!running) {
                            messageOff.start();
                        }
                    }
                }

                OpacityAnimator on opacity {
                    id: messageOff
                    from: 1
                    to: 0
                    duration: 3000
                }
            }
        }
    }
}
