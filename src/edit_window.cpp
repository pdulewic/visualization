#include "inc/edit_window.h"
#include "inc/materials.h"
#include "inc/constants.h"
//#include <QListWidget>
#include <QCloseEvent>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>

#include <QDebug>

EditWindow::EditWindow(Manipulator* m, QWidget* parent): QDialog(parent), man(m) {
    setWindowTitle(tr("Manipulator creator"));
    setGeometry(parent->geometry().x()+100,parent->geometry().y()+100,EDIT_WINDOW_WIDTH,EDIT_WINDOW_HEIGHT);

    list = new QListWidget;
    man->copyKinematicChainTo(temporaryElements);
    list->setFixedWidth(ELEMENT_LIST_WIDTH);

    type = new QComboBox;
    for(int i=0; i < NUMBER_OF_ELEMENTS; ++i)
        type->addItem(ElementArray[i].name);
    connect(type,SIGNAL(currentIndexChanged(int)),this,SLOT(handleTypeChange(int)));

    material = new QComboBox;
    for(int i=0; i < Materials::NUMBER; ++i)
        material->addItem(Materials::data[i].getName());
    connect(material,SIGNAL(currentIndexChanged(int)),this,SLOT(handleMaterialChangle(int)));

    rotation = new QDoubleSpinBox;
    rotation->setRange(-180.0, 180.0);
    connect(rotation,SIGNAL(valueChanged(double)),this,SLOT(handleRotationChange(double)));
    maxPosAngle = new QDoubleSpinBox;
    connect(maxPosAngle,SIGNAL(valueChanged(double)),this,SLOT(handleMPAChange(double)));
    maxNegAngle = new QDoubleSpinBox;
    connect(maxNegAngle,SIGNAL(valueChanged(double)),this,SLOT(handleMNAChange(double)));
    length = new QDoubleSpinBox;
    length->setRange(0.01, MAX_LINK_LENGTH);
    connect(length,SIGNAL(valueChanged(double)),this,SLOT(handleLengthChange(double)));

    QGroupBox* properties = new QGroupBox(tr("Properties"));
    QFormLayout* formLayout = new QFormLayout(properties);
    formLayout->addRow(tr("Type:"),type);
    formLayout->addRow(tr("Material:"),material);
    formLayout->addRow(tr("Rotation:"),rotation);
    formLayout->addRow(tr("Max positive angle:"),maxPosAngle);
    formLayout->addRow(tr("Max negative angle:"),maxNegAngle);
    formLayout->addRow(tr("Length:"),length);

    QHBoxLayout* lSaveClose = new QHBoxLayout;
    QPushButton* save = new QPushButton(tr("Save"));
    connect(save,SIGNAL(clicked(bool)),this,SLOT(saveChanges()));
    lSaveClose->addWidget(save);
    QPushButton* close = new QPushButton(tr("Close"));
    connect(close,SIGNAL(clicked(bool)),this,SLOT(close()));
    lSaveClose->addWidget(close);
    QVBoxLayout* rightLayout = new QVBoxLayout;
    //rightLayout->addLayout(formLayout);
    rightLayout->addWidget(properties);

    QWidget* test = new QWidget;
    test->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    rightLayout->addWidget(test);
    rightLayout->addLayout(lSaveClose);

    QVBoxLayout* controlButtons = new QVBoxLayout;
    controlButtons->addStretch();
    QPushButton* moveUp = new QPushButton(tr("Move up"));
    moveUp->setFocusPolicy(Qt::NoFocus);  //temporary solution, before i'll figure out focusing QFormLayout rows
    connect(moveUp,SIGNAL(clicked(bool)),this,SLOT(moveElementUp()));
    controlButtons->addWidget(moveUp);
    QPushButton* moveDown = new QPushButton(tr("Move down"));
    moveDown->setFocusPolicy(Qt::NoFocus);
    connect(moveDown,SIGNAL(clicked(bool)),this,SLOT(moveElementDown()));
    controlButtons->addWidget(moveDown);
    QPushButton* add = new QPushButton(tr("Add element"));
    add->setFocusPolicy(Qt::NoFocus);
    connect(add,SIGNAL(clicked(bool)),this,SLOT(addElement()));
    controlButtons->addWidget(add);
    remove = new QPushButton(tr("Remove element"));
    remove->setFocusPolicy(Qt::NoFocus);
    connect(remove,SIGNAL(clicked(bool)),this,SLOT(removeElement()));
    controlButtons->addWidget(remove);
    controlButtons->addStretch();

    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->addWidget(list);
    mainLayout->addLayout(controlButtons);
    mainLayout->addLayout(rightLayout);
    setLayout(mainLayout);

    connect(list,SIGNAL(currentRowChanged(int)),this,SLOT(handleRowChange(int)));
    listElements();
}

void EditWindow::closeEvent(QCloseEvent *event){
    for(auto x : temporaryElements)
        delete x;
    event->accept();
}

void EditWindow::moveElementUp(){
    int from = list->currentRow();
    int to = from - 1;
    if(from > 0){
        temporaryElements.swap(to,from);
        list->insertItem(to, list->takeItem(from));
        list->setCurrentRow(to);
    }
}


void EditWindow::moveElementDown(){
    int from = list->currentRow();
    int to = from + 1;
    if(to < list->count()){
        temporaryElements.swap(to,from);
        list->insertItem(to, list->takeItem(from));
        list->setCurrentRow(to);
    }
}

