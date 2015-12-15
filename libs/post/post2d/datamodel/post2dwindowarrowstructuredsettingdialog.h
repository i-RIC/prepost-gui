#ifndef POST2DWINDOWARROWSTRUCTUREDSETTINGDIALOG_H
#define POST2DWINDOWARROWSTRUCTUREDSETTINGDIALOG_H

#include "../post2dwindowdataitem.h"
#include "post2dwindownodevectorarrowgroupdataitem.h"
#include "post2dwindownodevectorarrowgroupstructureddataitem.h"

#include <QDialog>

class PostZoneDataContainer;
class ArrowShapeContainer;

namespace Ui
{
	class Post2dWindowArrowStructuredSettingDialog;
}

class Post2dWindowArrowStructuredSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Post2dWindowArrowStructuredSettingDialog(QWidget* parent = nullptr);
	~Post2dWindowArrowStructuredSettingDialog();
	void setZoneData(PostZoneDataContainer* data);

	void disableActive();

	void setSettings(const Post2dWindowNodeVectorArrowGroupDataItem::Setting& s, const Post2dWindowNodeVectorArrowGroupStructuredDataItem::Setting& sts);
	Post2dWindowNodeVectorArrowGroupDataItem::Setting setting() const;
	Post2dWindowNodeVectorArrowGroupStructuredDataItem::Setting stSetting() const;

private:
	void setupSolutionComboBox(PostZoneDataContainer* zoneData);

private slots:
	void showRegionDialog();
	void samplingAllToggle(bool toggled);

private:
	std::vector<std::string> m_targets;
	std::vector<std::string> m_scalars;

	int m_gridDims[3];
	bool m_activeAvailable;

	Post2dWindowNodeVectorArrowGroupDataItem::Setting m_setting;
	Post2dWindowNodeVectorArrowGroupStructuredDataItem::Setting m_stSetting;

	Ui::Post2dWindowArrowStructuredSettingDialog* ui;
};

#endif // POST2DWINDOWARROWSTRUCTUREDSETTINGDIALOG_H
