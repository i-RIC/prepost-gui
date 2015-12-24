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

	struct GUIBASEDLL_EXPORT Setting : public CompositeContainer
	{
		Setting();
		Setting(const Setting& s);

		Setting& operator=(const Setting& s);

		ColorContainer color;
		EnumContainerT<GridShapeEditDialog::Shape> shape;
		BoolContainer indexVisible;
		ColorContainer indexColor;
	};

	GridShapeEditDialog(QWidget* parent = nullptr);
	~GridShapeEditDialog();

	void hideShape();
	void hideIndex();
	void setSetting(const Setting& setting);
	Setting setting() const;

private slots:
	void outlineToggled(bool toggled);

private:
	Ui::GridShapeEditDialog* ui;
};

#endif // GRIDSHAPEEDITDIALOG_H
