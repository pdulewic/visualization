#include "inc/manipulator.h"
#include "inc/control_panel.h"
#include <QDomDocument>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QTextStream>
#include <QLabel>
#include "inc/angle_spinbox.h"

#include <QDebug>
#include <QObject>

void Manipulator::eraseKinematicChain(){
    emit kinematicChainAboutToChange(false);
    for(auto x : kinematicChain)
        delete x;
    kinematicChain.clear();
}

int Manipulator::countMovableElements() const{
    int counter = 0;
    for(const auto &x : kinematicChain){
        if(x->isMovable())
            counter++;
    }
    return counter;
}

void Manipulator::render() const{
    QListIterator<Element*> it(kinematicChain);
    it.toBack();
    while(it.hasPrevious())
        it.previous()->render();
}

void Manipulator::calculateZ(OctaveConnection *octave, QVector<QVector3D> &axis) const{
    QListIterator<Element*> it(kinematicChain);
    it.toBack();
    while(it.hasPrevious())
        it.previous()->calculateZAxis(octave,axis);
}

void Manipulator::createControlSliders(QVBoxLayout *layout, QVector<QSlider *> &vec, const ControlPanel* panel) const{
    QSlider* tmp;
    AngleSpinBox* spin;
    QFont font;
    font.setStyle(QFont::StyleItalic);
    font.setBold(true);
    QLabel* lab;
    int jointNumber = countMovableElements();
    for(const auto &elem : kinematicChain){
        if(elem->isMovable()){
            tmp = new QSlider(Qt::Horizontal);
            spin = new AngleSpinBox;
            int mna = static_cast<int>(elem->getMaxNegAngle());
            int mpa = static_cast<int>(elem->getMaxPosAngle());
            tmp->setRange(-mna * SLIDER_SENSITIVITY, mpa * SLIDER_SENSITIVITY);
            spin->setRange(-1.0 * elem->getMaxNegAngle(), elem->getMaxPosAngle());
            connect(tmp,SIGNAL(valueChanged(int)),spin,SLOT(setValue(int)));
            connect(spin,SIGNAL(valueChanged(int)),tmp,SLOT(setValue(int)));
            connect(tmp,SIGNAL(valueChanged(int)),elem,SLOT(setAngle(int)));
            connect(tmp,SIGNAL(valueChanged(int)),panel,SIGNAL(postureChanged()));
            vec.push_back(tmp);
            QHBoxLayout* lineLayout = new QHBoxLayout;
            lab = new QLabel(QString("q%1").arg(jointNumber));
            lab->setFont(font);
            lineLayout->addWidget(lab);
            lineLayout->addWidget(spin);
            lineLayout->addWidget(new QLabel(QString::number(-1*mna)));
            lineLayout->addWidget(tmp);
            lineLayout->addWidget(new QLabel(QString::number(mpa)));
            layout->addLayout(lineLayout);
            --jointNumber;
        }
    }
}

void Manipulator::copyKinematicChainTo(QList<Element *> &copy) const{
    for(const auto &x : kinematicChain)
        copy.push_back(new Element(x->getType(),x->getMaterial(),x->getRotation(),x->getMaxPosAngle(),
                                   x->getMaxNegAngle(), x->getVariableParameter()));
}

void Manipulator::copyKinematicChainFrom(const QList<Element *> &source){
    eraseKinematicChain();
    for(const auto &x : source){
        ElementType type = x->getType();
        float variableParameter = 0.0;
        if(type == ElementType::Link1)
            variableParameter = x->getVariableParameter();
        kinematicChain.push_back(new Element(type,x->getMaterial(),x->getRotation(),x->getMaxPosAngle(),
                                x->getMaxNegAngle(), variableParameter));
    }
    emit kinematicChainChanged();
}


