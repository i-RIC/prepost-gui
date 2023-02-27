#ifndef STRUCTURED3DFILTERINGSETTINGEDITWIDGET_H
#define STRUCTURED3DFILTERINGSETTINGEDITWIDGET_H

#include "../guicore_global.h"

#include <QWidget>

namespace Ui {
class Structured3dFilteringSettingEditWidget;
}

class Structured3dFilteringSettingContainer;

class GUICOREDLL_EXPORT Structured3dFilteringSettingEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit Structured3dFilteringSettingEditWidget(QWidget *parent = nullptr);
	~Structured3dFilteringSettingEditWidget();

	Structured3dFilteringSettingContainer setting() const;
	void setSetting(const Structured3dFilteringSettingContainer& setting);
	void setSetting(Structured3dFilteringSettingContainer* setting);

	QUndoCommand* createModifyCommand();

private:
	Structured3dFilteringSettingContainer* m_setting;
	Ui::Structured3dFilteringSettingEditWidget *ui;
};

#endif // STRUCTURED3DFILTERINGSETTINGEDITWIDGET_H
