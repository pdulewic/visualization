#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTranslator>

#include "scene.h"
#include "manipulator.h"
#include "control_panel.h"
#include "edit_window.h"
#include "octave_connection.h"

class SettingsWindow;
class IKDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    Scene* scene;
    ControlPanel* panel;
    EditWindow* edit;
    IKDialog* inverseKinematicsDialog;
    Manipulator manipulator;
    OctaveConnection* octave;
    SettingsWindow* settingsWin;
    QString currentFile;
    QString workspacePath;
    QString octavePath;
    bool areUnsavedChanges;

    QTranslator translator;

    void createMenus();
    /*!
     * \brief Prevent closing unsaved file
     *
     * If any unsaved changes exists, function
     * ask user what to do. It handles saving file,
     * but if user want to cancel current operation,
     * control is passed to the calling function.
     * \retval true - user want to cancel operation
     * \retval false - changes are safe
     */
    bool secureChanges();
    void saveManipulator(QString filename);
    QString askForOctavePath();

public:
    MainWindow(QWidget *parent = 0);

protected:
    void closeEvent(QCloseEvent* event);

private slots:
    void newFile();
    void openFile();
    void saveFile();
    void saveFileAs();
    void openEditWindow();
    void openSettingsWindow();
    void openIKDIalog();
    void adjustControlPanel();
    void setWorkspacePath(QString path);
};

#endif // MAINWINDOW_H
