#include "hydraulicdatariversurveywaterelevation.h"
#include "hydraulicdatariversurveywaterelevationimporter.h"

#include <QFile>
#include <QMessageBox>
#include <QString>
#include <QTextStream>

HydraulicDataRiverSurveyWaterElevationImporter::HydraulicDataRiverSurveyWaterElevationImporter(HydraulicDataCreator* creator) :
	HydraulicDataImporter {creator}
{}

QStringList HydraulicDataRiverSurveyWaterElevationImporter::fileDialogFilters()
{
	QStringList ret;
	ret.append(tr("CSV file (*.csv)"));
	ret.append(tr("Text file (*.txt)"));
	return ret;
}

QStringList HydraulicDataRiverSurveyWaterElevationImporter::acceptableExtensions()
{
	QStringList ret;
	ret << "csv" << "txt";
	return ret;
}

bool HydraulicDataRiverSurveyWaterElevationImporter::importData(HydraulicData* data, int index, const std::set<QString>& usedCaptions, QWidget* w)
{
	QFile file(filename());
	if (! file.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(w, tr("Error"), tr("File open error occured while opening %1.").arg(filename()));
		return false;
	}

	auto we = dynamic_cast<HydraulicDataRiverSurveyWaterElevation*> (data);

	QTextStream in(&file);

	QString line = in.readLine();
	QStringList pieces = line.split(QRegExp("(\\s+)|,"), QString::KeepEmptyParts);
	QString origCaption = pieces.at(index + 1);
	QString caption = origCaption;

	int copyIndex = 1;
	while (usedCaptions.find(caption) != usedCaptions.end()) {
		caption = QString("%1(%2)").arg(origCaption).arg(copyIndex);
		++ copyIndex;
	}
	we->setCaption(caption);
	we->clear();

	do {
		line = in.readLine();
		if (! line.isEmpty()) {
			QStringList pieces = line.split(QRegExp("(\\s+)|,"), QString::KeepEmptyParts);
			QString name = pieces.at(0);
			QString val = pieces.at(index + 1);
			bool ok;
			double doubleVal = val.toDouble(&ok);
			if (! ok) {
				int result = QMessageBox::warning(w, tr("Warning"), tr("In the column for %1, the value \"%2\" for cross section \"%3\" is invalid. The value is ignored.").
																					arg(origCaption).arg(val).arg(name),
																					QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
				if (result == QMessageBox::Cancel) {
					QMessageBox::information(w, tr("Information"), tr("Importing %1 canceled.").arg(origCaption));
					return false;
				}
			} else {
				we->addItem(name, true, doubleVal);
			}
		}
	} while (! line.isEmpty());
	file.close();
	return true;
}

bool HydraulicDataRiverSurveyWaterElevationImporter::doInit(const QString& filename, const QString& /*selectedFilter*/, int* count, QWidget* w)
{
	QFile file(filename);
	if (! file.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(w, tr("Error"), tr("File open error occured while opening %1.").arg(filename));
		return false;
	}

	QTextStream in(&file);

	QString line = in.readLine();
	QStringList pieces = line.split(QRegExp("(\\s+)|,"), QString::KeepEmptyParts);
	*count = pieces.size() - 1;
	do {
		line = in.readLine();
		if (! line.isEmpty()) {
			QStringList pieces = line.split(QRegExp("(\\s+)|,"), QString::KeepEmptyParts);
			if (*count != pieces.size() - 1) {
				QMessageBox::critical(w, tr("Error"), tr("The number of items must be the same in every line."));
				return false;
			}
		}
	} while (! line.isEmpty());
	file.close();
	return true;
}
