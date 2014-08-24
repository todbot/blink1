#include "cursorshapearea.h"

#include <QQuickItem>
#include <QCursor>

QsltCursorShapeArea::QsltCursorShapeArea(QQuickItem *parent) :
    QQuickItem(parent),
    m_currentShape(-1)
{

}

Qt::CursorShape QsltCursorShapeArea::cursorShape() const {
    return cursor().shape();
}

void QsltCursorShapeArea::setCursorShape(Qt::CursorShape cursorShape) {
    if (m_currentShape == (int) cursorShape)
        return;

    setCursor(cursorShape);
    emit cursorShapeChanged();
    m_currentShape = cursorShape;
}
