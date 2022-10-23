#ifndef REGION2DSETTINGEDITWIDGET_H
#define REGION2DSETTINGEDITWIDGET_H

#include "../guicore_global.h"

#include <QWidget>

namespace Ui {
class Region2dSettingEditWidget;
}

class Region2dSettingContainer;

class QUndoCommand;

class GUICOREDLL_EXPORT Region2dSettingEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit Region2dSettingEditWidget(QWidget *parent = nullptr);
	~Region2dSettingEditWidget();

	void disableActive();
	void hideCustom();
	void setDimensions(int idim, int jdim);

	QUndoCommand* createModifyCommand();

	Region2dSettingContainer setting();
	void setSetting(Region2dSettingContainer* setting);
	void setSetting(const Region2dSettingContainer& setting);

signals:
	void modeChanged();

private slots:
	void handleIMinChange(int value);
	void handleIMaxChange(int value);
	void handleJMinChange(int value);
	void handleJMaxChange(int value);

private:
	Region2dSettingContainer* m_setting;

	Ui::Region2dSettingEditWidget *ui;
};

#endif // REGION2DSETTINGEDITWIDGET_H
