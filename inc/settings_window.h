#ifndef SETTINGS_WINDOW
#define SETTINGS_WINDOW

#include <QDialog>
#include <QString>
#include <QLocale>

class QComboBox;
class QLineEdit;

class SettingsWindow: public QDialog{
    Q_OBJECT

    QComboBox* languages;
    QLineEdit* workspacePath;
    QLineEdit* octavePath;
    QString lastWorkspace;
    QString octPath;
public:
    SettingsWindow(QWidget* parent = nullptr);
private slots:
    void saveSettings();
    void chooseWorkspaceDirectory();
    void chooseOctavePath();
signals:
    void workspacePathChanged(QString path);
};

#endif // SETTINGS_WINDOW

