#ifndef POST3DWINDOWCONTOURGROUPSETTINGDIALOG_H
#define POST3DWINDOWCONTOURGROUPSETTINGDIALOG_H

#include <guibase/scalarsettingcontainer.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include "post3dwindowfacedataitem.h"
#include <guibase/scalarbarsetting.h>
#include <guibase/vtktextpropertysettingcontainer.h>
#include <QDialog>
#include <QList>
#include <QMap>

#include <string>
#include <vector>

namespace Ui
{
	class Post3dWindowContourGroupSettingDialog;
}

class iRICMainWindow;
class PostZoneDataContainer;
class LookupTableContainer;
class Post3dWindowGridTypeDataItem;

class QListWidgetItem;

class Post3dWindowContourGroupSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Post3dWindowContourGroupSettingDialog(QWidget* parent = nullptr);
	~Post3dWindowContourGroupSettingDialog();

	void setGridTypeDataItem(Post3dWindowGridTypeDataItem* item);
	void setZoneData(PostZoneDataContainer* zoneData);
	void setColorBarTitleMap(const QMap<std::string, QString>& titleMap);

	ScalarSettingContainer scalarSetting() const;
	void setScalarSetting(const ScalarSettingContainer& setting);

	LookupTableContainer lookupTable() const;
	void setLookupTable(const LookupTableContainer& c);

	const QMap<QString, Post3dWindowFaceDataItem::Setting>& faceMap() const;
	void setFaceMap(const QMap<QString, Post3dWindowFaceDataItem::Setting>& map);

	QString scalarBarTitle() const;

public slots:
	void accept() override;

private slots:
	void addFaceSetting();
	void removeFaceSetting();
	void switchFaceSetting(QListWidgetItem* current, QListWidgetItem* previous);

	void solutionChanged(int index);
	void checkSelectedNumber();
	void updateFaceMap();
	void showColorBarDialog();

private:
	std::string target() const;

	Ui::Post3dWindowContourGroupSettingDialog* ui;

	ScalarSettingContainer m_scalarSetting;
	LookupTableContainer m_lookupTable;
	QMap<QString, Post3dWindowFaceDataItem::Setting> m_faceMap;

	Post3dWindowGridTypeDataItem* m_gridTypeDataItem;
	std::vector<std::string> m_targets;
	QMap<std::string, QString> m_colorBarTitleMap;

	bool m_isRemoving;
};

#endif // POST3DWINDOWCONTOURGROUPSETTINGDIALOG_H
