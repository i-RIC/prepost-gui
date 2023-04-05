#ifndef COLORMAPENUMERATESETTINGEDITWIDGET_H
#define COLORMAPENUMERATESETTINGEDITWIDGET_H

#include "../guicore_global.h"
#include "colormapsettingeditwidgeti.h"
#include "colormapenumeratesettingcontainer.h"

#include <QWidget>

namespace Ui {
class ColorMapEnumerateSettingEditWidget;
}

class EnumLoader;

class GUICOREDLL_EXPORT ColorMapEnumerateSettingEditWidget : public ColorMapSettingEditWidgetI
{
	Q_OBJECT

public:
	explicit ColorMapEnumerateSettingEditWidget(QWidget *parent);
	~ColorMapEnumerateSettingEditWidget();

	ColorMapEnumerateSettingContainer concreteSetting() const;
	void setConcreteSetting(const ColorMapEnumerateSettingContainer& setting);

	void setDisableOtherThanImageSetting(bool disabled) override;
	QUndoCommand* createModifyCommand() const override;

private slots:
	void importSetting() override;
	void exportSetting() override;

private:
	QTableView* colorTable() const;

	void applySetting();

	void setupWidget() override;

	ColorMapEnumerateSettingContainer m_concreteSetting;

	class ColorTableController;
	ColorTableController* m_colorTableController;

	Ui::ColorMapEnumerateSettingEditWidget *ui;
};

#endif // COLORMAPENUMERATESETTINGEDITWIDGET_H
