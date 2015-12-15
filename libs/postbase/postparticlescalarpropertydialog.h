#ifndef POSTPARTICLESCALARPROPERTYDIALOG_H
#define POSTPARTICLESCALARPROPERTYDIALOG_H

#include "postbase_global.h"

#include <guibase/scalarbarsetting.h>
#include <guibase/vtktextpropertysettingcontainer.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>

#include <QDialog>
#include <QMap>
#include <QString>

#include <string>

class PostWindowGridTypeDataItemInterface;
class PostZoneDataContainer;

namespace Ui {
class PostParticleScalarPropertyDialog;
}

class POSTBASEDLL_EXPORT PostParticleScalarPropertyDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PostParticleScalarPropertyDialog(QWidget *parent = 0);
	~PostParticleScalarPropertyDialog();

	std::string target() const;
	void setTarget(const std::string& target);
	const LookupTableContainer& lookupTable() const;
	void setZoneData(PostZoneDataContainer* zoneData);

	void setGridTypeDataItem(PostWindowGridTypeDataItemInterface* item);

	QString scalarBarTitle() const;
	void setScalarBarTitleMap(const QMap<std::string, QString>& titlemap);
	const ScalarBarSetting& scalarBarSetting() const {return m_scalarBarSetting;}
	void setScalarBarSetting(const ScalarBarSetting& setting);
	const vtkTextPropertySettingContainer titleTextSetting() const {return m_titleTextSetting;}
	void setTitleTextSetting(const vtkTextPropertySettingContainer& cont);
	const vtkTextPropertySettingContainer labelTextSetting() const {return m_labelTextSetting;}
	void setLabelTextSetting(const vtkTextPropertySettingContainer& cont);
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
	QList<std::string> m_attributes;

	ScalarBarSetting m_scalarBarSetting;
	QMap<std::string, QString> m_scalarBarTitleMap;
	vtkTextPropertySettingContainer m_titleTextSetting;
	vtkTextPropertySettingContainer m_labelTextSetting;
	PostWindowGridTypeDataItemInterface* m_gridTypeDataItem;
	LookupTableContainer m_lookupTable;

	Ui::PostParticleScalarPropertyDialog *ui;
};

#endif // POSTPARTICLESCALARPROPERTYDIALOG_H
