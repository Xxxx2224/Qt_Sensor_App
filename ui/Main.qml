import QtQuick
import QtQuick.Controls

Window {
    width: 400
    height: 600
    visible: true
    title: "Sensor Dashboard"
    color: "#f0f0f0"

    Loader {
        id: pageLoader
        anchors.fill: parent

        source: Backend.isLoggedIn ? "HomeScreen.qml" : "LoginScreen.qml"

    }
}
