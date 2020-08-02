
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

    GEOINTMonitorForm {
        id: monitorForm
        anchors.fill: parent

        onMapNotification: {
            messageLabel.text = message;
            messageOn.start();
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
