#ifndef GRIDSHAPEEDITDIALOG_H
#define GRIDSHAPEEDITDIALOG_H

#include "guibase_global.h"

#include <QDialog>
#include <QMap>

class QComboBox;

namespace Ui {
	class GridShapeEditDialog;
}

class GUIBASEDLL_EXPORT GridShapeEditDialog : public QDialog {
	Q_OBJECT

public:
	enum Shape {Outline, Wireframe};
	GridShapeEditDialog(QWidget *parent = nullptr);
	~GridShapeEditDialog();
	void hideShape();
	void hideIndex();
	void setGridColor(const QColor& color);
	QColor gridColor();
	void setShape(Shape shape);
	Shape shape();
	void setIndexVisible(bool visible);
	bool indexVisible();
	void setIndexColor(const QColor& color);
	QColor indexColor();

private slots:
	void outlineToggled(bool toggled);

private:
	Ui::GridShapeEditDialog *ui;
};

#endif // GRIDSHAPEEDITDIALOG_H
