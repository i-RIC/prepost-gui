#ifndef POSTTIMESTEPS_H
#define POSTTIMESTEPS_H

#include "../guicore_global.h"
#include "postabstractsteps.h"
#include <QList>

/// This class stores the timesteps included in cgns file.
class GUICOREDLL_EXPORT PostTimeSteps : public PostAbstractSteps
{
	Q_OBJECT
public:
	/// Constructor
	PostTimeSteps(ProjectDataItem* parent) : PostAbstractSteps(parent) {}
	void loadFromCgnsFile(const int fn);
	const QList<double>& timesteps() const {return m_timesteps;}
	bool dataExists() {return m_timesteps.count() > 0;}
	void checkStepsUpdate(int fn);
	void informSteps();
protected:
	void clearArray() {m_timesteps.clear();}
signals:
	void stepsUpdated(QList<double> steps);
	void stepsUpdated(int fn);
private:
	QList<double> m_timesteps;
};

#endif // POSTTIMESTEPS_H
