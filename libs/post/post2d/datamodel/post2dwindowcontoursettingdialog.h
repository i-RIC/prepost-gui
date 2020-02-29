#ifndef POST2DWINDOWCONTOURSETTINGDIALOG_H
#define POST2DWINDOWCONTOURSETTINGDIALOG_H

#include "../post2d_global.h"

#include <guibase/widget/contoursettingwidget.h>
#include <guibase/scalarbarsetting.h>
#include <guibase/vtktextpropertysettingcontainer.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <postbase/post2dwindowcontoursetting.h>

#include <cgnslib.h>

#include <QDialog>
#include <QMap>
#include <QList>

#include <string>
#include <vector>

namespace Ui
{
	class Post2dWindowContourSettingDialog;
}

class PostZoneDataContainer;
class LookupTableContainer;
class PostWindowGridTypeDataItemInterface;

class POST2D_EXPORT Post2dWindowContourSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Post2dWindowContourSettingDialog(QWidget* parent = nullptr);
	~Post2dWindowContourSettingDialog();

	void setGridTypeDataItem(PostWindowGridTypeDataItemInterface* item);
	void setZoneData(PostZoneDataContainer* zoneData, GridLocation_t gridLocation);
	void disableActive();
	void disablePhysicalValueComboBox();
	void hideOpacity();

	void setSetting(const Post2dWindowContourSetting& setting);
	void setColorBarTitleMap(const QMap<std::string, QString>& titlemap);

	Post2dWindowContourSetting setting() const;
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
	GridLocation_t m_gridLocation;

	Post2dWindowContourSetting m_setting;
	QMap<std::string, QString> m_colorBarTitleMap;
	Ui::Post2dWindowContourSettingDialog* ui;
};

#endif // POST2DWINDOWCONTOURSETTINGDIALOG_H
