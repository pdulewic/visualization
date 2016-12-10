#ifndef DH_PARAMETERS
#define DH_PARAMETERS

#include <array>
#include <QVector>
#include <QAbstractTableModel>
#include "model_manager.h"

class Scene;

class QTextStream;

class DHParameters: public QAbstractTableModel{
    Q_OBJECT

    QVector<std::array<double, 4>> table;
    QVector<bool> angleTypes;
    ElementType baseType;
    Scene* scene;
    void drawCoordinates(int number);
public:
    DHParameters(QObject* parent = nullptr, Scene* s = nullptr): QAbstractTableModel(parent), baseType(ElementType::Base1), scene(s) {}
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex & parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    virtual int sizeHintForColumn(int column) const;
    void render(QVector<float> angles);
    void setAngleTypes(QVector<bool> types);
    void adjustCoordinateSystem();  // function make OpenGL coordinate system identical to robot coordinate system
    ElementType getBaseType(){ return baseType; }
public slots:
    void setDHParameters(QVector <std::array<double, 4>>& tab, ElementType type);
};

#endif // DH_PARAMETERS

