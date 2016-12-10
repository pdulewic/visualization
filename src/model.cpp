#include "inc/model.h"
#include <QFile>
#include <QString>
#include <QTextStream>
#include <GL/gl.h>

using namespace std;

bool Model::loadObj(QString path){
    vertices.clear();
    normals.clear();
    faces.clear();  /* clean after previous models */
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
        return true;
    QByteArray line;
    float x,y,z;
    QString c;
    char slashes[2];
    while(!file.atEnd()){
        line = file.readLine();
        QTextStream stream(line);
        stream >> c;
        if(c == "v"){
            stream >> x >> y >> z;
            QVector3D tmp(x , y , z);
            vertices.push_back(tmp);
        }
        else if(c == "vn"){
            stream >> x >> y >> z;
            QVector3D tmp(x , y , z);
            normals.push_back(tmp);

        }
        else if(c == "f"){
            Face tmp;
            stream >> tmp.v[0] >> slashes[0] >> slashes[1] >> tmp.normalID >> tmp.v[1] >> c >> tmp.v[2];
            faces.push_back(tmp);
        }
    }
    file.close();
    return false;
}

void Model::render(){
    glBegin(GL_TRIANGLES);
    for(auto x : faces){
        glNormal3f(normals[x.normalID-1].x(),normals[x.normalID-1].y(),normals[x.normalID-1].z());
        for(int i=0; i < VERTICES_IN_POLYGON; ++i)
            glVertex3f(vertices[x.v[i]-1].x(),vertices[x.v[i]-1].y(),vertices[x.v[i]-1].z());
    }
    glEnd();
}

void Model::renderWithCustomLenght(float length){
    glBegin(GL_TRIANGLES);
    for(auto x : faces){
        glNormal3f(normals[x.normalID-1].x(),normals[x.normalID-1].y(),normals[x.normalID-1].z());
        for(int i=0; i < VERTICES_IN_POLYGON; ++i){
            float tmp = vertices[x.v[i]-1].y();
            if(tmp != 0.0)
                tmp += length;
            glVertex3f(vertices[x.v[i]-1].x(),tmp ,vertices[x.v[i]-1].z());
        }
    }
    glEnd();
}