void EditWindow::addElement(){
    int row = list->currentRow();
    if(list->count() == 0){
        type->setEnabled(true);
        material->setEnabled(true);
        rotation->setEnabled(true);
        length->setEnabled(true);
        remove->setEnabled(true);
    }
    temporaryElements.insert(row,new Element(ElementType::Link1, 0, 0.0, 0.0, 0.0, 1.0));  //Link1 should be default type for new elements
    int ind = static_cast<int>(ElementType::Link1);
    QListWidgetItem* item = new QListWidgetItem(QIcon(QPixmap(ElementArray[ind].icon)), ElementArray[ind].name);
    list->insertItem(row,item);
    list->setCurrentRow(row);
}

void EditWindow::removeElement(){
    int row = list->currentRow();
    // if no element is selected - nothing happens
    if(row < 0 || row >= list->count())
        return;
    //delete temporaryElements[row];
    if(list->count() == 1){
        type->setEnabled(false);
        material->setEnabled(false);
        rotation->setEnabled(false);
        maxPosAngle->setEnabled(false);
        maxNegAngle->setEnabled(false);
        length->setEnabled(false);
        remove->setEnabled(false);
    }
    else{
        if(row == list->count()-1)
            list->setCurrentRow(row-1);
        else
            list->setCurrentRow(row+1);
    }

    temporaryElements.removeAt(row);
    QListWidgetItem* item = list->takeItem(row);
    delete item;
}


void EditWindow::handleRowChange(int index){
    if(index < 0 || index >= temporaryElements.size())
        return;
    int t = static_cast<int>(temporaryElements[index]->getType());
    type->setCurrentIndex(t);
    material->setCurrentIndex(temporaryElements[index]->getMaterial());
    rotation->setValue(static_cast<double>(temporaryElements[index]->getRotation()));
    if(t < static_cast<int>(ElementType::Link1)){
        maxPosAngle->setValue(static_cast<double>(temporaryElements[index]->getMaxPosAngle()));
        maxNegAngle->setValue(static_cast<double>(temporaryElements[index]->getMaxNegAngle()));
    }
    else if(t == static_cast<int>(ElementType::Link1)){  //ASSUMING THAT THERE IS ONLY ONE WARIABLE LENGTH ELEMENT TYPE!
        length->setValue(static_cast<double>(temporaryElements[index]->getVariableParameter()));
    }
}


void EditWindow::handleTypeChange(int index){
    int row = list->currentRow();
    if((row < 0 || row >= list->count()))
        return;
    if(index < static_cast<int>(ElementType::Link1)){
        maxPosAngle->setEnabled(true);
        maxPosAngle->setRange(0.0, ElementArray[index].positiveAngleLimit);
        maxNegAngle->setEnabled(true);
        maxNegAngle->setRange(0.0, ElementArray[index].negativeAngleLimit);
        length->setValue(0.0);
        length->setEnabled(false);
    }
    else if(index == static_cast<int>(ElementType::Link1)){  //ASSUMING THAT THERE IS ONLY ONE WARIABLE LENGTH ELEMENT TYPE!
        maxPosAngle->setValue(0.0);
        maxPosAngle->setEnabled(false);
        maxNegAngle->setValue(0.0);
        maxNegAngle->setEnabled(false);
        length->setEnabled(true);
        length->setValue(static_cast<double>(temporaryElements[row]->getVariableParameter()));
    }
    else{
        maxPosAngle->setValue(0.0);
        maxPosAngle->setEnabled(false);
        maxNegAngle->setValue(0.0);
        maxNegAngle->setEnabled(false);
        length->setValue(0.0);
        length->setEnabled(false);
    }
    if(temporaryElements[row]->getType() != static_cast<ElementType>(index)){ //row didn't changed
        temporaryElements[row]->setType(static_cast<ElementType>(index));
        temporaryElements[row]->setMPA(ElementArray[index].positiveAngleLimit);
        temporaryElements[row]->setMNA(ElementArray[index].negativeAngleLimit);
        maxPosAngle->setValue(ElementArray[index].positiveAngleLimit);
        maxNegAngle->setValue(ElementArray[index].negativeAngleLimit);
    }
    list->item(row)->setText(ElementArray[index].name);
    list->item(row)->setIcon(QIcon(QPixmap(ElementArray[index].icon)));
}

void EditWindow::handleMaterialChangle(int index){
    int row = list->currentRow();
    if(row < 0 || row >= temporaryElements.size())
        return;
    temporaryElements[row]->setMaterial(index);
}

void EditWindow::handleRotationChange(double val){
    int row = list->currentRow();
    if(row < 0 || row >= temporaryElements.size())
        return;
    temporaryElements[row]->setRotation(static_cast<float>(val));
}

void EditWindow::handleMPAChange(double val){
    int row = list->currentRow();
    if(row < 0 || row >= temporaryElements.size())
        return;
    temporaryElements[row]->setMPA(static_cast<float>(val));
}

void EditWindow::handleMNAChange(double val){
    int row = list->currentRow();
    if(row < 0 || row >= temporaryElements.size())
        return;
    temporaryElements[row]->setMNA(static_cast<float>(val));
}

void EditWindow::handleLengthChange(double val){
    int row = list->currentRow();
    if(row < 0 || row >= temporaryElements.size())
        return;
    temporaryElements[row]->setVariableParameter(static_cast<float>(val));
}


void EditWindow::listElements(){
    if(!temporaryElements.isEmpty()){
        for(const auto &x : temporaryElements){
            int ind = static_cast<int>(x->getType());
            new QListWidgetItem(QIcon(QPixmap(ElementArray[ind].icon)), ElementArray[ind].name, list);
        }
        handleTypeChange(static_cast<int>(temporaryElements.front()->getType()));
        list->setCurrentRow(0);
    }
}

