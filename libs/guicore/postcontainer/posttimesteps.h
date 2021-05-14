#ifndef POSTTIMESTEPS_H
#define POSTTIMESTEPS_H

#include "../guicore_global.h"
#include "postabstractsteps.h"

#include <QList>

namespace iRICLib {
	class H5CgnsFile;
} // namespace iRICLib

/// This class stores the timesteps included in cgns file.
class GUICOREDLL_EXPORT PostTimeSteps : public PostAbstractSteps
{
	Q_OBJECT

public:
	PostTimeSteps(ProjectDataItem* parent);

	void loadFromCgnsFile(iRICLib::H5CgnsFile& file);
	void loadFromCgnsFile(const int fn) override;
	const QList<double>& timesteps() const;
	bool dataExists() const override;
	void checkStepsUpdate(iRICLib::H5CgnsFile& file);
	void informSteps();

protected:
	void clearArray() override;

signals:
	void stepsUpdated(QList<double> steps);
	void stepsUpdated(int fn);

private:
	QList<double> m_timesteps;
};

#endif // POSTTIMESTEPS_H
