#include "inc/octave_terminal.h"
#include "inc/constants.h"
#include "inc/command_line.h"
#include <QTextBrowser>
#include <QTextEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QString>
#include <QKeyEvent>

OctaveTerminal::OctaveTerminal(QWidget *parent): QDialog(parent){
    setWindowTitle(tr("Octave terminal"));
    display = new QTextBrowser;
    display->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    commandLine = new CommandLine;
    sendButton = new QPushButton(tr("Send"));
    connect(sendButton,SIGNAL(clicked(bool)),this,SLOT(emitCommand()));
    connect(commandLine,SIGNAL(enterKeyPressed()),this,SLOT(emitCommand()));

    QHBoxLayout* downLayout = new QHBoxLayout;
    downLayout->addWidget(commandLine);
    downLayout->addWidget(sendButton);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(display);
    layout->addLayout(downLayout);
    setLayout(layout);
}

void OctaveTerminal::displayText(const QString &text){
    QString command(text);
    display->append(command.remove('\n'));
}

void OctaveTerminal::emitCommand(){
    QString command = commandLine->toPlainText();
    commandLine->setText("");
    display->append(command);
    command += "\n";
    emit newCommand(command);
}



