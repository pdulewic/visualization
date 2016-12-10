#include "inc/settings_window.h"
#include "inc/constants.h"
#include <QComboBox>
#include <QVector>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QSettings>
#include <QLineEdit>
#include <QFileDialog>

#include <QDebug>

QVector<QLocale::Language> supportedLanguages = { QLocale::English,
                                                  QLocale::Polish };

SettingsWindow::SettingsWindow(QWidget *parent): QDialog(parent){
    QSettings settings(COMPANY_NAME, APPLICATION_NAME);
    setWindowTitle(tr("Preferences"));
    QString userLanguage = "";
    if(settings.contains("language"))
        userLanguage = settings.value("language").toString();
    languages = new QComboBox;
    int langIndex = 0;
    for(int i = 0; i != supportedLanguages.size(); ++i){
        QString langName = QLocale(supportedLanguages[i]).bcp47Name();
        if(langName == userLanguage)
            langIndex = i;
        languages->addItem(QIcon(QPixmap(QString(":/icons/%1.png").arg(langName))),
                           QLocale::languageToString(supportedLanguages[i]));
    }
    languages->setCurrentIndex(langIndex);
    workspacePath = new QLineEdit;
    workspacePath->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    if(settings.contains("workspace"))
        lastWorkspace = settings.value("workspace").toString();
    else
        lastWorkspace = DEFAULT_WORKSPACE_PATH;
    workspacePath->setText(lastWorkspace);

    octavePath = new QLineEdit;
    octavePath->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    if(settings.contains("octavePath"))
        octPath = settings.value("octavePath").toString();
    else
        octPath = "";
    octavePath->setText(octPath);

    QFormLayout* controlLayout = new QFormLayout;
    controlLayout->addRow(tr("Language:"),languages);

    QHBoxLayout* workspaceLayout = new QHBoxLayout;
    QPushButton* chooseWorkPath = new QPushButton("...");
    chooseWorkPath->setFixedWidth(CHOOSE_PATH_BUTTON_WIDTH);
    connect(chooseWorkPath,SIGNAL(clicked(bool)),this,SLOT(chooseWorkspaceDirectory()));
    workspaceLayout->addWidget(workspacePath);
    workspaceLayout->addWidget(chooseWorkPath);
    controlLayout->addRow(tr("Workspace directory:"),workspaceLayout);

    QHBoxLayout* octaveLayout = new QHBoxLayout;
    QPushButton* chooseOctPath = new QPushButton("...");
    chooseOctPath->setFixedWidth(CHOOSE_PATH_BUTTON_WIDTH);
    connect(chooseOctPath,SIGNAL(clicked(bool)),this,SLOT(chooseOctavePath()));
    octaveLayout->addWidget(octavePath);
    octaveLayout->addWidget(chooseOctPath);
    controlLayout->addRow(tr("Octave path:"),octaveLayout);

    QPushButton* saveButton = new QPushButton(tr("Save"));
    QPushButton* cancelButton = new QPushButton(tr("Cancel"));
    connect(saveButton,SIGNAL(clicked(bool)),this,SLOT(saveSettings()));
    connect(cancelButton,SIGNAL(clicked(bool)),this,SLOT(close()));

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->addLayout(controlLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
}

void SettingsWindow::saveSettings(){
    QSettings settings(COMPANY_NAME, APPLICATION_NAME);
    settings.setValue("language",QLocale(supportedLanguages[languages->currentIndex()]).bcp47Name());
    if(workspacePath->text() != lastWorkspace){
        lastWorkspace = workspacePath->text();
        settings.setValue("workspace",lastWorkspace);
        emit workspacePathChanged(lastWorkspace);
    }
    if(octavePath->text() != octPath){
        octPath = octavePath->text();
        settings.setValue("octavePath",octPath);
    }
}

void SettingsWindow::chooseWorkspaceDirectory(){
    QString filename = QFileDialog::getExistingDirectory(this,tr("select workspace"),lastWorkspace);
    if(!filename.isEmpty())
        workspacePath->setText(filename);
}

void SettingsWindow::chooseOctavePath(){
    QString filename = QFileDialog::getOpenFileName(this,tr("select octave executable"),octPath,tr("all files (*)"));
    if(!filename.isEmpty())
        octavePath->setText(filename);
}
