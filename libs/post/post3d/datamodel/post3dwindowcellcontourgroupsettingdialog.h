#ifndef POST3DWINDOWCELLCONTOURGROUPSETTINGDIALOG_H
#define POST3DWINDOWCELLCONTOURGROUPSETTINGDIALOG_H

#include <QDialog>
#include <QMap>

#include "post3dcellrangesettingcontainer.h"

#include <guibase/scalarsettingcontainer.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>

#include <string>

namespace Ui {
class Post3dWindowCellContourGroupSettingDialog;
}

class Post3dWindowGridTypeDataItem;
class PostZoneDataContainer;
class ScalarSettingContainer;

class Post3dWindowCellContourGroupSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Post3dWindowCellContourGroupSettingDialog(QWidget *parent = nullptr);
	~Post3dWindowCellContourGroupSettingDialog();

	void setGridTypeDataItem(Post3dWindowGridTypeDataItem* item);
	void setZoneData(PostZoneDataContainer* zoneData);
	void setColorBarTitleMap(const QMap<std::string, QString>& titleMap);

	ScalarSettingContainer scalarSetting() const;
	void setScalarSetting(const ScalarSettingContainer& setting);

	LookupTableContainer lookupTable() const;
	void setLookupTable(const LookupTableContainer& c);

	std::vector<Post3dCellRangeSettingContainer> rangeSettings();
	void setRangeSettings(const std::vector<Post3dCellRangeSettingContainer> rangeSettings);

	QString scalarBarTitle();

public slots:
	void accept() override;

private slots:
	void addRange();
	void removeRange();
	void setCurrentRange(int row);

	void solutionChanged(int index);
	void showColorBarDialog();

private:
	void updateRangeList();
	void saveCurrentRange();
	std::string target() const;

	ScalarSettingContainer m_scalarSetting;
	LookupTableContainer m_lookupTable;
	std::vector<Post3dCellRangeSettingContainer> m_rangeSettings;

	Post3dWindowGridTypeDataItem* m_gridTypeDataItem;
	std::vector<std::string> m_targets;
	QMap<std::string, QString> m_colorBarTitleMap;
	int m_gridDims[3];
	int m_currentRow;

	Ui::Post3dWindowCellContourGroupSettingDialog *ui;
};

#endif // POST3DWINDOWCELLCONTOURGROUPSETTINGDIALOG_H
