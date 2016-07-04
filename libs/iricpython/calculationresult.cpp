#include "application.h"
#include "calculationresult.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/posttimesteps.h>
#include <guicore/postcontainer/postdataexportdialog.h>

iRICPython::CalculationResult::CalculationResult(const Application* app) :
	m_app {app}
{}

int iRICPython::CalculationResult::length() const
{
	return postSolutionInfo()->timeSteps()->timesteps().length();
}

void iRICPython::CalculationResult::exportToFolder(const std::string& folderName, const std::string& prefix, const std::vector<int>& steps, ExportType type)
{
	PostDataExportDialog::Format format = PostDataExportDialog::Format::VTKBinary;
	switch (type) {
	case ExportType::vtkAscii:
		format = PostDataExportDialog::Format::VTKASCII;
		break;
	case ExportType::VtkBinary:
		format = PostDataExportDialog::Format::VTKBinary;
		break;
	case ExportType::CSV:
		format = PostDataExportDialog::Format::CSV;
		break;
	case ExportType::ESRIShape:
		format = PostDataExportDialog::Format::ESRIShape;
		break;
	}

	postSolutionInfo()->exportCalculationResult(folderName, prefix, steps, format);

}

PostSolutionInfo* iRICPython::CalculationResult::postSolutionInfo() const
{
	return m_app->projectData()->mainfile()->postSolutionInfo();
}
