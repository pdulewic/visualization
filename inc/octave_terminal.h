#ifndef OCTAVE_TERMINAL
#define OCTAVE_TERMINAL

#include <QDialog>

class QTextBrowser;
class QTextEdit;
class QPushButton;
class QString;
class CommandLine;

class OctaveTerminal: public QDialog{
    Q_OBJECT

    CommandLine* commandLine;
    QTextBrowser* display;
    QPushButton* sendButton;
public:
    OctaveTerminal(QWidget* parent = nullptr);
public slots:
    void displayText(const QString &text);
private slots:
    void emitCommand();
signals:
    void newCommand(const QString &command);

};

#endif // OCTAVE_TERMINAL



