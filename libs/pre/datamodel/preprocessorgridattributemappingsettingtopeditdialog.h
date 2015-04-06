#ifndef PREPROCESSORGRIDATTRIBUTEMAPPINGSETTINGTOPEDITDIALOG_H
#define PREPROCESSORGRIDATTRIBUTEMAPPINGSETTINGTOPEDITDIALOG_H

#include "preprocessorrootdataitem.h"
#include <rawdata/pointmap/rawdatapointmapmappingmode.h>
#include "../misc/preprocessorgridattributemappingmode.h"

#include <QDialog>

namespace Ui {
	class PreProcessorGridAttributeMappingSettingTopEditDialog;
}

class PreProcessorGridAttributeMappingSettingTopEditDialog : public QDialog {
	Q_OBJECT
public:
	PreProcessorGridAttributeMappingSettingTopEditDialog(QWidget *parent = 0);
	~PreProcessorGridAttributeMappingSettingTopEditDialog();
	PreProcessorGridAttributeMappingMode::Mode mappingMode();
	void setMappingMode(PreProcessorGridAttributeMappingMode::Mode mm);
	RawDataPointmapMappingMode::Mode pointmapMappingMode();
	void setPointmapMappingMode(RawDataPointmapMappingMode::Mode mm);

	bool autoMode(){return m_autoMode;}
	void setAutoMode(bool a){m_autoMode = a;}
	double streamWiseLength(){return m_streamWiseLength;}
	void setStreamWiseLength(double len){m_streamWiseLength = len;}
	double crossStreamLength(){return m_crossStreamLength;}
	void setCrossStreamLength(double len){m_crossStreamLength = len;}
	int numExpansion(){return m_numExpansion;}
	void setNumExpansion(int num){m_numExpansion = num;}
	double weightExponent(){return m_weightExponent;}
	void setWeightExponent(double exp){m_weightExponent = exp;}
private slots:
	void showDetailDialog();
private:
	bool m_autoMode;
	double m_streamWiseLength;
	double m_crossStreamLength;
	int m_numExpansion;
	double m_weightExponent;
	Ui::PreProcessorGridAttributeMappingSettingTopEditDialog *ui;
};

#endif // PREPROCESSORGRIDATTRIBUTEMAPPINGSETTINGTOPEDITDIALOG_H
