#ifndef POST2DBIRDEYEWINDOWCONTOURSETTINGDIALOG_H
#define POST2DBIRDEYEWINDOWCONTOURSETTINGDIALOG_H

#include "../post2dbirdeye_global.h"
#include "post2dbirdeyewindowcontoursetting.h"

#include <guibase/widget/contoursettingwidget.h>
#include <guibase/scalarbarsetting.h>
#include <guibase/vtktextpropertysettingcontainer.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>

#include <QButtonGroup>
#include <QDialog>
#include <QMap>
#include <QList>

#include <h5cgnszone.h>

#include <string>
#include <vector>

namespace Ui
{
	class Post2dBirdEyeWindowContourSettingDialog;
}

class PostZoneDataContainer;
class LookupTableContainer;
class PostWindowGridTypeDataItemInterface;

class POST2DBIRDEYE_EXPORT Post2dBirdEyeWindowContourSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Post2dBirdEyeWindowContourSettingDialog(QWidget* parent = nullptr);
	~Post2dBirdEyeWindowContourSettingDialog();

	void setGridTypeDataItem(PostWindowGridTypeDataItemInterface* item);
	void setZoneData(PostZoneDataContainer* zoneData, iRICLib::H5CgnsZone::SolutionPosition position);
	void disableActive();
	void disablePhysicalValueComboBox();

	void setSetting(const Post2dBirdEyeWindowContourSetting& setting);
	void setColorBarTitleMap(const QMap<std::string, QString>& titlemap);

	Post2dBirdEyeWindowContourSetting setting() const;
	const LookupTableContainer& lookupTable();
	QString scalarBarTitle();

public slots:
	void accept() override;

private slots:
	void targetChanged(int index);
	void showRegionDialog();
	void showScalarBarDialog();

private:
	std::string target() const;

	int m_gridDims[3];
	std::vector<std::string> m_solutions;
	PostWindowGridTypeDataItemInterface* m_gridTypeDataItem;
	LookupTableContainer m_lookupTable;
	bool m_activeAvailable;
	bool m_unstructured;
	iRICLib::H5CgnsZone::SolutionPosition m_position;

	Post2dBirdEyeWindowContourSetting m_setting;
	QMap<std::string, QString> m_colorBarTitleMap;

	QButtonGroup m_colorButtonGroup;
	Ui::Post2dBirdEyeWindowContourSettingDialog* ui;
};

#endif // POST2DBIRDEYEWINDOWCONTOURSETTINGDIALOG_H
