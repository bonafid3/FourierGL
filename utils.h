#ifndef UTILS_H
#define UTILS_H

#include <qmath.h>
#include <QDebug>
#include <QVector3D>

#define qd qDebug()

#define toStr(x) QString::number(x,'T',3)

const QVector3D up(0,1,0);

struct sPlane {
    sPlane(QVector3D _n, QVector3D _p): n(_n), p(_p){}
    QVector3D n; //plane normal
    QVector3D p; //point on plane
};

// 140 = 0.5ms = -90°
// 206 = 1ms = -45°
// 310 = 1.5ms = 0°
// 410 = 2ms = +45°
// 514 = 2.5ms = +90°

#define SERVO_MIN 140
#define SERVO_MAX 514

float d2r(float deg);
float r2d(float rad);

float clamp(float val, float min, float max);

#endif // UTILS_H
