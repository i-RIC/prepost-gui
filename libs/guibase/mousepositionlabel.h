#ifndef MOUSEPOSITIONLABEL_H
#define MOUSEPOSITIONLABEL_H

#include "guibase_global.h"

#include <QLabel>
#include <QVector2D>

class GUIBASEDLL_EXPORT MousePositionLabel : public QLabel
{
	Q_OBJECT
public:
	explicit MousePositionLabel(QWidget *parent = nullptr);

public slots:
	void updatePosition(const QVector2D& position);
	void clear();
};

#endif // MOUSEPOSITIONLABEL_H
