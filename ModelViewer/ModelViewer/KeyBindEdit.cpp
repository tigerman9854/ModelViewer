#include "KeyBindEdit.h"
#include "KeySequenceParse.h"

#include <QKeyEvent>
#include <QSettings>

KeyBindEdit::KeyBindEdit(QString text, Qt::Key defaultKey, QWidget* pParent) :QLineEdit(pParent)
{
    QSettings settings;
    if (defaultKey != Qt::Key::Key_AsciiTilde)
        defalutKeys.append(defaultKey);

    // Check to see if we already have this keybind
    if (settings.value(text, NULL) != NULL) {
        // If we do parse it and set
        keySequence = KeySequenceParse(text).getVec();    
    }
    else {
        keySequence = defalutKeys;
    }

    setText(getSequence());
}

QString KeyBindEdit::getSequence()
{
    QString finalString = "";
    if (keySequence.length() > 0) {
        for (int i = 0; i < keySequence.length(); i++)
        {
            if (i > 0)
                finalString.append("+");
            if (keySequence[i] == Qt::Key::Key_Shift)
                finalString.append("Shift");
            else if (keySequence[i] == Qt::Key::Key_Control)
                finalString.append("Ctrl");
            else if (keySequence[i] == Qt::Key::Key_Alt)
                finalString.append("Alt");
            else if (keySequence[i] == Qt::Key::Key_Up)
                finalString.append("Up");
            else if (keySequence[i] == Qt::Key::Key_Down)
                finalString.append("Down");
            else if (keySequence[i] == Qt::Key::Key_Right)
                finalString.append("Right");
            else if (keySequence[i] == Qt::Key::Key_Left)
                finalString.append("Left");
            else
                finalString.append(keySequence[i]);
        }
    }
    
    if (finalString == "")
        return QString::fromLatin1("Unset");
    else
        return finalString;
}

void KeyBindEdit::reset()
{
    keySequence = defalutKeys;
    setText(getSequence());
}

void KeyBindEdit::keyPressEvent(QKeyEvent* keyEvent)
{
    int keyInt = keyEvent->key();
    Qt::Key key = static_cast<Qt::Key>(keyInt);

    // Checking for key combinations
    Qt::KeyboardModifiers modifiers = keyEvent->modifiers();


    if (modifiers.testFlag(Qt::ShiftModifier) && key != Qt::Key::Key_Shift)
            keySequence.append(Qt::Key::Key_Shift);
    else if (modifiers.testFlag(Qt::ControlModifier) && key != Qt::Key::Key_Control)
            keySequence.append(Qt::Key::Key_Control);
    else if (modifiers.testFlag(Qt::AltModifier) && key != Qt::Key::Key_Alt)
            keySequence.append(Qt::Key::Key_Alt);
        
    // Handle unknown keys
    if (key == Qt::Key_unknown)
        return;

    // Pressing Esc or Backspace will clear the content
    if (key == Qt::Key_Escape || key == Qt::Key_Backspace)
    {
        keySequence.clear();
        setText(NULL);
        return;
    }

    keySequence.append(key);
    setText(getSequence());
}

void KeyBindEdit::mousePressEvent(QMouseEvent* event)
{
    // Clear the keybinding when the text box is clicked
    keySequence.clear();
    setText(NULL);

    QWidget::mousePressEvent(event);
}
