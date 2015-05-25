#ifndef POST2DWINDOWCONTOURSETTINGDIALOG_H
#define POST2DWINDOWCONTOURSETTINGDIALOG_H

#include "../post2d_global.h"
#include <guibase/contoursettingwidget.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include "post2dwindownodescalargroupdataitem.h"
#include <guibase/scalarbarsetting.h>
#include <guibase/vtktextpropertysettingcontainer.h>

#include <QDialog>
#include <QList>

namespace Ui
{
	class Post2dWindowContourSettingDialog;
}

class iRICMainWindow;
class PostZoneDataContainer;
class LookupTableContainer;
class Post2dWindowGridTypeDataItemInterface;

class POST2D_EXPORT Post2dWindowContourSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Post2dWindowContourSettingDialog(QWidget* parent = nullptr);
	~Post2dWindowContourSettingDialog();
	void setZoneData(PostZoneDataContainer* zoneData);
	void setCurrentSolution(QString sol);
	void setContour(ContourSettingWidget::Contour c);
	void setNumberOfDivision(int div);
	void setGridTypeDataItem(Post2dWindowGridTypeDataItemInterface* item);
	void setRegionMode(StructuredGridRegion::RegionMode mode);
	void setRange(StructuredGridRegion::Range2d range);
	void setFillUpper(bool fill);
	void setFillLower(bool fill);
	void setOpacityPercent(int o);
	void setColorBarTitleMap(const QMap<QString, QString>& titlemap);
	void setScalarBarSetting(const ScalarBarSetting& setting);
	void setTitleTextSetting(const vtkTextPropertySettingContainer& cont);
	void setLabelTextSetting(const vtkTextPropertySettingContainer& cont);

	QString currentSolution();
	ContourSettingWidget::Contour contour();
	int numberOfDivision();
	const LookupTableContainer& lookupTable();
	StructuredGridRegion::RegionMode regionMode();
	StructuredGridRegion::Range2d range();
	bool fillUpper();
	bool fillLower();
	int opacityPercent();

	void disableActive();
	void hideOpacity();
	QString scalarBarTitle();
	const ScalarBarSetting& scalarBarSetting() const {return m_scalarBarSetting;}
	const vtkTextPropertySettingContainer titleTextSetting() const {return m_titleTextSetting;}
	const vtkTextPropertySettingContainer labelTextSetting() const {return m_labelTextSetting;}

public slots:
	void accept() override;

private slots:
	void solutionChanged(int index);
	void showRegionDialog();
	void showScalarBarDialog();

private:
	int m_gridDims[3];
	QList<QString> m_solutions;
	StructuredGridRegion::RegionMode m_regionMode;
	StructuredGridRegion::Range2d m_region;
	Ui::Post2dWindowContourSettingDialog* ui;

	bool m_activeAvailable;
	bool m_unstructured;
	QMap<QString, QString> m_colorBarTitleMap;
	ScalarBarSetting m_scalarBarSetting;
	vtkTextPropertySettingContainer m_titleTextSetting;
	vtkTextPropertySettingContainer m_labelTextSetting;
	Post2dWindowGridTypeDataItemInterface* m_gridTypeDataItem;
	LookupTableContainer m_lookupTable;
};

#endif // POST2DWINDOWCONTOURSETTINGDIALOG_H
