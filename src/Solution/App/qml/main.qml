
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
        RowLayout {
            anchors.fill: parent

            ToolButton {
                text: qsTr("Export map")
                onClicked: {
                    monitorForm.exportMapImage();
                }
            }

            ToolButton {
                text: qsTr("Query GDELT")
                onClicked: {
                    monitorForm.queryGdelt();
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
                // Add new list element and select it
                var listElement = {
                    uid: calloutData.uid,
                    title: calloutData.title,
                    detail: calloutData.detail,
                    url: calloutData.link
                };
                console.log(calloutData.uid);
                var lastIndex = gdeltListModel.count;
                gdeltListModel.append(listElement);
                gdeltListView.currentIndex = lastIndex;
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
                        if (index === gdeltListView.currentIndex) {
                            // Selected item clicked
                            // open the url
                            var gdeltListElement = gdeltListModel.get(index);
                            Qt.openUrlExternally(gdeltListElement.url);

                            mouse.accepted = false;
                        }
                        else {
                            gdeltListView.currentIndex = index;
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
