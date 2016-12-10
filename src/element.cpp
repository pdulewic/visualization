#include "inc/element.h"
#include "inc/materials.h"
#include "inc/model_manager.h"
#include "inc/octave_connection.h"

#include <QDebug>

Element::Element(ElementType t, int m, float r, float mpa, float mna, float v): type(t), material(m), rotation(r),
    maxPositiveAngle(mpa), maxNegativeAngle(mna), variableParameter(v) {}

void Element::render() const{
    glRotatef(rotation, 0.0f, 1.0f, 0.0f);
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, Materials::data[material].getAmbient() );
    glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, Materials::data[material].getDiffuse() );
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, Materials::data[material].getSpecular() );
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, Materials::data[material].getShininess() );
    ModelManager::renderModel(type,variableParameter);
}

void Element::calculateZAxis(OctaveConnection *octave, QVector<QVector3D> &axis) const{
    GLfloat partialKinematics[16];
    glRotatef(rotation, 0.0f, 1.0f, 0.0f);
    switch(type){
    case ElementType::Base1:
        glGetFloatv(GL_MODELVIEW_MATRIX, partialKinematics);
        octave->loadMatrix(partialKinematics,OctaveConnection::PartialKinematics);
        axis.push_back(QVector3D(1.0, 0.0, 0.0));
        glTranslatef(0.0, 1.155, 0.0);
        break;
    case ElementType::Base2:
        glGetFloatv(GL_MODELVIEW_MATRIX, partialKinematics);
        octave->loadMatrix(partialKinematics,OctaveConnection::PartialKinematics);
        axis.push_back(QVector3D(0.0, 1.0, 0.0));
        glTranslatef(0.0, 2.61, 0.0);
        break;
    case ElementType::Joint1:
        glTranslatef(0.0, 0.63987, 0.0);
        axis.push_back(QVector3D(1.0, 0.0, 0.0));
        glGetFloatv(GL_MODELVIEW_MATRIX, partialKinematics);
        octave->loadMatrix(partialKinematics,OctaveConnection::PartialKinematics);
        glTranslatef(0.0, 1.155, 0.0);
        break;
    case ElementType::Joint2:
        glTranslatef(0.0, 0.55, 0.0);
        axis.push_back(QVector3D(0.0, 1.0, 0.0));
        glGetFloatv(GL_MODELVIEW_MATRIX, partialKinematics);
        octave->loadMatrix(partialKinematics,OctaveConnection::PartialKinematics);
        glTranslatef(0.0, 0.5, 0.0);
        break;
    case ElementType::Joint3:
        glTranslatef(0.0, 0.85, 0.0);
        axis.push_back(QVector3D(0.0, 1.0, 0.0));
        glGetFloatv(GL_MODELVIEW_MATRIX, partialKinematics);
        octave->loadMatrix(partialKinematics,OctaveConnection::PartialKinematics);
        glTranslatef(0.0, 0.8, 0.0);
        break;
    case ElementType::Joint4:
        glTranslatef(0.0, 1.0, 0.0);
        glRotatef(-90, 0.0, 0.0, 1.0);
        axis.push_back(QVector3D(0.0, 1.0, 0.0));
        glGetFloatv(GL_MODELVIEW_MATRIX, partialKinematics);
        octave->loadMatrix(partialKinematics,OctaveConnection::PartialKinematics);
        glTranslatef(0.0, 0.55, 0.0);
        break;
    case ElementType::Link1:
        glTranslatef(0.0, variableParameter, 0.0);
        break;
    case ElementType::Link2:
        glTranslatef(0.0, 1.0, 0.0);
        glRotatef(-90, 1.0, 0.0, 0.0);
        glTranslatef(0.0, 1.0, 0.0);
        break;
    case ElementType::Tip:
        glTranslatef(0.0, 1.5, 0.0);
        glGetFloatv(GL_MODELVIEW_MATRIX, partialKinematics);
        octave->loadMatrix(partialKinematics,OctaveConnection::PartialKinematics);
        break;
    case ElementType::xAxis:
    case ElementType::zAxis:
    case ElementType::Token:
        break;
    }
}

bool Element::isMovable() const{
    if(static_cast<int>(type) < static_cast<int>(ElementType::Link1))
        return true;
    return false;
}
