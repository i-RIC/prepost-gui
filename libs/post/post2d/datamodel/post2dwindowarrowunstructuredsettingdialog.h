#ifndef POST2DWINDOWARROWUNSTRUCTUREDSETTINGDIALOG_H
#define POST2DWINDOWARROWUNSTRUCTUREDSETTINGDIALOG_H

#include "../post2dwindowdataitem.h"
#include "post2dwindownodevectorarrowunstructuredsetting.h"

#include <QDialog>
#include <vector>

class PostZoneDataContainer;

namespace Ui
{
	class Post2dWindowArrowUnstructuredSettingDialog;
}

class Post2dWindowArrowUnstructuredSettingDialog : public QDialog
{
	Q_OBJECT
public:
	explicit Post2dWindowArrowUnstructuredSettingDialog(QWidget* parent = nullptr);
	~Post2dWindowArrowUnstructuredSettingDialog();

	void setZoneData(PostZoneDataContainer* data);
	void disableActive();

	Post2dWindowNodeVectorArrowUnstructuredSetting setting() const;
	void setSetting(const Post2dWindowNodeVectorArrowUnstructuredSetting& setting);

private slots:
	void showRegionDialog();

private:
	void setupSolutionComboBox(PostZoneDataContainer* zoneData);

	std::vector<std::string> m_solutions;
	std::vector<std::string> m_scalars;

	bool m_activeAvailable;

	Post2dWindowNodeVectorArrowUnstructuredSetting m_setting;

	Ui::Post2dWindowArrowUnstructuredSettingDialog* ui;
};

#endif // POST2DWINDOWARROWUNSTRUCTUREDSETTINGDIALOG_H
