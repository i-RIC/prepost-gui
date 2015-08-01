#ifndef GRIDSHAPEEDITDIALOG_H
#define GRIDSHAPEEDITDIALOG_H

#include "guibase_global.h"

#include <misc/colorcontainer.h>
#include <misc/boolcontainer.h>
#include <misc/enumcontainert.h>
#include <misc/compositecontainer.h>

#include <QDialog>

class QComboBox;

namespace Ui
{
	class GridShapeEditDialog;
}

/// Dialog to edit grid shape
class GUIBASEDLL_EXPORT GridShapeEditDialog : public QDialog
{
	Q_OBJECT

public:
	/// Grid shape display type
	enum Shape {
		Outline,    ///< Outline only
		Wireframe   ///< Show all grid cell lines
	};

	struct Setting : public CompositeContainer
	{
		Setting();

		ColorContainer color;
		EnumContainerT<GridShapeEditDialog::Shape> shape;
		BoolContainer indexVisible;
		ColorContainer indexColor;
	};

	GridShapeEditDialog(QWidget* parent = nullptr);
	~GridShapeEditDialog();
	/// Hide the widget to select grid shape
	void hideShape();
	/// Hide the widget to toggle indices display on/off
	void hideIndex();
	/// Set the setting
	void setSetting(const Setting& setting);
	/// Get the setting
	Setting setting() const;

/*
	/// Set the grid color
	void setGridColor(const QColor& color);
	/// The grid color
	QColor gridColor();
	/// Set the grid shape display type
	void setShape(Shape shape);
	/// The grid shape display type
	Shape shape();
	/// Set whether the grid indices are visible
	void setIndexVisible(bool visible);
	/// Whether the grid indices are visible
	bool indexVisible();
	/// Set the grid indices color
	void setIndexColor(const QColor& color);
	/// The grid indices color
	QColor indexColor();
*/
private slots:
	void outlineToggled(bool toggled);

private:
	Ui::GridShapeEditDialog* ui;
};

#endif // GRIDSHAPEEDITDIALOG_H
