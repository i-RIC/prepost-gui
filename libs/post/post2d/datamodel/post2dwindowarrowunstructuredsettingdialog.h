#ifndef POST2DWINDOWARROWUNSTRUCTUREDSETTINGDIALOG_H
#define POST2DWINDOWARROWUNSTRUCTUREDSETTINGDIALOG_H

#include "../post2dwindowdataitem.h"
#include "post2dwindownodevectorarrowgroupdataitem.h"
#include "post2dwindownodevectorarrowgroupunstructureddataitem.h"

#include <QDialog>

class PostZoneDataContainer;
class ArrowSettingContainer;

namespace Ui {
		class Post2dWindowArrowUnstructuredSettingDialog;
}

class Post2dWindowArrowUnstructuredSettingDialog : public QDialog {
		Q_OBJECT
public:
	explicit Post2dWindowArrowUnstructuredSettingDialog(QWidget *parent = nullptr);
	~Post2dWindowArrowUnstructuredSettingDialog();
	void setZoneData(PostZoneDataContainer *data);
	bool isEnabled() const;
	void setSolution(const QString& sol);
	const QString solution();
	void setScalarValue(const QString& scalar);
	const QString scalarValue();
	void setColor(const QColor &color);
	const QColor color() const;
	void setSamplingMode(Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::SamplingMode sm);
	Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::SamplingMode samplingMode();
	void setSamplingRate(int rate);
	int samplingRate();
	void setSamplingNumber(int num);
	int samplingNumber();

	void disableActive();

	void setRegionMode(StructuredGridRegion::RegionMode rm);
	StructuredGridRegion::RegionMode regionMode() const;
	void setLengthMode(Post2dWindowNodeVectorArrowGroupDataItem::LengthMode lm);
	Post2dWindowNodeVectorArrowGroupDataItem::LengthMode lengthMode();
	void setStandardValue(double stdVal);
	double standardValue();
	void setLegendLength(int len);
	int legendLength();
	void setMinimumValue(double minVal);
	double minimumValue();

	void setMapping(Post2dWindowNodeVectorArrowGroupDataItem::Mapping mapping);
	Post2dWindowNodeVectorArrowGroupDataItem::Mapping mapping();
	void setArrowSetting(const ArrowSettingContainer& arrowSetting);
	ArrowSettingContainer arrowSetting();

private slots:
	void showRegionDialog();

private:
	void setupSolutionComboBox(PostZoneDataContainer* zoneData);
	QList<QString> m_solutions;
	QList<QString> m_scalars;
	bool m_activeAvailable;
	StructuredGridRegion::RegionMode m_regionMode;
	Ui::Post2dWindowArrowUnstructuredSettingDialog *ui;
};

#endif // POST2DWINDOWARROWUNSTRUCTUREDSETTINGDIALOG_H
