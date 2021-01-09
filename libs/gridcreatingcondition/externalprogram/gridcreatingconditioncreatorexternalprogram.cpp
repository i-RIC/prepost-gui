#include "gridcreatingconditioncreatorexternalprogram.h"
#include "gridcreatingconditionexternalprogram.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiontranslator.h>
#include <misc/errormessage.h>
#include <misc/iricrootpath.h>
#include <misc/xmlsupport.h>

#include <QCoreApplication>
#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QMessageBox>
#include <QString>
#include <QTextStream>

GridCreatingConditionCreatorExternalProgram::GridCreatingConditionCreatorExternalProgram(const QString& folderName, const QLocale& locale) :
	GridCreatingConditionCreator()
{
	m_folderName = folderName;
	QDir folder(m_folderName);
	QString filename = folder.absoluteFilePath(SolverDefinition::FILENAME);
	// Set up translator first.
	// Solver definition translator is used.
	SolverDefinitionTranslator translator(m_folderName, locale);
	QDomDocument doc;
	QString errorStr;
	int errorLine;
	int errorColumn;
	QFile file(filename);
	QString errorHeader = "Error occured while loading %1\n";
	bool ok = doc.setContent(&file, &errorStr, &errorLine, &errorColumn);
	if (! ok) {
		QString msg = errorHeader;
		msg.append("Parse error %2 at line %3, column %4");
		msg = msg.arg(filename).arg(errorStr).arg(errorLine).arg(errorColumn);
		throw ErrorMessage(msg);
	}
	QDomElement docElem = doc.documentElement();
	iRIC::removeAllComments(&docElem);
	QDomNode SDNode = doc.documentElement();
	QDomElement sdElem = SDNode.toElement();

	m_name = sdElem.attribute("name");
	m_caption = translator.translate(sdElem.attribute("caption"));

	// get readme filename and load description from that.
	filename = SolverDefinition::README;
	filename.append("_").append(locale.name());
	// get the local language version README first.
	filename = folder.absoluteFilePath(filename);
	bool nodesc = false;
	if (! QFile::exists(filename)) {
		// local language version does not exists;
		filename = folder.absoluteFilePath(SolverDefinition::README);
		if (! QFile::exists(filename)) {
			// README does not exists.
			m_description = "";
			nodesc = true;
		}
	}
	if (! nodesc) {
		QFile file(filename);
		QString ret, buffer;
		file.open(QIODevice::ReadOnly | QIODevice::Text);
		QTextStream in(&file);
		in.setCodec("UTF-8");
		while (! in.atEnd()) {
			buffer = in.readLine();
			ret.append(buffer);
			ret.append("\n");
		}
		m_description = ret;
	}

	QString gridType = sdElem.attribute("gridtype");
	if (gridType == "structured2d") {
		m_gridType = SolverDefinitionGridType::gtStructured2DGrid;
	} else if (gridType == "unstructured2d") {
		m_gridType = SolverDefinitionGridType::gtUnstructured2DGrid;
	}
}

GridCreatingConditionCreatorExternalProgram::~GridCreatingConditionCreatorExternalProgram()
{}

SolverDefinitionGridType::GridType GridCreatingConditionCreatorExternalProgram::gridType() const
{
	return m_gridType;
}

GridCreatingCondition* GridCreatingConditionCreatorExternalProgram::create(ProjectDataItem* parent)
{
	return new GridCreatingConditionExternalProgram(m_folderName, parent->iricMainWindow()->locale(), parent, this);
}

const QString& GridCreatingConditionCreatorExternalProgram::folderName() const
{
	return m_folderName;
}

QList<GridCreatingConditionCreator*> GridCreatingConditionCreatorExternalProgram::getList(const QLocale& locale, QWidget* mainWindow)
{
	QList<GridCreatingConditionCreator*> ret;
	QString gcFolder("gridcreators");
	QString private_gcFolder("private_gridcreators");

	QDir inst(iRICRootPath::get());
	inst.cdUp();
	inst.cdUp();

	QString targetDirectory = inst.absoluteFilePath(gcFolder);
	if (! QDir(targetDirectory).exists()) {
		inst.mkdir(gcFolder);
	}
	addToList(targetDirectory, locale, mainWindow, &ret);

	QString private_targetDirectory = inst.absoluteFilePath(private_gcFolder);
	if (! QDir(private_targetDirectory).exists()) {
		inst.mkdir(private_gcFolder);
	}
	addToList(private_targetDirectory, locale, mainWindow, &ret);

	return ret;
}

void GridCreatingConditionCreatorExternalProgram::addToList(const QString& dirName, const QLocale& locale, QWidget* w, QList<GridCreatingConditionCreator*>* list)
{
	QDir gcsdir(dirName);
	QStringList subdirs = gcsdir.entryList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);

	for (const QString& subdir : subdirs) {
		QString defFileName = QDir(gcsdir.absoluteFilePath(subdir)).absoluteFilePath(SolverDefinition::FILENAME);
		if (! QFile::exists(defFileName)) {continue;}

		// definition.xml exists.
		try {
			GridCreatingConditionCreatorExternalProgram* creator = new GridCreatingConditionCreatorExternalProgram(gcsdir.absoluteFilePath(subdir), locale);
			list->append(creator);
		} catch (ErrorMessage& e) {
			QMessageBox::warning(w, tr("Warning"), tr("Error occured while loading grid creator definition file in folder \"%1\". This grid creator is ignored.\n%2").arg(subdir).arg(e));
		}
	}
}
