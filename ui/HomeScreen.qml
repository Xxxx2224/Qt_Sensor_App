import QtQuick
import QtQuick.Controls
import QtCharts
import QtQuick.Layouts

Item {
    id: root
    property double sicaklik: 0.0
    property double nem: 0.0
    property int zamanSayaci: 0


    Connections {
        target: Backend

        function onSensorDataReceived(t, h) {
            root.sicaklik = t
            root.nem = h


            root.zamanSayaci++
            tempSeries.append(root.zamanSayaci, t)

            if (root.zamanSayaci > 20) {
                xAxis.min = root.zamanSayaci - 20
                xAxis.max = root.zamanSayaci
            }
        }
    }

    Component.onCompleted: {

        Backend.fetchSensorData()
    }

    Timer {
        interval: 2000; running: true; repeat: true
        onTriggered: Backend.fetchSensorData()
    }

    Column {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 10

        Text {
            text: "CANLI SENSÖR GRAFİĞİ"
            font.bold: true; font.pixelSize: 20
            anchors.horizontalCenter: parent.horizontalCenter
        }

        ChartView {

            id: chart
            title: "Sıcaklık Değişimi"
            width: parent.width
            height: 300
            antialiasing: true
            theme: ChartView.ChartThemeBlueCerulean

            ValueAxis {
                id: xAxis
                min: 0; max: 20
                labelFormat: "%.0f sn"
            }

            ValueAxis {
                id: yAxis
                min: 0; max: 50
                titleText: "Sıcaklık (°C)"
            }

            LineSeries {
                id: tempSeries
                name: "Sıcaklık"
                axisX: xAxis
                axisY: yAxis
                width: 3
            }
        }

        Row {
            spacing: 20
            anchors.horizontalCenter: parent.horizontalCenter

            Rectangle {
                width: 120; height: 80; color: "#FFEBEE"; radius: 10
                Column {
                    anchors.centerIn: parent
                    Text { text: "SICAKLIK"; font.bold: true; color: "red" }
                    Text { text: root.sicaklik.toFixed(1); font.pixelSize: 24 }
                }
            }

            Rectangle {
                width: 120; height: 80; color: "#E3F2FD"; radius: 10
                Column {
                    anchors.centerIn: parent
                    Text { text: "NEM"; font.bold: true; color: "blue" }
                    Text { text: "%" + root.nem.toFixed(1); font.pixelSize: 24 }
                }
            }
        }


        // --- KONTROL PANELİ ---
        Rectangle {

            width: parent.width * 0.8
            height: 60
            color: "white"
            radius: 10
            border.color: "#ddd"
            anchors.horizontalCenter: parent.horizontalCenter

            RowLayout {
                anchors.fill: parent
                anchors.margins: 15

                Text {
                    text: "Soğutucu Fan:"
                    font.bold: true
                    font.pixelSize: 16
                }

                Item { Layout.fillWidth: true }

                Switch {

                    id: fanSwitch
                    text: checked ? "AÇIK" : "KAPALI"


                    onToggled: {
                        console.log("Fan değiştiriliyor:", checked)
                        Backend.sendCommand("FAN", checked)
                    }
                }
            }
        }
        Button {
            text: "Çıkış Yap"
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: Backend.logout()
        }
    }
}
