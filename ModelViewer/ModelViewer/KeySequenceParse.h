#pragma once
#include <QKeySequence>
#include <QString>
#include <QSet>
#include <Qt>

class KeySequenceParse
{
public:
	KeySequenceParse(QString text);
	~KeySequenceParse();
	QSet<QKeySequence> get();
	QVector<Qt::Key> getVec();
private:
	QSet<QKeySequence> parsed;
	QVector<Qt::Key> parsed_vec;
};