#ifndef GENERALFILTERINGSETTINGEDITWIDGET_H
#define GENERALFILTERINGSETTINGEDITWIDGET_H

#include "../guicore_global.h"

#include <QWidget>

namespace Ui {
class GeneralFilteringSettingEditWidget;
}

class GeneralFilteringSettingContainer;

class QUndoCommand;

class GUICOREDLL_EXPORT GeneralFilteringSettingEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit GeneralFilteringSettingEditWidget(QWidget *parent = nullptr);
	~GeneralFilteringSettingEditWidget();

	void setSetting(GeneralFilteringSettingContainer* setting);
	GeneralFilteringSettingContainer setting() const;

	QUndoCommand* createModifyCommand(QUndoCommand* parent = nullptr) const;

private:
	GeneralFilteringSettingContainer* m_setting;

	Ui::GeneralFilteringSettingEditWidget *ui;
};

#endif // GENERALFILTERINGSETTINGEDITWIDGET_H
