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
	PostTimeSteps(PostSolutionInfo* parent);

	void loadFromCgnsFile(iRICLib::H5CgnsFile& file) override;
	const QList<double>& timesteps() const;
	bool dataExists() const override;
	void checkStepsUpdate(iRICLib::H5CgnsFile& file);
	void informSteps();

protected:
	void clearArray() override;

signals:
	void stepsUpdated(QList<double> steps);
	void stepsUpdated();

private:
	QList<double> m_timesteps;
};

#endif // POSTTIMESTEPS_H
