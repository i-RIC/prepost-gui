#ifndef POST2DWINDOWARROWSTRUCTUREDSETTINGDIALOG_H
#define POST2DWINDOWARROWSTRUCTUREDSETTINGDIALOG_H

#include "../post2dwindowdataitem.h"

#include "post2dwindownodevectorarrowstructuredsetting.h"

#include <QDialog>

class PostZoneDataContainer;
class Post2dWindowGridTypeDataItem;

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

	void setGridType(Post2dWindowGridTypeDataItem* item);
	void setZoneData(PostZoneDataContainer* data);

	void disableActive();

	Post2dWindowNodeVectorArrowStructuredSetting setting() const;
	void setSetting(const Post2dWindowNodeVectorArrowStructuredSetting& s);

private:
	void setupSolutionComboBox(PostZoneDataContainer* zoneData);

private slots:
	void showRegionDialog();
	void samplingAllToggle(bool toggled);

private:
	std::vector<std::string> m_targets;
	std::vector<std::string> m_scalars;

	int m_gridDims[3];
	bool m_activeDisabled;

	Post2dWindowGridTypeDataItem* m_gridTypeDataItem;

	Post2dWindowNodeVectorArrowStructuredSetting m_setting;

	Ui::Post2dWindowArrowStructuredSettingDialog* ui;
};

#endif // POST2DWINDOWARROWSTRUCTUREDSETTINGDIALOG_H
