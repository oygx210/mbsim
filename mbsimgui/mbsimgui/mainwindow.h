/*
    MBSimGUI - A fronted for MBSim.
    Copyright (C) 2012 Martin Förg

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef __MAINWINDOW_H_
#define __MAINWINDOW_H_

#include <QMainWindow>
#include <QProcess>
#include <QTimer>
#include <QTime>
#include <QDir>
#include <boost/filesystem/path.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <deque>
#include <sstream>
#include <openmbv/mainwindow.h>

class QAction;
class QModelIndex;

namespace OpenMBVGUI {
  class MainWindow;
  class AbstractViewFilter;
}

namespace MBXMLUtils {
  class Eval;
  class NewParamLevel;
  class DOMParser;
}

namespace XERCES_CPP_NAMESPACE {
  class DOMImplementation;
  class DOMLSParser;
  class DOMLSSerializer;
  class DOMDocument;
  class DOMElement;
}

namespace MBSimGUI {

  class MBSimThread;
  class ElementView;
  class ParameterView;
  class EchoView;
  class FileView;
  class PropertyDialog;
  class Element;
  class Frame;
  class Contour;
  class Group;
  class Object;
  class Link;
  class Constraint;
  class Observer;
  class Solver;
  class Parameter;
  class EmbedItemData;
  class Project;
  class FileItemData;
  class EchoStream;

  class MainWindow : public QMainWindow {
    Q_OBJECT

    private:
      bool exitOK = true;
      std::unordered_map<std::string,Element*> idMap;
      Project *project;
      std::vector<FileItemData*> file;
      ElementView *elementView;
      std::vector<ElementView*> itemView;
      ParameterView *parameterView;
      EchoView *echoView;
      FileView *fileView;
      QTabWidget *tabWidget;
      PropertyDialog *editor{nullptr};
      std::shared_ptr<bool> debugStreamFlag;
      QString projectFile;
      QProcess process;
      OpenMBVGUI::MainWindow *inlineOpenMBVMW;
      boost::filesystem::path uniqueTempDir;
      QAction *actionSaveProject, *actionSimulate, *actionOpenMBV, *actionH5plotserie, *actionEigenanalysis, *actionHarmonicResponseAnalysis, *actionStateTable, *actionSaveDataAs, *actionSaveMBSimH5DataAs, *actionSaveOpenMBVDataAs, *actionRefresh, *actionDebug, *actionSaveStateVectorAs, *actionSaveStateTableAs, *actionSaveEigenanalysisAs, *actionSaveHarmonicResponseAnalysisAs, *actionUndo, *actionRedo;
      OpenMBVGUI::AbstractViewFilter *elementViewFilter, *parameterViewFilter;
      QTimer autoSaveTimer;
      QTime statusTime;
      int IDcounter{0};
      std::string currentID;
      enum { maxRecentFiles = 5 };
      QAction *recentProjectFileActs[maxRecentFiles];
      bool allowUndo;
      int maxUndo;
      bool autoRefresh;
      xercesc::DOMDocument *doc;
      std::deque<xercesc::DOMDocument*> undos, redos;
      std::pair<Element*,bool> elementBuffer;
      std::pair<Parameter*,bool> parameterBuffer;
      boost::filesystem::path installPath;
      void initInlineOpenMBV();
      void dragEnterEvent(QDragEnterEvent *event) override;
      void dropEvent(QDropEvent *event) override;
      void closeEvent(QCloseEvent *event) override;
      void showEvent(QShowEvent *event) override;
      bool maybeSave();
      void setCurrentProjectFile(const QString &fileName);
      void updateRecentProjectFileActions();
      void move(bool up);
      void openOptionsMenu(bool justSetOptions=false);
      void openRecentProjectFile();
      void newProject();
      void loadProject(const QString &file);
      bool saveProject(const QString &fileName="", bool modifyStatus=true);
      void edit();
      void undo();
      void redo();
      void copy(bool cut=false);
      void cut() { copy(true); }
      void paste();
      void remove();
      void moveUp() { move(true); }
      void moveDown() { move(false); }
      void saveDataAs();
      void saveMBSimH5DataAs();
      void saveOpenMBVDataAs();
      void saveStateVectorAs();
      void saveStateTableAs();
      void saveEigenanalysisAs();
      void saveHarmonicResponseAnalysisAs();
      void help();
      void about();
      void simulate();
      void interrupt();
      void openmbv();
      void h5plotserie();
      void eigenanalysis();
      void harmonicResponseAnalysis();
      void showStateTable();
      void debug();
      void settingsFinished(int result);
      void applySettings();
      void kill();
      void elementChanged(const QModelIndex &current);
      void elementViewClicked(const QModelIndex &current);
      void parameterViewClicked(const QModelIndex &current);
      void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
      void updateEchoView();
      void updateStatus();
      void autoSaveProject();
      void updateReferences(Element *element);
      void updateParameterReferences(EmbedItemData *parent);
      void saveReferencedFile(int i);
    private slots:
      void selectElement(const std::string& ID);

    public:
      MainWindow(QStringList &arg);
      ~MainWindow() override;
      std::shared_ptr<MBXMLUtils::DOMParser> mbxmlparser;
      std::shared_ptr<MBXMLUtils::Eval> eval;
      xercesc::DOMImplementation *impl;
      xercesc::DOMLSParser *parser;
      xercesc::DOMLSSerializer *serializer;
      xercesc::DOMLSSerializer *basicSerializer;
      void mbsimxml(int task);
      const boost::filesystem::path& getUniqueTempDir() const { return uniqueTempDir; }
      void addParameter(Parameter *parameter, EmbedItemData *parent);
      void addFrame(Frame *frame, Element *parent);
      void addContour(Contour *contour, Element *parent);
      void addGroup(Group *group, Element *parent);
      void addObject(Object *object, Element *parent);
      void addLink(Link *link, Element *parent);
      void addConstraint(Constraint *constraint, Element *parent);
      void addObserver(Observer *observer, Element *parent);
      void loadParameter(EmbedItemData *parent, Parameter *param=nullptr, bool embed=false, bool add=false);
      void removeParameter(EmbedItemData *parent);
      std::tuple<xercesc::DOMElement*,FileItemData*> loadElement(Element *parent, Element *element=nullptr, bool embed=false);
      void loadFrame(Element *parent, Element *element=nullptr, bool embed=false);
      void loadContour(Element *parent, Element *element=nullptr, bool embed=false);
      void loadGroup(Element *parent, Element *element=nullptr, bool embed=false);
      void loadObject(Element *parent, Element *element=nullptr, bool embed=false);
      void loadLink(Element *parent, Element *element=nullptr, bool embed=false);
      void loadConstraint(Element *parent, Element *element=nullptr, bool embed=false);
      void loadObserver(Element *parent, Element *element=nullptr, bool embed=false);
      void loadDynamicSystemSolver(bool embed=false);
      void highlightObject(const std::string &ID);
      const std::string& getHighlightedObject() const { return currentID; }
      ElementView* getElementView() { return elementView; }
      ParameterView* getParameterView() { return parameterView; }
      void setProjectChanged(bool changed=true);
      void selectSolver(Solver *solver);
      void setAllowUndo(bool allowUndo);
      const std::pair<Element*,bool>& getElementBuffer() const { return elementBuffer; }
      const std::pair<Parameter*,bool>& getParameterBuffer() const { return parameterBuffer; }
      Project* getProject() { return project; }
      QTime& getStatusTime() { return statusTime; }
      QString getProjectFile() const { return projectFile; }
      QString getProjectFilePath() const;
      QString getProjectPath() const { return QFileInfo(getProjectFilePath()).canonicalPath(); }
      QDir getProjectDir() const { return QFileInfo(getProjectFilePath()).dir(); }
      bool getAutoRefresh() const { return autoRefresh; }
      bool editorIsOpen() const { return editor; }
      void loadProject();
      bool saveProjectAs();
      void refresh();
      void xmlHelp(const QString &url="");
      void editElementSource();
      void editParametersSource();
      void exportElement();
      void copyElement(bool cut=false);
      void removeElement();
      void enableElement(bool enabled);
      void moveFrame(bool up);
      void moveContour(bool up);
      void moveGroup(bool up);
      void moveObject(bool up);
      void moveLink(bool up);
      void moveConstraint(bool up);
      void moveObserver(bool up);
      void copyParameter(bool cut=false);
      void removeParameter();
      void moveParameter(bool up);
      void saveMBSimH5Data(const QString &file);
      void saveOpenMBVXMLData(const QString &file);
      void saveOpenMBVH5Data(const QString &file);
      void saveStateVector(const QString &file);
      void saveStateTable(const QString &file);
      void saveEigenanalysis(const QString &file);
      void saveHarmonicResponseAnalysis(const QString &file);
      void updateParameters(EmbedItemData *item, bool exceptLatestParameter=false);
      void rebuildTree();
      void exportParameters(EmbedItemData *item=nullptr);
      void loadSolver(bool embed=false);
      void openParameterEditor(bool config=true);
      void openCloneEditor();
      FileItemData* addFile(const QFileInfo &file);
      void removeFile(FileItemData *fileItem);
      std::string getID(Element* element) { std::string ID = std::to_string(IDcounter++); idMap[ID] = element; return ID; }
      int getExitOK() { return exitOK; }
      void setExitBad() { exitOK=false; }
      boost::filesystem::path getInstallPath() const { return installPath; }
    public slots:
      void openElementEditor(bool config=true);

  };

}

#endif
