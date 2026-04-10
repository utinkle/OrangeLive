import QtQuick
import QtQuick.Controls

import OL.Core

ScrollView {
    id: root

    property string textSource

    TextArea {
        id: textArea

        topPadding: 10

        clip: true
        readOnly: true
        wrapMode: TextArea.NoWrap
        activeFocusOnPress: false
        background: null
        textFormat: TextEdit.RichText
    }

    FileIO {
        id: fileIO

        source: textSource
        onSourceChanged: {
            textArea.text = fileIO.read()
        }
    }
}
