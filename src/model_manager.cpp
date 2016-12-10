#include "inc/model_manager.h"
#include <GL/gl.h>

#include <QDebug>

void ModelManager::initialize(){
    models.push_back(new Model(":/models/base1p1.obj"));  //0
    models.push_back(new Model(":/models/base1p2.obj"));  //1
    models.push_back(new Model(":/models/joint1.obj"));   //2
    models.push_back(new Model(":/models/tip.obj"));      //3
    models.push_back(new Model(":/models/link1.obj"));    //4
    models.push_back(new Model(":/models/joint2p1.obj")); //5
    models.push_back(new Model(":/models/joint2p2.obj")); //6
    models.push_back(new Model(":/models/link2.obj"));    //7
    models.push_back(new Model(":/models/base2p1.obj"));  //8
    models.push_back(new Model(":/models/joint3p1.obj")); //9
    models.push_back(new Model(":/models/joint3p2.obj")); //10
    models.push_back(new Model(":/models/joint4p1.obj")); //11
    models.push_back(new Model(":/models/xAxis.obj"));    //12
    models.push_back(new Model(":/models/zAxis.obj"));    //13
    models.push_back(new Model(":/models/token.obj"));    //14
}

void ModelManager::adjustCoordinateSystem(ElementType type){
    switch(type){
    case ElementType::Base1:
        glRotatef(90, 0.0, 1.0, 0.0);
        glRotatef(90, 0.0, 0.0, 1.0);
        break;
    case ElementType::Base2:
        glRotatef(-90, 1.0, 0.0, 0.0);
        break;
    case ElementType::Joint1:
    case ElementType::Joint2:
    case ElementType::Joint3:
    case ElementType::Joint4:
    case ElementType::Link1:
    case ElementType::Link2:
    case ElementType::Tip:
    case ElementType::xAxis:
    case ElementType::zAxis:
    case ElementType::Token:
        qDebug() << "Invaild element type used as a base";
    }
}

bool ModelManager::renderModel(ElementType type, float variableParameter){
    switch(type){
    case ElementType::Base1:
        models[0]->render();
        glRotatef(variableParameter, 1.0, 0.0, 0.0);
        models[1]->render();
        glTranslatef(0.0, 1.155, 0.0);
        break;
    case ElementType::Base2:
        models[8]->render();
        glTranslatef(0.0, 2.11, 0.0);
        glRotatef(variableParameter, 0.0, 1.0, 0.0);
        models[6]->render();
        glTranslatef(0.0, 0.5, 0.0);
        break;
    case ElementType::Joint1:
        models[2]->render();
        glTranslatef(0.0, 0.63987, 0.0);
        glRotatef(variableParameter, 1.0, 0.0, 0.0);
        models[1]->render();
        glTranslatef(0.0, 1.155, 0.0);
        break;
    case ElementType::Joint2:
        models[5]->render();
        glTranslatef(0.0, 0.55, 0.0);
        glRotatef(variableParameter, 0.0, 1.0, 0.0);
        models[6]->render();
        glTranslatef(0.0, 0.5, 0.0);
        break;
    case ElementType::Joint3:
        models[9]->render();
        glTranslatef(0.0, 0.85, 0.0);
        models[10]->renderWithCustomLenght(variableParameter);
        glTranslatef(0.0, variableParameter + 0.8, 0.0);
        break;
    case ElementType::Joint4:
        models[11]->render();
        glTranslatef(0.0, 1.0, 0.0);
        glRotatef(-90, 0.0, 0.0, 1.0);
        glTranslatef(0.0, 0.05, 0.0);
        glRotatef(variableParameter, 0.0, 1.0, 0.0);
        models[6]->render();
        glTranslatef(0.0, 0.5, 0.0);
        break;
    case ElementType::Link1:
        models[4]->renderWithCustomLenght(variableParameter);
        glTranslatef(0.0, variableParameter, 0.0);
        break;
    case ElementType::Link2:
        models[7]->render();
        glTranslatef(0.0, 1.0, 0.0);
        glRotatef(-90, 1.0, 0.0, 0.0);
        glTranslatef(0.0, 1.0, 0.0);
        break;
    case ElementType::Tip:
        models[3]->render();
        glTranslatef(0.0, 1.5, 0.0);
        break;
    case ElementType::xAxis:
        models[12]->render();
        break;
    case ElementType::zAxis:
        models[13]->render();
        break;
    case ElementType::Token:
        models[14]->render();
        break;
    }
    return false;
}

