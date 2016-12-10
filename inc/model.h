#ifndef MODEL_H
#define MODEL_H

#include <QString>
#include <QVector>
#include <QVector3D>
#include "constants.h"

struct Face{
    int v[VERTICES_IN_POLYGON];
    int normalID;
};

class Model{
    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;
    QVector<Face> faces;
public:
    Model(QString path){loadObj(path);}
    bool loadObj(QString path);
    void render();
    void renderWithCustomLenght(float length);
};

#endif // MODEL_H
