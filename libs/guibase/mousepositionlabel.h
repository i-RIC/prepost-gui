#ifndef MOUSEPOSITIONLABEL_H
#define MOUSEPOSITIONLABEL_H

#include "guibase_global.h"

#include <QLabel>

class QVector2D;

/// Widget to show mouse position (x, y) in a label
class GUIBASEDLL_EXPORT MousePositionLabel : public QLabel
{
	Q_OBJECT

public:
	explicit MousePositionLabel(QWidget* parent = nullptr);

public slots:
	/// Update the position to display in the widget
	void updatePosition(const QVector2D& position);
	/// Clear the content
	void clear();
};

#endif // MOUSEPOSITIONLABEL_H
