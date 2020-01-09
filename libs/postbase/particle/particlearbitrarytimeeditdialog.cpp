#include "particlearbitrarytimeeditdialog.h"
#include "ui_particlearbitrarytimeeditdialog.h"

#include <guibase/timeformat/timeformatutil.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/posttimesteps.h>

#include <QDateTime>
#include <QInputDialog>

#include <algorithm>
#include <set>

ParticleArbitraryTimeEditDialog::ParticleArbitraryTimeEditDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ParticleArbitraryTimeEditDialog)
{
	ui->setupUi(this);
	connect(ui->addButton, SIGNAL(clicked()), this, SLOT(add()));
	connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(removeSelected()));
}

ParticleArbitraryTimeEditDialog::~ParticleArbitraryTimeEditDialog()
{
	delete ui;
}

void ParticleArbitraryTimeEditDialog::setMainFile(ProjectMainFile* file)
{
	m_mainFile = file;
}

void ParticleArbitraryTimeEditDialog::setTimeSteps(const std::vector<int>& steps)
{
	m_timeSteps = steps;
	updateList();
}

std::vector<int> ParticleArbitraryTimeEditDialog::timeSteps() const
{
	return m_timeSteps;
}

void ParticleArbitraryTimeEditDialog::add()
{
	std::set<int> tset;
	for (int idx : m_timeSteps) {
		tset.insert(idx);
	}
	auto zt = m_mainFile->zeroDateTime();
	auto f = m_mainFile->timeFormat();
	auto cf = m_mainFile->customTimeFormat();
	auto timeVals = m_mainFile->postSolutionInfo()->timeSteps()->timesteps();

	std::vector<int> addVals;
	QStringList addStrs;

	for (int i = 0; i < timeVals.size(); ++i) {
		if (tset.find(i) != tset.end()) {continue;}
		double timeVal = timeVals.at(i);

		addVals.push_back(i);
		addStrs.push_back(TimeFormatUtil::formattedString(zt, timeVal, f, cf));
	}
	bool ok;
	QString selectedItem = QInputDialog::getItem(this, tr("Add time step"), tr("Select Time step to add"), addStrs, 0, false, &ok);
	if (! ok) {return;}

	int idx = addStrs.indexOf(selectedItem);
	m_timeSteps.push_back(addVals.at(idx));

	std::sort(m_timeSteps.begin(), m_timeSteps.end());
	updateList();
}

void ParticleArbitraryTimeEditDialog::removeSelected()
{
	int row = ui->listWidget->currentRow();
	if (row == -1) {return;}

	m_timeSteps.erase(m_timeSteps.begin() + row);
	updateList();
	if (ui->listWidget->count() == 0) {return;}
	if (row == ui->listWidget->count()) {
		ui->listWidget->setCurrentRow(row - 1);
	} else {
		ui->listWidget->setCurrentRow(row);
	}
}

void ParticleArbitraryTimeEditDialog::updateList()
{
	auto zt = m_mainFile->zeroDateTime();
	auto f = m_mainFile->timeFormat();
	auto cf = m_mainFile->customTimeFormat();
	auto timeVals = m_mainFile->postSolutionInfo()->timeSteps()->timesteps();

	ui->listWidget->clear();

	for (int t : m_timeSteps) {
		if (t >= timeVals.size()) {continue;}

		double timeVal = timeVals.at(t);
		ui->listWidget->addItem(TimeFormatUtil::formattedString(zt, timeVal, f, cf));
	}
}
