import QtQuick 2.7
import QtQuick.Window 2.2
import QtWebEngine 1.3

Window {
    id: window

    property alias currentWebView: webView

    flags: Qt.Dialog
    width: 800
    height: 600
    visible: true
    onClosing: destroy()

    WebEngineView {
        id: webView

        anchors.fill: parent
        settings.pluginsEnabled: true
        settings.javascriptEnabled: true

        onWindowCloseRequested: window.close();
    }
}
