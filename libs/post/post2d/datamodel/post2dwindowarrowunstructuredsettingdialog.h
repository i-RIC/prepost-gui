#ifndef POST2DWINDOWARROWUNSTRUCTUREDSETTINGDIALOG_H
#define POST2DWINDOWARROWUNSTRUCTUREDSETTINGDIALOG_H

#include "../post2dwindowdataitem.h"
#include "post2dwindownodevectorarrowgroupdataitem.h"
#include "post2dwindownodevectorarrowgroupunstructureddataitem.h"

#include <QDialog>

class PostZoneDataContainer;
class ArrowSettingContainer;

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

	void setSettings(const Post2dWindowNodeVectorArrowGroupDataItem::Setting& s, const Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::Setting& unss);
	Post2dWindowNodeVectorArrowGroupDataItem::Setting setting() const;
	Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::Setting unsSetting() const;

private slots:
	void showRegionDialog();

private:
	void setupSolutionComboBox(PostZoneDataContainer* zoneData);
	QList<QString> m_solutions;
	QList<QString> m_scalars;
	bool m_activeAvailable;
	Post2dWindowNodeVectorArrowGroupDataItem::Setting m_setting;
	Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::Setting m_unsSetting;

	Ui::Post2dWindowArrowUnstructuredSettingDialog* ui;
};

#endif // POST2DWINDOWARROWUNSTRUCTUREDSETTINGDIALOG_H
