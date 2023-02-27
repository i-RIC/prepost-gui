#ifndef STRUCTURED2DFILTERINGSETTINGEDITWIDGET_H
#define STRUCTURED2DFILTERINGSETTINGEDITWIDGET_H

#include "../guicore_global.h"

#include <QWidget>

namespace Ui {
class Structured2dFilteringSettingEditWidget;
}

class Structured2dFilteringSettingContainer;

class GUICOREDLL_EXPORT Structured2dFilteringSettingEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit Structured2dFilteringSettingEditWidget(QWidget *parent = nullptr);
	~Structured2dFilteringSettingEditWidget();

	void setSetting(Structured2dFilteringSettingContainer* setting);
	void setModeToAll();

	QUndoCommand* createModifyCommand();

private:
	Structured2dFilteringSettingContainer setting() const;
	void setSetting(const Structured2dFilteringSettingContainer& setting);

	Structured2dFilteringSettingContainer* m_setting;
	Ui::Structured2dFilteringSettingEditWidget *ui;
};

#endif // STRUCTURED2DFILTERINGSETTINGEDITWIDGET_H
