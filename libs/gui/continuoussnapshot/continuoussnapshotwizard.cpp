#include "../main/iricmainwindow.h"
#include "continuoussnapshotconfirmpage.h"
#include "continuoussnapshotfilepropertypage.h"
#include "continuoussnapshotgoogleearthsettingpage.h"
#include "continuoussnapshotintroductionpage.h"
#include "continuoussnapshotmoviepropertypage.h"
#include "continuoussnapshottimestepsettingpage.h"
#include "continuoussnapshotwindowselectionpage.h"
#include "continuoussnapshotwizard.h"

#include <guicore/base/snapshotenabledwindowinterface.h>

#include <QDir>
#include <QLabel>
#include <QListWidget>
#include <QMdiSubWindow>
#include <QPushButton>
#include <QVBoxLayout>

ContinuousSnapshotWizard::ContinuousSnapshotWizard(QWidget* parent) :
	QWizard(parent),
	m_coordinateSystem {nullptr}
{
	addPage(new ContinuousSnapshotIntroductionPage(this));
	addPage(new ContinuousSnapshotWindowSelectionPage(this));
	addPage(new ContinuousSnapshotFilePropertyPage(this));
	addPage(new ContinuousSnapshotMoviePropertyPage(this));
	addPage(new ContinuousSnapshotTimestepSettingPage(this));
	addPage(new ContinuousSnapshotGoogleEarthSettingPage(this));
	addPage(new ContinuousSnapshotConfirmPage(this));
	setWindowTitle(tr("Continuous Snapshot / Movie / Google Earth Export Wizard"));
	resize(600, 500);

	m_mainWindow = dynamic_cast<iRICMainWindow*>(parent);
}

void ContinuousSnapshotWizard::accept()
{
	QWizard::accept();
}
