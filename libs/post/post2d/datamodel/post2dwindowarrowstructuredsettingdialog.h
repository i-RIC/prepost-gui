#ifndef POST2DWINDOWARROWSTRUCTUREDSETTINGDIALOG_H
#define POST2DWINDOWARROWSTRUCTUREDSETTINGDIALOG_H

#include "../post2dwindowdataitem.h"
#include "post2dwindownodevectorarrowgroupdataitem.h"

#include <QDialog>

class PostZoneDataContainer;
class ArrowSettingContainer;

namespace Ui
{
	class Post2dWindowArrowStructuredSettingDialog;
}

class Post2dWindowArrowStructuredSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit Post2dWindowArrowStructuredSettingDialog(QWidget* parent = nullptr);
	~Post2dWindowArrowStructuredSettingDialog();
	void setZoneData(PostZoneDataContainer* data);
	void setSolution(const QString& sol);
	const QString solution() const;
	void setScalarValue(const QString& scalar);
	const QString scalarValue() const;
	void setColor(const QColor& color);
	const QColor color() const;
	void setRegionMode(StructuredGridRegion::RegionMode regionMode);
	StructuredGridRegion::RegionMode regionMode() const;
	void setRange(const StructuredGridRegion::Range2d& range);
	StructuredGridRegion::Range2d range() const;
	void setSamplingRates(int irate, int jrate);
	int iSamplingRate() const;
	int jSamplingRate() const;
	void disableActive();

	void setLengthMode(Post2dWindowNodeVectorArrowGroupDataItem::LengthMode lm);
	Post2dWindowNodeVectorArrowGroupDataItem::LengthMode lengthMode() const;
	void setStandardValue(double stdVal);
	double standardValue() const;
	void setLegendLength(int len);
	int legendLength() const;
	void setMinimumValue(double minVal);
	double minimumValue() const;

	void setMapping(Post2dWindowNodeVectorArrowGroupDataItem::Mapping mapping);
	Post2dWindowNodeVectorArrowGroupDataItem::Mapping mapping() const;
	void setArrowSetting(const ArrowSettingContainer& acon);
	ArrowSettingContainer arrowSetting() const;

private:
	void setupSolutionComboBox(PostZoneDataContainer* zoneData);

private slots:
	void showRegionDialog();
	void samplingAllToggle(bool toggled);

private:
	QList<QString> m_solutions;
	QList<QString> m_scalars;

	int m_gridDims[3];
	bool m_activeAvailable;
	StructuredGridRegion::RegionMode m_regionMode;
	StructuredGridRegion::Range2d m_range;
	Ui::Post2dWindowArrowStructuredSettingDialog* ui;
};

#endif // POST2DWINDOWARROWSTRUCTUREDSETTINGDIALOG_H
