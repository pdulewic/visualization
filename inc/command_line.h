#ifndef COMMAND_LINE
#define COMMAND_LINE

#include <QTextEdit>

class CommandLine: public QTextEdit{
    Q_OBJECT
public:
    CommandLine(QWidget* parent = nullptr);
protected:
    void keyPressEvent(QKeyEvent * event);
signals:
    void enterKeyPressed();
};

#endif // COMMAND_LINE

