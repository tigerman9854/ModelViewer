#include "KeySequenceParse.h"

KeySequenceParse::KeySequenceParse(QString text)
{
	QStringList parseMe = text.split('+');
	for (int i = 0; i < parseMe.length(); i++)
	{
		// NOTE: We are forced to save keybinds as strings then we have to convert them back into
		// a QKeySequence. Unfortuntaly shift, ctrl, and others don't go throgh this cleanly.
		// So we have to handle them here as edge cases.
		if (parseMe[i] == "Shift") {
			parsed.insert(QKeySequence(Qt::Key::Key_Shift));
			parsed_vec.append(Qt::Key::Key_Shift);
		}
		else if (parseMe[i] == "Ctrl") {
			parsed.insert(QKeySequence(Qt::Key::Key_Control));
			parsed_vec.append(Qt::Key::Key_Control);
		}
		else if (parseMe[i] == "Alt") {
			parsed.insert(QKeySequence(Qt::Key::Key_Alt));
			parsed_vec.append(Qt::Key::Key_Alt);
		}
		else {
			parsed.insert(QKeySequence(parseMe[i]));
			parsed_vec.append(static_cast<Qt::Key>(QKeySequence(parseMe[i])[0]));
		}
	}
}

KeySequenceParse::~KeySequenceParse()
{
}

QSet<QKeySequence> KeySequenceParse::get()
{
	return parsed;
}

QVector<Qt::Key> KeySequenceParse::getVec()
{
	return parsed_vec;
}
