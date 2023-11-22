#include "iricmainwindow_calculatedresultmanager.h"

#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/post/v4postzoneselectingdialog.h>
#include <guicore/postcontainer/v4postcalculatedresultlistdialog.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>

#include <QInputDialog>
#include <QMessageBox>

#include <vector>

void iRICMainWindow::CalculatedResultManager::openManageDialog(iRICMainWindow* w)
{
	auto info = w->m_projectData->mainfile()->postSolutionInfo();

	// reload without calculated result
	info->setCalculatedResultDisabled(true);
	info->setCurrentStep(info->currentStep());

	std::vector<const std::vector<v4PostZoneDataContainer*>*> containersVec;
	QStringList items;

	auto c1 = &(info->v4ZoneContainers1D());
	if (c1->size() > 0) {
		containersVec.push_back(c1);
		items.push_back(iRICMainWindow::tr("One dimension"));
	}
	auto c2 = &(info->v4ZoneContainers2D());
	if (c2->size() > 0) {
		containersVec.push_back(c2);
		items.push_back(iRICMainWindow::tr("Two dimensions"));
	}
	auto c3 = &(info->v4ZoneContainers3D());
	if (c3->size() > 0) {
		containersVec.push_back(c3);
		items.push_back(iRICMainWindow::tr("Three dimensions"));
	}
	const std::vector<v4PostZoneDataContainer*>* containers = nullptr;
	if (containersVec.size() == 0) {
		QMessageBox::warning(w, iRICMainWindow::tr("Warning"), iRICMainWindow::tr("There is no calculation data, so you can not define simple operation results."));
		return;
	} else if (containersVec.size() == 1) {
		containers = containersVec.at(0);
	} else {
		bool ok = true;
		QString selected = QInputDialog::getItem(w, iRICMainWindow::tr("Select dimension"), iRICMainWindow::tr("Dimension"), items, 0, false, &ok);
		if (! ok) {return;}
		int index = items.indexOf(selected);
		containers = containersVec.at(index);
	}
	v4PostZoneDataContainer* zoneDataContainer = nullptr;
	if (containers->size() == 1) {
		zoneDataContainer = containers->at(0);
	} else {
		v4PostZoneSelectingDialog dialog(w);
		dialog.setContainers(*containers);
		int result = dialog.exec();
		if (result == QDialog::Rejected) {return;}
		for (auto c : *containers) {
			if (c->zoneName() == dialog.zoneName()) {
				zoneDataContainer = c;
				break;
			}
		}
		if (zoneDataContainer == nullptr) {return;}
	}
	v4PostCalculatedResultListDialog dialog(w);
	dialog.setZoneDataContainer(zoneDataContainer);
	dialog.exec();

	// reload with calculated result
	info->setCalculatedResultDisabled(false);
	info->setCurrentStep(info->currentStep());
}
