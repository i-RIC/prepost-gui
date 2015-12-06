#ifndef POST3DWINDOWCONTOURGROUPSETTINGDIALOG_H
#define POST3DWINDOWCONTOURGROUPSETTINGDIALOG_H

#include <guibase/contoursettingwidget.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include "post3dwindowfacedataitem.h"
#include <guibase/scalarbarsetting.h>
#include <guibase/vtktextpropertysettingcontainer.h>
#include <QDialog>
#include <QList>
#include <QMap>

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
	void setZoneData(PostZoneDataContainer* zoneData);
	void setCurrentSolution(const std::string& sol);
	void setContour(ContourSettingWidget::Contour c);
	void setNumberOfDivision(int div);
	void setLookupTable(const LookupTableContainer& c);
	void setGridTypeDataItem(Post3dWindowGridTypeDataItem* item);
	void setFaceMap(const QMap<QString, Post3dWindowFaceDataItem::Setting>& map);
	void setFillUpper(bool fill);
	void setFillLower(bool fill);

	std::string currentSolution() const;
	ContourSettingWidget::Contour contour();
	int numberOfDivision();
	LookupTableContainer& lookupTable();
	const QMap<QString, Post3dWindowFaceDataItem::Setting>& faceMap();
	bool fillUpper();
	bool fillLower();

	void setColorBarTitleMap(const QMap<std::string, QString>& titleMap);
	void setScalarBarSetting(const ScalarBarSetting& setting);
	void setTitleTextSetting(const vtkTextPropertySettingContainer& cont);
	void setLabelTextSetting(const vtkTextPropertySettingContainer& cont);

	QString scalarBarTitle();
	const ScalarBarSetting& scalarBarSetting() const {return m_scalarBarSetting;}
	const vtkTextPropertySettingContainer titleTextSetting() const {return m_titleTextSetting;}
	const vtkTextPropertySettingContainer labelTextSetting() const {return m_labelTextSetting;}

public slots:
	void accept() override;
	void addFaceSetting();
	void removeFaceSetting();
	void switchFaceSetting(QListWidgetItem* current, QListWidgetItem* previous);

private slots:
	void solutionChanged(int index);
	void checkSelectedNumber();
	void updateFaceMap();
	void showColorBarDialog();

private:
	QList<std::string> m_solutions;
	Ui::Post3dWindowContourGroupSettingDialog* ui;
	Post3dWindowGridTypeDataItem* m_gridTypeDataItem;
	QMap<QString, Post3dWindowFaceDataItem::Setting> m_faceMap;
	QMap<std::string, QString> m_colorBarTitleMap;
	LookupTableContainer m_lookupTable;
	bool m_isRemoving;

	ScalarBarSetting m_scalarBarSetting;
	vtkTextPropertySettingContainer m_titleTextSetting;
	vtkTextPropertySettingContainer m_labelTextSetting;
};

#endif // POST3DWINDOWCONTOURGROUPSETTINGDIALOG_H
