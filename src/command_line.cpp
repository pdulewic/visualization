#include "inc/command_line.h"
#include "inc/constants.h"
#include <QKeyEvent>

CommandLine::CommandLine(QWidget *parent): QTextEdit(parent){
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
    setFixedHeight(COMMAND_LINE_HEIGHT);
}

void CommandLine::keyPressEvent(QKeyEvent *event){
    if(event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
        emit enterKeyPressed();
    }
    else{
        QTextEdit::keyPressEvent(event);
    }
}