bool Manipulator::loadFromXML(QFile *file){
    eraseKinematicChain();
    QDomDocument doc;
    if(!doc.setContent(file))
        return true;
    QDomNodeList elements = doc.elementsByTagName("element");
    for (int i = 0; i < elements.size(); i++) {
        QDomNode n = elements.item(i);
        QDomElement rotation = n.firstChildElement("rotation");
        QDomElement node = n.toElement();
        int type = node.attribute("type").toInt();
        if(type < static_cast<int>(ElementType::Link1)){
            QDomElement rangeP = n.firstChildElement("maxPositiveAngle");
            QDomElement rangeN = n.firstChildElement("maxNegativeAngle");
            kinematicChain.push_back(new Element(static_cast<ElementType>(type), node.attribute("material").toInt(),
                                            rotation.text().toFloat(), rangeP.text().toFloat(), rangeN.text().toFloat() ));
        }
        else if(type == static_cast<int>(ElementType::Link1)){  //ASSUMING THAT THERE IS ONLY ONE WARIABLE LENGTH ELEMENT TYPE!
            QDomElement length = n.firstChildElement("length");
            kinematicChain.push_back(new Element(static_cast<ElementType>(type), node.attribute("material").toInt(),
                                            rotation.text().toFloat(), 0, 0, length.text().toFloat() ));
        }
        else
            kinematicChain.push_back(new Element(static_cast<ElementType>(type), node.attribute("material").toInt(),
                                            rotation.text().toFloat() ));
    }
    emit kinematicChainChanged();
    return false;
}

void Manipulator::saveToXML(QFile *file){
    QDomDocument doc;
    QDomElement elements = doc.createElement("elements");
    for(const auto& x : kinematicChain){
        QDomElement element = doc.createElement("element");
        int type = static_cast<int>(x->getType());
        element.setAttribute("type",type);
        element.setAttribute("material",x->getMaterial());
        QDomElement rotation = doc.createElement("rotation");
        QDomText rotValue = doc.createTextNode(QString::number(static_cast<double>(x->getRotation()),'f',2));
        rotation.appendChild(rotValue);
        element.appendChild(rotation);
        if(type < static_cast<int>(ElementType::Link1)){
            QDomElement maxPositiveAngle = doc.createElement("maxPositiveAngle");
            QDomText mpaValue = doc.createTextNode(QString::number(static_cast<double>(x->getMaxPosAngle()),'f',2));
            maxPositiveAngle.appendChild(mpaValue);
            element.appendChild(maxPositiveAngle);
            QDomElement maxNegativeAngle = doc.createElement("maxNegativeAngle");
            QDomText mnaValue = doc.createTextNode(QString::number(static_cast<double>(x->getMaxNegAngle()),'f',2));
            maxNegativeAngle.appendChild(mnaValue);
            element.appendChild(maxNegativeAngle);
        }
        else if(type == static_cast<int>(ElementType::Link1)){  //ASSUMING THAT THERE IS ONLY ONE WARIABLE LENGTH ELEMENT TYPE!
            QDomElement length = doc.createElement("length");
            QDomText lengthValue = doc.createTextNode(QString::number(static_cast<double>(x->getVariableParameter()),'f',2));
            length.appendChild(lengthValue);
            element.appendChild(length);
        }
        elements.appendChild(element);
    }
    doc.appendChild(elements);
    QTextStream out(file);
    out << doc.toByteArray();
}

void Manipulator::createNew(){
    eraseKinematicChain();
    emit kinematicChainChanged();
}

QVector<float> Manipulator::getAngles() const{
    QVector<float> angles;
    QListIterator<Element*> it(kinematicChain);
    it.toBack();
    while(it.hasPrevious()){
        auto x = it.previous();
        if(x->isMovable())
            angles.push_back(x->getVariableParameter());
    }
    return angles;
}

QVector<bool> Manipulator::getAngleTypes() const{
    QVector<bool> types;
    QListIterator<Element*> it(kinematicChain);
    it.toBack();
    while(it.hasPrevious()){
        auto x = it.previous();
        if(x->isMovable())
            types.push_back(x->getType() == ElementType::Joint3);
    }
    return types;
}
