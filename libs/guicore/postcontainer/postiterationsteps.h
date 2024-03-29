#ifndef POSTITERATIONSTEPS_H
#define POSTITERATIONSTEPS_H

#include "../guicore_global.h"
#include "postabstractsteps.h"

#include <QList>

namespace iRICLib {
	class H5CgnsFile;
} // namespace iRICLib

/// This class stores the iterationsteps included in cgns file.
class GUICOREDLL_EXPORT PostIterationSteps : public PostAbstractSteps
{
	Q_OBJECT

public:
	PostIterationSteps(PostSolutionInfo* parent);

	void loadFromCgnsFile(iRICLib::H5CgnsFile& file) override;
	const QList<int>& iterationSteps() const;
	bool dataExists() const override;
	void checkStepsUpdate(iRICLib::H5CgnsFile& file);
	void informSteps();

protected:
	void clearArray() override;

signals:
	void stepsUpdated(const QList<int>& steps);
	void stepsUpdated();

private:
	QList<int> m_iterationSteps;
};

#endif // POSTITERATIONSTEPS_H
