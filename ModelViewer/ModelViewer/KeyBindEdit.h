#pragma once
#include <QLineEdit>

class KeyBindEdit : public QLineEdit
{
    Q_OBJECT
public:
    KeyBindEdit(QString text, Qt::Key defaultKey = Qt::Key::Key_AsciiTilde, QWidget* pParent = NULL);
    ~KeyBindEdit() {}
    QString getSequence();
    void reset();
protected:
    void keyPressEvent(QKeyEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);
    QVector<Qt::Key> defalutKeys;
    QVector<Qt::Key> keySequence;
};