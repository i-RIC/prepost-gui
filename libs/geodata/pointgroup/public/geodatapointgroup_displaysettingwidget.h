#ifndef GEODATAPOINTGROUP_DISPLAYSETTINGWIDGET_H
#define GEODATAPOINTGROUP_DISPLAYSETTINGWIDGET_H

#include "../geodatapointgroup.h"
#include "../private/geodatapointgroup_displaysetting.h"

#include <misc/modifycommandwidget.h>

#include <QPixmap>

namespace Ui {
class GeoDataPointGroup_DisplaySettingWidget;
}

class QTableView;

class GeoDataPointGroup::DisplaySettingWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	explicit DisplaySettingWidget(QWidget *parent = nullptr);
	~DisplaySettingWidget();

	QTableView* sizesTable() const;
	QPushButton* removeButton() const;

	QUndoCommand* createModifyCommand(bool apply) override;

	DisplaySetting setting() const;
	void setSetting(const DisplaySetting& setting);

	void setSetting(DisplaySetting* setting);
	void setIsReferenceInformation(bool isReference);

private slots:
	void importImage();
	void updateImage();
	void clearImage();

	void addValue();
	void removeValue();

private:
	QPixmap m_pixmap;
	DisplaySetting* m_displaySetting;
	DisplaySetting m_copySetting;

	class ScaleSizePairTableController;
	ScaleSizePairTableController* m_scaleSizePairTableController;

	Ui::GeoDataPointGroup_DisplaySettingWidget *ui;
};

#endif // GEODATAPOINTGROUP_DISPLAYSETTINGWIDGET_H
