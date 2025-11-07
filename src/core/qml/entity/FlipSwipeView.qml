import QtQuick
import QtQuick.Controls

SwipeView {
    id: root

    property int movingDirection: 0 // -1: 向左滑动, +1: 向右滑动, 0: 停止
    property int directState: 0 // 0: end, 1 moving
    property alias contentX: content.contentX

    function incrementCurrentIndex() {
        content.incrementCurrentIndex()
    }

    function decrementCurrentIndex() {
        content.decrementCurrentIndex()
    }

    function setCurrentIndex(index) {
        content.setCurrentIndex(index)
    }

    contentItem: ListView {
        id: content
        property real rotateAngle: 0

        property real _movementStartedX: 0
        property int _movementStartedIndex: -1

        property bool autoMoving: movementAnimation.running
        property int  _targetIndex: -1

        property real moveVelocityFactor: 1
        property real moveVelocity: 600

        model: root.contentModel
        interactive: root.interactive && !autoMoving
        currentIndex: root.currentIndex
        focus: root.focus

        spacing: root.spacing
        orientation: root.orientation
        snapMode: ListView.SnapOneItem
        // boundsBehavior: Flickable.StopAtBounds

        highlightRangeMode: ListView.StrictlyEnforceRange
        preferredHighlightBegin: 0
        preferredHighlightEnd: 0
        highlightMoveDuration: 250
        highlightMoveVelocity: this.moveVelocityFactor * this.moveVelocity
        maximumFlickVelocity: 4 * (root.orientation === Qt.Horizontal ? width : height)

        SmoothedAnimation {
            id: movementAnimation

            target: this
            properties: "contentX"
            velocity: content.highlightMoveVelocity
            duration: content.highlightMoveDuration
        }

        function incrementCurrentIndex()
        {
            if (this.moving)
                return

            var nextIndex = this.currentIndex + 1
            if (nextIndex >= this.count)
                return

            var targetX = 0
            var item = this.itemAtIndex(nextIndex)
            if (item === null) {
                targetX = this.currentItem.x + this.currentItem.width
            } else {
                targetX = item.x
            }

            this._beginMovement();

            if (movementAnimation.running)
                movementAnimation.stop()

            movementAnimation.to = targetX
            movementAnimation.start()
        }

        function decrementCurrentIndex()
        {
            if (this.moving)
                return

            var nextIndex = this.currentIndex - 1
            if (nextIndex < 0)
                return

            var targetX = 0
            var item = this.itemAtIndex(nextIndex)
            if (item === null) {
                targetX = this.currentItem.x - this.currentItem.width
            } else {
                targetX = item.x
            }

            if (targetX < 0)
                return;

            this._beginMovement();

            if (movementAnimation.running)
                movementAnimation.stop()

            movementAnimation.to = targetX
            movementAnimation.start()
        }

        function setCurrentIndex(index)
        {
            if (this.moving)
                return

            if (this.currentIndex === index)
                return

            if (index < 0 || index > this.count - 1)
                return

            this._targetIndex = index

            if (this._targetIndex !== -1) {
                this.moveVelocityFactor = Math.abs(this._targetIndex - this.currentIndex)
            } else {
                this.moveVelocityFactor = 1
            }

            var curIndex = this.currentIndex

            if (index > curIndex) {
                this.incrementCurrentIndex()
            } else {
                this.decrementCurrentIndex()
            }
        }

        function _beginMovement()
        {
            this._movementStartedX = this.contentX;
            this._movementStartedIndex = this.currentIndex
        }

        function _endMovement()
        {
            root.movingDirection = 0
            root.directState = 0
            this._movementStartedX = 0
            this._movementStartedIndex = -1
        }

        onMovementStarted: {
            this._beginMovement()
            this._targetIndex = -1
        }

        onContentXChanged: {
            if ((root.movingDirection === 1 && this._movementStartedX > this.contentX)
                    || (root.movingDirection === -1 && this._movementStartedX < this.contentX))
                root.directState = 0

            if ((this.currentIndex > this._movementStartedIndex && this.contentX > this.currentItem.x)
                        || (this.currentIndex < this._movementStartedIndex && this.contentX < this.currentItem.x)) {
                root.directState = 0
                this._beginMovement();
            }

            if (this.autoMoving && this.contentX === this.currentItem.x) {
                this._endMovement()
                if (this._targetIndex !== -1) {
                    if (this._targetIndex > this.currentIndex) {
                        this.incrementCurrentIndex()
                    } else if (this._targetIndex < this.currentIndex) {
                        this.decrementCurrentIndex();
                    } else {
                        this._targetIndex = -1
                    }
                }

                return
            }

            if (root.directState !== 0)
                return

            if (this.contentX > this._movementStartedX)
                root.movingDirection = 1;
            else if (this.contentX < this._movementStartedX)
                root.movingDirection = -1;

            root.directState = 1
        }

        onMovementEnded: {
            this._endMovement()
        }
    }
}
