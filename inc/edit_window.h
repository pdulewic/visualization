#ifndef EDIT_WINDOW
#define EDIT_WINDOW

#include <QDialog>
#include <QList>
#include <QListWidget>
#include "manipulator.h"

class QListWidgetItem;
class QComboBox;
class QDoubleSpinBox;
class QPushButton;

class EditWindow: public QDialog{
    Q_OBJECT

    QList<Element*> temporaryElements;
    QListWidget* list;
    Manipulator* man;

    QComboBox* type;
    QComboBox* material;
    QDoubleSpinBox* rotation;
    QDoubleSpinBox* maxPosAngle;
    QDoubleSpinBox* maxNegAngle;
    QDoubleSpinBox* length;

    QPushButton* remove;

    void listElements();
    void showElement(int index);
public:
    EditWindow(Manipulator* m, QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event);

private slots:
    void moveElementUp();
    void moveElementDown();
    void addElement();
    void removeElement();
    void handleRowChange(int index);
    void handleTypeChange(int index);
    void handleMaterialChangle(int index);
    void handleRotationChange(double val);
    void handleMPAChange(double val);
    void handleMNAChange(double val);
    void handleLengthChange(double val);

    void saveChanges(){ man->copyKinematicChainFrom(temporaryElements); }
};

#endif // EDIT_WINDOW

