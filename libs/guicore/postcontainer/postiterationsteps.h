#ifndef POSTITERATIONSTEPS_H
#define POSTITERATIONSTEPS_H

#include "../guicore_global.h"
#include "postabstractsteps.h"

#include <QList>

/// This class stores the iterationsteps included in cgns file.
class GUICOREDLL_EXPORT PostIterationSteps : public PostAbstractSteps
{
	Q_OBJECT

public:
	PostIterationSteps(ProjectDataItem* parent);

	void loadFromCgnsFile(const int fn) override;
	const QList<int>& iterationSteps() const;
	bool dataExists() const override;
	void checkStepsUpdate(int fn);
	void informSteps();

protected:
	void clearArray() override;

signals:
	void stepsUpdated(const QList<int>& steps);
	void stepsUpdated(int fn);

private:
	QList<int> m_iterationSteps;
};

#endif // POSTITERATIONSTEPS_H
