#ifndef INPUTCONDITIONCGNSFILE_H
#define INPUTCONDITIONCGNSFILE_H

#include <QObject>

class InputConditionCgnsFile : public QObject
{
	Q_OBJECT

public:
	struct SolverInformation {
		enum class GridType {Structured, Unstructured};

		QString solverName;
		QString solverVersion;
		GridType gridType;
		int size;
		int iSize;
		int jSize;
		int timeSteps;
	};

	InputConditionCgnsFile();

	QString fileName() const;
	void setFileName(const QString& filename);

	bool isEffective() const;

	const SolverInformation& solverInformation() const;

	QStringList baseIterativeIntegerResults() const;
	QStringList baseIterativeRealResults() const;
	QStringList gridNodeIntegerResults() const;
	QStringList gridNodeRealResults() const;
	QStringList gridCellIntegerResults() const;
	QStringList gridCellRealResults() const;
	QStringList gridEdgeIIntegerResults() const;
	QStringList gridEdgeIRealResults() const;
	QStringList gridEdgeJIntegerResults() const;
	QStringList gridEdgeJRealResults() const;

signals:
	void changed();

private:
	bool loadData();

	QString m_fileName;
	bool m_isEffective;

	SolverInformation m_solverInformation;

	QStringList m_baseIterativeIntegerResults;
	QStringList m_baseIterativeRealResults;
	QStringList m_gridNodeIntegerResults;
	QStringList m_gridNodeRealResults;
	QStringList m_gridCellIntegerResults;
	QStringList m_gridCellRealResults;
	QStringList m_gridEdgeIIntegerResults;
	QStringList m_gridEdgeIRealResults;
	QStringList m_gridEdgeJIntegerResults;
	QStringList m_gridEdgeJRealResults;
};

#endif // INPUTCONDITIONCGNSFILE_H
