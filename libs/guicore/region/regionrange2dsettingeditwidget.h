#ifndef REGIONRANGE2DSETTINGEDITWIDGET_H
#define REGIONRANGE2DSETTINGEDITWIDGET_H

#include "../guicore_global.h"

#include <QWidget>

namespace Ui {
class RegionRange2dSettingEditWidget;
}

class RegionRange2dSettingContainer;

class QUndoCommand;

class GUICOREDLL_EXPORT RegionRange2dSettingEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit RegionRange2dSettingEditWidget(QWidget *parent = nullptr);
	~RegionRange2dSettingEditWidget();

	void setDimensions(int idim, int jdim);
	void setupFullRange();

	QUndoCommand* createModifyCommand(bool allowMerge = false);

	RegionRange2dSettingContainer setting();
	void setSetting(RegionRange2dSettingContainer* setting);
	void setSetting(const RegionRange2dSettingContainer& setting);

private slots:
	void handleIMinChange(int value);
	void handleIMaxChange(int value);
	void handleJMinChange(int value);
	void handleJMaxChange(int value);

private:
	RegionRange2dSettingContainer* m_setting;

	Ui::RegionRange2dSettingEditWidget *ui;
};

#endif // REGIONRANGE2DSETTINGEDITWIDGET_H
