#ifndef IRICPYTHON_CALCULATIONRESULT_H
#define IRICPYTHON_CALCULATIONRESULT_H

#include "iricpython_api.h"

#include <vector>

class PostSolutionInfo;

namespace iRICPython {

class Application;

class IRICPYTHON_API CalculationResult
{
public:
	enum class ExportType {
		ESRIShape,
		CSV,
		VtkBinary,
		vtkAscii
	};

	CalculationResult(const Application* app);

	int length() const;
	void exportToFolder(const std::string& folderName, const std::string& prefix, const std::vector<int>& steps, ExportType type);

private:
	PostSolutionInfo* postSolutionInfo() const;

	const Application* m_app;
};

} // iRICPython

#endif // IRICPYTHON_CALCULATIONRESULT_H
