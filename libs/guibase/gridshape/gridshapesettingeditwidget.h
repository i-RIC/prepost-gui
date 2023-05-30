#ifndef GRIDSHAPESETTINGEDITWIDGET_H
#define GRIDSHAPESETTINGEDITWIDGET_H

#include "../guibase_global.h"

#include <QWidget>

namespace Ui {
class GridShapeSettingEditWidget;
}

class GridShapeSettingContainer;

class GUIBASEDLL_EXPORT GridShapeSettingEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit GridShapeSettingEditWidget(QWidget *parent);
	~GridShapeSettingEditWidget();

	void hideShape();
	void hideIndex();

	GridShapeSettingContainer setting() const;
	void setSetting(GridShapeSettingContainer* setting);
	void setSetting(const GridShapeSettingContainer& setting);

	QUndoCommand* createModifyCommand(bool allowMerge) const;

private slots:
	void handleOutlineOnlyClick();

private:
	GridShapeSettingContainer* m_setting;

	Ui::GridShapeSettingEditWidget *ui;
};

#endif // GRIDSHAPESETTINGEDITWIDGET_H
