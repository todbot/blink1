#ifndef CURSORSHAPEAREA_H
#define CURSORSHAPEAREA_H

#include <QQuickItem>

class QsltCursorShapeArea : public QQuickItem

{
    Q_OBJECT

    Q_PROPERTY(Qt::CursorShape cursorShape READ cursorShape WRITE setCursorShape NOTIFY cursorShapeChanged)

public:
    explicit QsltCursorShapeArea(QQuickItem *parent = 0);
    Qt::CursorShape cursorShape() const;
    Q_INVOKABLE void setCursorShape(Qt::CursorShape cursorShape);

private:
    int m_currentShape;

signals:
    void cursorShapeChanged();
};

#endif // CURSORSHAPEAREA_H
