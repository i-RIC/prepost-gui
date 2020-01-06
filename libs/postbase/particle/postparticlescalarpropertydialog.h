#ifndef POSTPARTICLESCALARPROPERTYDIALOG_H
#define POSTPARTICLESCALARPROPERTYDIALOG_H

#include "../postbase_global.h"

#include <guibase/scalarsettingcontainer.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>

#include <QDialog>
#include <QMap>
#include <QString>

#include <string>
#include <vector>

class PostWindowGridTypeDataItemInterface;

class vtkPolyData;

namespace Ui {
class PostParticleScalarPropertyDialog;
}

class POSTBASEDLL_EXPORT PostParticleScalarPropertyDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PostParticleScalarPropertyDialog(QWidget *parent = nullptr);
	~PostParticleScalarPropertyDialog();

	void setGridTypeDataItem(PostWindowGridTypeDataItemInterface* item);
	void setData(vtkPolyData* data);

	ScalarSettingContainer setting() const;
	void setSetting(const ScalarSettingContainer setting);

	const LookupTableContainer& lookupTable() const;

	QString scalarBarTitle() const;
	void setScalarBarTitleMap(const QMap<std::string, QString>& titlemap);

	int particleSize() const;
	void setParticleSize(int size);

	QColor customColor() const;
	void setCustomColor(const QColor& c);

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
	QMap<std::string, QString> m_scalarBarTitleMap;
	LookupTableContainer m_lookupTable;

	Ui::PostParticleScalarPropertyDialog *ui;
};

#endif // POSTPARTICLESCALARPROPERTYDIALOG_H
