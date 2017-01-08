#include "inc/mainwindow.h"
#include "inc/model_manager.h"
#include "inc/settings_window.h"
#include "inc/materials.h"
#include "inc/constants.h"
#include "inc/ik_dialog.h"
#include <QTextEdit>  //tmp
#include <QFileDialog>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QCloseEvent>
#include <QMenuBar>
#include <QMenu>
#include <QSettings>
#include <QApplication>
#include <utility>

#include <QDebug>
#include <GL/gl.h>
#include <QPushButton>

extern QVector<QLocale::Language> supportedLanguages;

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), areUnsavedChanges(false) {

    QSettings settings(COMPANY_NAME, APPLICATION_NAME);
    QString lang;
    if(settings.contains("language"))
        lang = settings.value("language").toString();
    else{
        // if no language is specified in settings, system language is used as default
        lang = QLocale::system().name();
        lang.truncate(lang.lastIndexOf('_'));
    }
    // if language is not supported, english is used as a default
    if(!supportedLanguages.contains(QLocale(lang).language()))
        lang = "en";
    if(!translator.load(QString(":/languages/visualisation_%1.qm").arg(lang)))
        qDebug() << "Error occured when loading translation file";               // this can be removed in final release
    qApp->installTranslator(&translator);

    if(settings.contains("workspace"))
        workspacePath = settings.value("workspace").toString();
    else
        workspacePath = DEFAULT_WORKSPACE_PATH;

    setWindowTitle(tr("Visualisation of the manipulator"));
    initializeElementArray();           //setting element names after loading translator
    Materials::initializeMaterials();   //setting material names after loading translator

    if(settings.contains("octavePath"))
        octavePath = settings.value("octavePath").toString();
    else{
        octavePath = askForOctavePath();
        settings.setValue("octavePath",octavePath);
    }

    octave = new OctaveConnection(octavePath,this); //add reading path from settings
    octave->runOctave();
    octave->waitForStarted();
    connect(&manipulator,SIGNAL(kinematicChainChanged()),this,SLOT(adjustControlPanel()));
    scene = new Scene(&manipulator,octave);
    panel = new ControlPanel;
    panel->connectWithScene(scene);
    settingsWin = new SettingsWindow;

    connect(scene,SIGNAL(newTipPosition(QVector3D)),panel,SLOT(showTipCoordinates(QVector3D)));
    connect(octave,SIGNAL(newInverseKinematics(QQueue<QVector<float> >)),panel,SLOT(animateMovement(QQueue<QVector<float> >)));
    connect(panel,SIGNAL(postureChanged()),scene,SLOT(setPostureChanged()));
    connect(&manipulator,SIGNAL(kinematicChainChanged()),scene,SLOT(calculateDH()));
    connect(&manipulator,SIGNAL(kinematicChainAboutToChange(bool)),scene,SLOT(setPaintingEnabled(bool)));
    connect(settingsWin,SIGNAL(workspacePathChanged(QString)),this,SLOT(setWorkspacePath(QString)));

    QVBoxLayout* rightLayout = new QVBoxLayout;
    rightLayout->addWidget(panel);
    rightLayout->addStretch();

    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->addWidget(scene);
    mainLayout->addLayout(rightLayout);
    QWidget* background = new QWidget;
    background->setLayout(mainLayout);
    setCentralWidget(background);
    scene->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    panel->setFixedWidth(CONTROL_PANEL_WIDTH);
    ModelManager::initialize();

    createMenus();
    adjustSize();
}

void MainWindow::closeEvent(QCloseEvent *event){
    if(secureChanges())
        event->ignore();
    else{
        octave->close();
        event->accept();
    }
}



