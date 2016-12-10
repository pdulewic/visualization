#include "inc/dh_parameters.h"
#include "inc/scene.h"
#include <GL/gl.h>
#include <utility>
#include <cmath>

#include <QDebug>

void DHParameters::drawCoordinates(int number){
    glDisable(GL_LIGHTING);
    glColor3f(0.0, 0.0, 1.0);
    ModelManager::renderModel(ElementType::zAxis);
    glColor3f(1.0, 0.0, 0.0);
    ModelManager::renderModel(ElementType::xAxis);
    scene->renderText(7.0,0.5,0.5,QString("X%1").arg(number));
    glColor3f(0.0, 0.0, 1.0);
    scene->renderText(0.5,0.5,7.0,QString("Z%1").arg(number));
    glEnable(GL_LIGHTING);
}

int DHParameters::rowCount(const QModelIndex &) const{
    return table.size();
}

int DHParameters::columnCount(const QModelIndex &) const{
    return 4;
}

QVariant DHParameters::data(const QModelIndex &index, int role) const{
    if(role == Qt::DisplayRole){
        int row = index.row();
        int column = index.column();
        QString display = QString::number(table[row][column]);
        if(angleTypes.size()-1 >= row){
            if( (angleTypes[row] && column == 1) || (!angleTypes[row] && column == 0) ){
                if(std::abs(table[row][column]) < 0.0001)  // in case of numerical errors
                    display = QString("q%1").arg(row+1);
                else
                    display.append(QString(" + q%1").arg(row+1));
            }
        }
        return QVariant(display);
    }
    return QVariant();
}

QVariant DHParameters::headerData(int section, Qt::Orientation orientation, int role) const{
    if(role == Qt::DisplayRole){
        switch(orientation){
        case Qt::Horizontal:
            switch(section){
            case 0:
                return QVariant(static_cast<QChar>(0x03B8));
                break;
            case 1:
                return QVariant("d");
                break;
            case 2:
                return QVariant("a");
                break;
            case 3:
                return QVariant(static_cast<QChar>(0x03B1));
                break;
            }
            break;
        case Qt::Vertical:
            return QVariant(QString::number(section+1));
        }
    }
    return QVariant();
}

int DHParameters::sizeHintForColumn(int column) const{
    if(column == 0 || column == 3)
        return 60;
    return 90;
}



void DHParameters::render(QVector<float> angles){
    drawCoordinates(0);
    for(int i=0; i < table.size(); ++i){
        std::array<double, 4> tmp = table[i];
        if(!angles.isEmpty() && !angleTypes.isEmpty()){
            if(angleTypes[i])
                tmp[1] += static_cast<double>(angles[i]);
            else
                tmp[0] += static_cast<double>(angles[i]);
        }
        glRotated(tmp[0], 0.0, 0.0, 1.0);
        glTranslated(tmp[2], 0.0, tmp[1]);
        glRotated(tmp[3], 1.0, 0.0, 0.0);
        drawCoordinates(i+1);
    }
}

void DHParameters::setAngleTypes(QVector<bool> types){
    angleTypes = std::move(types);
}

void DHParameters::adjustCoordinateSystem(){
    if(baseType == ElementType::Base1){
        glRotatef(90, 0.0, 0.0, 1.0);
        glRotatef(90, 1.0, 0.0, 0.0);
    }
    else if(baseType == ElementType::Base2)
        glRotatef(-90, 1.0, 0.0, 0.0);
}

void DHParameters::setDHParameters(QVector<std::array<double, 4> > &tab, ElementType type){
    beginRemoveRows(QModelIndex(),0,table.size()-1);
    table.clear();
    endRemoveRows();
    beginInsertRows(QModelIndex(),0,tab.size()-1);
    table = std::move(tab);
    endInsertRows();
    baseType = type;
    scene->setPaintingEnabled(true);
}
