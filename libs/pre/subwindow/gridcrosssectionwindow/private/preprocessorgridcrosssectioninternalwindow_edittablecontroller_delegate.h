#ifndef PREPROCESSORGRIDCROSSSECTIONINTERNALWINDOW_EDITTABLECONTROLLER_DELEGATE_H
#define PREPROCESSORGRIDCROSSSECTIONINTERNALWINDOW_EDITTABLECONTROLLER_DELEGATE_H

#include "preprocessorgridcrosssectioninternalwindow_edittablecontroller.h"

#include <crosssection/private/abstractcrosssectionwindow_edittablecontroller_delegate.h>

class PreProcessorGridCrosssectionInternalWindow::EditTableController::Delegate : public AbstractCrosssectionWindow::EditTableController::Delegate
{
public:
	Delegate(EditTableController* controller);

	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void setEditorData(QWidget* editor, const QModelIndex& index) const override;
	void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
	void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
	GridAttributeDisplaySettingContainer* displaySetting(int column) const;
};

#endif // PREPROCESSORGRIDCROSSSECTIONINTERNALWINDOW_EDITTABLECONTROLLER_DELEGATE_H