void MainWindow::createMenus(){
    QMenu* fileMenu = menuBar()->addMenu(tr("File"));

    QAction* newAct = new QAction(tr("New"),this);
    connect(newAct,SIGNAL(triggered(bool)),this,SLOT(newFile()));
    fileMenu->addAction(newAct);

    QAction* openAct = new QAction(tr("Open"),this);
    //openAct->setShortcut(QKeySequence(tr("Ctrl+o")));  //todo: add shotrcut and icon
    connect(openAct,SIGNAL(triggered(bool)),this,SLOT(openFile()));
    fileMenu->addAction(openAct);

    QAction* saveAct = new QAction(tr("Save"),this);
    connect(saveAct,SIGNAL(triggered(bool)),this,SLOT(saveFile()));
    fileMenu->addAction(saveAct);

    QAction* saveAsAct = new QAction(tr("Save as"),this);
    connect(saveAsAct,SIGNAL(triggered(bool)),this,SLOT(saveFileAs()));
    fileMenu->addAction(saveAsAct);

    QMenu* editMenu = menuBar()->addMenu(tr("Edit"));

    QAction* editWindowAct = new QAction(tr("Edit manipulator"),this);
    connect(editWindowAct,SIGNAL(triggered(bool)),this,SLOT(openEditWindow()));
    editMenu->addAction(editWindowAct);

    QAction* settingsWindowAct = new QAction(tr("Preferences"),this);
    connect(settingsWindowAct,SIGNAL(triggered(bool)),this,SLOT(openSettingsWindow()));
    editMenu->addAction(settingsWindowAct);

    QMenu* toolsMenu = menuBar()->addMenu(tr("Tools"));

    QAction* octaveTerminal = new QAction(tr("Octave terminal"),this);
    connect(octaveTerminal,SIGNAL(triggered(bool)),octave,SLOT(openTerminal()));
    toolsMenu->addAction(octaveTerminal);

    QAction* inverseDialog = new QAction(tr("Inverse kinematics"),this);
    connect(inverseDialog,SIGNAL(triggered(bool)),this,SLOT(openIKDIalog()));
    toolsMenu->addAction(inverseDialog);

}

bool MainWindow::secureChanges(){
    if(!areUnsavedChanges)
        return false;
    QMessageBox msgBox;
    msgBox.setText(tr("The manipulator has been modified"));
    msgBox.setInformativeText(tr("Do you want to save your changes?"));
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();
    switch(ret){
      case QMessageBox::Save:
          saveFile();
          break;
      case QMessageBox::Discard:
          areUnsavedChanges = false;
          break;
      case QMessageBox::Cancel:
          return true;
    }
    return false;
}

void MainWindow::saveManipulator(QString filename){
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;
    currentFile = filename;
    manipulator.saveToXML(&file);
    file.close();
}

QString MainWindow::askForOctavePath(){
    QMessageBox msgBox;
    msgBox.setText(tr("Path to the GNU Octave executable file is not specified"));
    msgBox.setInformativeText(tr("Do you want to select it now?"));
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    switch(ret){
      case QMessageBox::Yes:
        return QFileDialog::getOpenFileName(this,tr("select octave executable"),octavePath,tr("all files (*)"));
      case QMessageBox::No:
        break;
    }
    return QString();
}

void MainWindow::newFile(){
    if(secureChanges())
        return;
    currentFile.clear();
    manipulator.createNew();
    areUnsavedChanges = false;
}

void MainWindow::openFile(){
    QString filename = QFileDialog::getOpenFileName(this,tr("Open file"),workspacePath,tr("XML Files (*.xml)"));
    if(filename.isEmpty())
        return;

    if(secureChanges())
        return;

    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly))
        return;
    currentFile = filename;
    manipulator.loadFromXML(&file);
    file.close();
    areUnsavedChanges = false;  //loading new kinematic chain always sets flag up
}

void MainWindow::saveFile(){
    if(currentFile.isEmpty())
        saveFileAs();
    else{
        saveManipulator(currentFile);
        areUnsavedChanges = false;
    }
}


void MainWindow::saveFileAs(){
    QString filename = QFileDialog::getSaveFileName(this,tr("Save file as"),workspacePath,tr("XML Files (*.xml)"));
    if(filename.isEmpty())
        return;
    if(!filename.endsWith(".xml"))  // Append file extension. Only .xml is allowed
        filename.append(".xml");
    saveManipulator(filename);
    areUnsavedChanges = false;
}

void MainWindow::openEditWindow(){
    edit = new EditWindow(&manipulator,this);
    edit->show();
}

void MainWindow::openSettingsWindow(){
    settingsWin->show();
}

void MainWindow::openIKDIalog(){
    inverseKinematicsDialog = new IKDialog(&manipulator,this);
    connect(inverseKinematicsDialog,SIGNAL(execIKCommand(QVector3D,QVector<float>,IKDialog::Model)),octave,SLOT(sendIKRequest(QVector3D,QVector<float>,IKDialog::Model)));
    connect(inverseKinematicsDialog,SIGNAL(newPosition(QVector3D)),scene,SLOT(setTokenPosition(QVector3D)));
    connect(inverseKinematicsDialog,SIGNAL(finished(int)),scene,SLOT(stopDisplayingToken()));
    scene->startDisplayingToken();
    inverseKinematicsDialog->show();
}


void MainWindow::adjustControlPanel(){
    areUnsavedChanges = true;
    panel->connectWithManipulator(manipulator);
}

void MainWindow::setWorkspacePath(QString path){
    workspacePath = std::move(path);
}



