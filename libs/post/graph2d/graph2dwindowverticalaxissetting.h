#ifndef GRAPH2DWINDOWVERTICALAXISSETTING_H
#define GRAPH2DWINDOWVERTICALAXISSETTING_H

#include <QString>
#include <guicore/project/projectdataitem.h>

class Graph2dWindowVerticalAxisSetting : public ProjectDataItem
{
public:
	enum RangeMode {
		rmAuto,
		rmCustom,
	};
	enum LabelMode {
		lmAuto,
		lmCustom,
	};
	Graph2dWindowVerticalAxisSetting();
	Graph2dWindowVerticalAxisSetting(ProjectDataItem* parent);
	RangeMode rangeMode() {
		return m_rangeMode;
	}
	void setRangeMode(RangeMode rm) {
		m_rangeMode = rm;
	}
	double customRangeMax() const {return m_customRangeMax;}
	double customRangeMin() const {return m_customRangeMin;}
	void setCustomRange(double min, double max) {
		m_customRangeMin = min;
		m_customRangeMax = max;
	}
	LabelMode labelMode() const {return m_labelMode;}
	void setLabelMode(LabelMode lm) {
		m_labelMode = lm;
	}
	const QString& customLabel() const {return m_customLabel;}
	void setCustomLabel(const QString& label) {
		m_customLabel = label;
	}
	Graph2dWindowVerticalAxisSetting& operator=(const Graph2dWindowVerticalAxisSetting& setting);

protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);

private:
	void init();
	RangeMode m_rangeMode;
	LabelMode m_labelMode;
	double m_customRangeMin;
	double m_customRangeMax;
	QString m_customLabel;
};

#endif // GRAPH2DWINDOWVERTICALAXISSETTING_H
