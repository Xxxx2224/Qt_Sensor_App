import QtQuick

import QtQuick.Controls

import QtQuick.Layouts

Item {

    property string statusMessage: "Sunucu Durumu: Beklemede"

    property color statusColor: "gray"

    Connections {

        target: Backend

        function onLoginError(message) {

            statusMessage = "❌ " + message

            statusColor = "red"
        }

        function onLoginStatusChanged() {

            if (Backend.isLoggedIn) {

                statusMessage = "✅ Giriş Başarılı, yönlendiriliyor..."

                statusColor = "green"
            }
        }
    }

    ColumnLayout {

        anchors.centerIn: parent

        spacing: 15

        width: parent.width * 0.8

        Text {

            text: "Giriş Yap"

            font.pixelSize: 24

            font.bold: true

            Layout.alignment: Qt.AlignHCenter
        }

        TextField {

            id: usernameInput

            placeholderText: "Kullanıcı Adı"

            text: "admin"

            Layout.fillWidth: true
        }

        TextField {

            id: passwordInput

            placeholderText: "Şifre"

            text: "12345"

            echoMode: TextInput.Password

            Layout.fillWidth: true
        }

        Button {

            text: "GİRİŞ YAP"

            Layout.fillWidth: true

            highlighted: true

            onClicked: {

                statusMessage = "⏳ Sunucuya bağlanılıyor..."

                statusColor = "blue"

                Backend.login(usernameInput.text, passwordInput.text)
            }
        }

        Button {

            text: "KAYIT OL"

            Layout.fillWidth: true

            highlighted: true

            Layout.alignment: Qt.AlignHCenter

            onClicked: {

                statusColor: "blue"

                settingsRegister.open()
            }
        }

        Button {

            contentItem: Text {
                text: "Sunucu Ayarları ⚙️"
                font.bold: true
                color: "black"
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            flat: true

            Layout.alignment: Qt.AlignHCenter

            onClicked: settingsPopup.open()
        }

        Text {

            text: statusMessage

            color: statusColor

            font.pixelSize: 11

            wrapMode: Text.Wrap

            Layout.fillWidth: true

            horizontalAlignment: Text.AlignHCenter

            Layout.alignment: Qt.AlignHCenter
        }
    }

    Popup {

        id: settingsRegister

        width: 350

        height: 250

        anchors.centerIn: parent

        modal: true

        focus: true

        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        background: Rectangle {

            color: "white"

            border.color: "#ddd"

            radius: 10

            layer.enabled: true
        }

        contentItem: ColumnLayout {

            anchors.fill: parent

            anchors.margins: 20

            spacing: 15

            Text {

                text: "KAYIT OL"

                font.bold: true

                Layout.alignment: Qt.AlignHCenter
            }

            TextField {

                id: regUserInput

                Layout.fillWidth: true

                placeholderText: "Kullacici Adi"
            }

            TextField {

                id: regPassInput

                Layout.fillWidth: true

                placeholderText: "Password"
            }

            Button {

                text: "Kayıt Ol"

                Layout.fillWidth: true

                highlighted: true

                onClicked: {

                    Backend.register_user(regUserInput.text, regPassInput.text)
                }
            }
        }
    }

    Popup {

        id: settingsPopup

        width: 300

        height: 200

        anchors.centerIn: parent

        modal: true

        focus: true

        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

        background: Rectangle {

            color: "white"

            border.color: "#ddd"

            radius: 10

            layer.enabled: true
        }

        ColumnLayout {

            anchors.fill: parent

            anchors.margins: 20

            spacing: 15

            Text {

                text: "API Adresi"

                font.bold: true

                Layout.alignment: Qt.AlignHCenter
            }

            TextField {

                id: ipInput

                Layout.fillWidth: true

                placeholderText: "http://127.0.0.1:8000"

                Component.onCompleted: text = Backend.apiUrl
            }

            Button {

                text: "KAYDET"

                Layout.fillWidth: true

                highlighted: true

                onClicked: {

                    Backend.apiUrl = ipInput.text

                    statusMessage = "Ayarlar güncellendi. Yeni IP deneniyor..."

                    statusColor = "orange"

                    settingsPopup.close()
                }
            }
        }

        onOpened: ipInput.text = Backend.apiUrl
    }
}
