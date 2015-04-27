#ifndef VERIFICATIONGRAPHDIALOG_H
#define VERIFICATIONGRAPHDIALOG_H

#include <QDialog>
#include <QList>

class iRICMainWindow;
class PostSolutionInfo;
class MeasuredData;
class PostZoneDataContainer;

class QwtPlotCurve;
class QwtPlotMarker;

namespace Ui {
class VerificationGraphDialog;
}

class VerificationGraphDialog : public QDialog
{
	Q_OBJECT
public:
	enum GraphType {gtSWDvsValues, gtSWDvsError, gtMVvsCR, gtMVvsError};
	explicit VerificationGraphDialog(iRICMainWindow* mainWindow);
	~VerificationGraphDialog();
	void setPostSolutionInfo(PostSolutionInfo* info){
		m_postSolutionInfo = info;
		m_activePostData = nullptr;
		m_activeResult = "";
	}
	void setMeasuredValues(const QList<MeasuredData*> measuredData){
		m_measuredData = measuredData;
		m_activeMeasuredData = nullptr;
		m_activeValue = "";
	}
public slots:
	/// Open setting dialog.
	bool setting();
	/// Export data to CSV file.
	void exportData();

	void setType(int type);
private:
	void updateGraph();
	void clearData();

	GraphType m_graphType;
	PostSolutionInfo* m_postSolutionInfo;
	QList<MeasuredData*> m_measuredData;

	int m_timeStep;
	PostZoneDataContainer* m_activePostData;
	QString m_activeResult;

	MeasuredData* m_activeMeasuredData;
	QString m_activeValue;

	QString m_xLabel;
	QString m_yLabel;

	QwtPlotCurve* m_pointsCurve;
	QwtPlotCurve* m_lineCurve;
	QwtPlotCurve* m_dotLineCurve;
	QwtPlotMarker* m_zeroMarker;

	QVector<double> xVals;
	QVector<double> yVals;

	iRICMainWindow* m_mainWindow;

	Ui::VerificationGraphDialog *ui;
};

#endif // VERIFICATIONGRAPHDIALOG_H
