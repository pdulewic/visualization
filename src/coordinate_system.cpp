#include "inc/coordinate_system.h"
#include <GL/gl.h>

#include <QDebug>

bool CoordinateSystem::read(QTextStream &in, QString name){
    QString buffer;
    in >> buffer;
    if(buffer != name)
        return true;
    in >> buffer;
    if(buffer != "=")
        return true;
    for(int i=0; i<size; ++i){
        float tmp[3];
        in >> tmp[0] >> tmp[1] >> tmp[2];
        if(name == "z_final")
            zAxis.push_back(QVector3D(tmp[0],tmp[1],tmp[2]));
        else if(name == "x_axis")
            xAxis.push_back(QVector3D(tmp[0],tmp[1],tmp[2]));
        else if(name == "origins")
            origins.push_back(QVector3D(tmp[0],tmp[1],tmp[2]));

    }
    return false;
}


void CoordinateSystem::render(){
    //qDebug() << "coord render";
    glDisable(GL_LIGHTING);
    glLineWidth(6.0);
    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 1.0);
    for(int i=0; i<size; ++i){
        //qDebug() <<zAxis[i];
        glVertex3f(origins[i].x(),origins[i].y(),origins[i].z());
        glVertex3f(zAxis[i].x(),zAxis[i].y(),zAxis[i].z());
    }
    glColor3f(1.0, 0.0, 0.0);
    for(int i=0; i<size; ++i){
        glVertex3f(origins[i].x(),origins[i].y(),origins[i].z());
        glVertex3f(xAxis[i].x(),xAxis[i].y(),xAxis[i].z());
    }

    glEnd();
    glLineWidth(1.0);
    glEnable(GL_LIGHTING);
}


