#include "particlearbitrarytimeeditdialog.h"
#include "particlesettingeditwidget.h"
#include "ui_particlesettingeditwidget.h"

#include <misc/qundocommandhelper.h>
#include <misc/stringtool.h>
#include <misc/valuemodifycommandt.h>

#include <map>

ParticleSettingEditWidget::ParticleSettingEditWidget(QWidget *parent) :
	ModifyCommandWidget(parent),
	m_setting {nullptr},
	m_mainFile {nullptr},
	ui(new Ui::ParticleSettingEditWidget)
{
	ui->setupUi(this);

	setupNominations(5);

	connect(ui->timeSlider, &QSlider::valueChanged, this, &ParticleSettingEditWidget::handleTimeSliderChange);
	connect(ui->arbitraryEditButton, &QPushButton::clicked, this, &ParticleSettingEditWidget::editArbitraryTimes);
}

ParticleSettingEditWidget::~ParticleSettingEditWidget()
{
	delete ui;
}

void ParticleSettingEditWidget::setSolutions(const std::unordered_map<std::string, QString>& solutions)
{
	std::map<QString, std::string> smap;

	for (const auto& pair : solutions) {
		smap.insert({pair.second, pair.first});
	}

	auto comboBox = ui->solutionComboBox;
	comboBox->blockSignals(true);
	comboBox->clear();
	m_solutions.clear();
	for (const auto& pair : smap) {
		comboBox->addItem(pair.first);
		m_solutions.push_back(pair.second);
	}
	comboBox->blockSignals(false);

	if (m_solutions.size() == 1) {
		ui->physValLabel->hide();
		ui->solutionComboBox->hide();
	}
}

void ParticleSettingEditWidget::setProjectMainFile(ProjectMainFile* file) {
	m_mainFile = file;
}

QUndoCommand* ParticleSettingEditWidget::createModifyCommand(bool apply)
{
	return new ValueModifyCommmand<ParticleSettingContainer>(iRIC::generateCommandId("ParticleSettingModify"), apply,
																														setting(), m_setting);
}

ParticleSettingContainer ParticleSettingEditWidget::setting() const
{
	ParticleSettingContainer ret;

	ret.target = m_solutions.at(ui->solutionComboBox->currentIndex()).c_str();

	if (ui->periodicalRadioButton->isChecked()) {
		ret.generateMode = ParticleSettingContainer::GenerateMode::Periodical;
	} else if (ui->arbitraryRadioButton->isChecked()) {
		ret.generateMode = ParticleSettingContainer::GenerateMode::Arbitrary;
	}

	int v = ui->timeSlider->value();
	if (v < static_cast<int> (m_skipNominations.size())) {
		ret.timeMode = ParticleSettingContainer::TimeMode::Skip;
		ret.timeSamplingRate = m_skipNominations.at(m_skipNominations.size() - ui->timeSlider->value() - 1);
	} else if (v > static_cast<int> (m_skipNominations.size())) {
		ret.timeMode = ParticleSettingContainer::TimeMode::Subdivide;
		ret.timeDivision = m_subDivideNominations.at(ui->timeSlider->value() - m_skipNominations.size() - 1);
	} else {
		ret.timeMode = ParticleSettingContainer::TimeMode::Normal;
	}
	ret.arbitraryTimes.setValue(m_arbitraryTimes);

	return ret;
}

void ParticleSettingEditWidget::setSetting(const ParticleSettingContainer& setting)
{
	auto it = std::find(m_solutions.begin(), m_solutions.end(), iRIC::toStr(setting.target));
	if (it == m_solutions.end()) {it = m_solutions.begin();}
	ui->solutionComboBox->setCurrentIndex(it - m_solutions.begin());
	if (setting.generateMode == ParticleSettingContainer::GenerateMode::Periodical) {
		ui->periodicalRadioButton->setChecked(true);
	} else if (setting.generateMode == ParticleSettingContainer::GenerateMode::Arbitrary) {
		ui->arbitraryRadioButton->setChecked(true);
	}

	ui->timeSlider->setValue(static_cast<int>(m_skipNominations.size()));
	if (setting.timeMode == ParticleSettingContainer::TimeMode::Normal) {
		// do nothing
	} else if (setting.timeMode == ParticleSettingContainer::TimeMode::Skip) {
		for (int i = 0; i < static_cast<int>(m_skipNominations.size()); ++i) {
			if (m_skipNominations.at(i) == setting.timeSamplingRate) {
				ui->timeSlider->setValue(static_cast<int> (m_skipNominations.size()) - i - 1);
			}
		}
	} else if (setting.timeMode == ParticleSettingContainer::TimeMode::Subdivide) {
		for (int i = 0; i < static_cast<int>(m_subDivideNominations.size()); ++i) {
			if (m_subDivideNominations.at(i) == setting.timeDivision) {
				ui->timeSlider->setValue(static_cast<int> (m_skipNominations.size() + i + 1));
			}
		}
	}

	m_arbitraryTimes = setting.arbitraryTimes.value();
}

void ParticleSettingEditWidget::setSetting(ParticleSettingContainer* setting)
{
	m_setting = setting;
	setSetting(*setting);
}

void ParticleSettingEditWidget::setupNominations(int max)
{
	for (int n = 2; n <= max; ++n) {
		m_subDivideNominations.push_back(n);
		m_skipNominations.push_back(n);
	}
	ui->timeSlider->setMaximum(static_cast<int>(m_subDivideNominations.size() + m_skipNominations.size()));
	ui->timeMinLabel->setText(QString("1/%1").arg(max));
	ui->timeMaxLabel->setText(QString("%1").arg(max));
}

void ParticleSettingEditWidget::handleTimeSliderChange(int val)
{
	if (val < static_cast<int>(m_skipNominations.size())) {
		ui->timeValueLabel->setText(QString("1/%1").arg(m_skipNominations.at(static_cast<int>(m_skipNominations.size()) - val - 1)));
	} else if (val > static_cast<int>(m_skipNominations.size())) {
		ui->timeValueLabel->setText(QString("%1").arg(m_subDivideNominations.at(val - static_cast<int>(m_skipNominations.size()) - 1)));
	} else {
		ui->timeValueLabel->setText("1");
	}
}

void ParticleSettingEditWidget::editArbitraryTimes()
{
	ParticleArbitraryTimeEditDialog dialog(this);
	dialog.setMainFile(m_mainFile);
	dialog.setTimeSteps(m_arbitraryTimes);
	int ret = dialog.exec();

	if (ret == QDialog::Rejected) {return;}

	m_arbitraryTimes = dialog.timeSteps();
}
