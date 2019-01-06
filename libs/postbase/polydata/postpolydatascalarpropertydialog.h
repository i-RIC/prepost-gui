#ifndef POSTPOLYDATASCALARPROPERTYDIALOG_H
#define POSTPOLYDATASCALARPROPERTYDIALOG_H

#include "../postbase_global.h"

#include <guibase/scalarsettingcontainer.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>

#include <QDialog>
#include <QString>

#include <map>
#include <string>
#include <vector>

class PostWindowGridTypeDataItemInterface;
class PostZoneDataContainer;

class vtkPolyData;

namespace Ui {
class PostPolyDataScalarPropertyDialog;
}

class POSTBASEDLL_EXPORT PostPolyDataScalarPropertyDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PostPolyDataScalarPropertyDialog(QWidget *parent = nullptr);
	~PostPolyDataScalarPropertyDialog();

	void setGridTypeDataItem(PostWindowGridTypeDataItemInterface* item);
	void setPolyData(vtkPolyData* polyData);

	ScalarSettingContainer setting() const;
	void setSetting(const ScalarSettingContainer setting);

	const LookupTableContainer& lookupTable() const;

	QString scalarBarTitle() const;
	void setScalarBarTitleMap(const std::map<std::string, QString> &titlemap);

	QColor customColor() const;
	void setCustomColor(const QColor& c);

	int lineWidgh() const;
	void setLineWidth(int width);

public slots:
	void accept();

private slots:
	void solutionChanged(int index);
	void showScalarBarDialog();

private:
	std::string target() const;

	PostWindowGridTypeDataItemInterface* m_gridTypeDataItem;

	ScalarSettingContainer m_setting;
	std::vector<std::string> m_targets;
	std::map<std::string, QString> m_scalarBarTitleMap;
	LookupTableContainer m_lookupTable;

	Ui::PostPolyDataScalarPropertyDialog *ui;
};

#endif // POSTPOLYDATASCALARPROPERTYDIALOG_H
