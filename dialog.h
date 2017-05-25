#ifndef DIALOG_H
#define DIALOG_H

#include <QTimer>
#include <QDialog>
#include <QtOpenGL>
#include <QVector>
#include <QList>

#include "ccircle.h"

namespace Ui {
    class Dialog;
}

struct Complex
{
    Complex():real(0),imag(0) {}
    Complex(float r, float i) : real(r), imag(i) {}
    ~Complex() {}
    float real=0, imag=0;
    float magnitude() { return sqrt(real*real+imag*imag); }
    float phase() { return atan2(imag, real); }
};

class Dialog : public QDialog
{
    Q_OBJECT

public:

    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

    QVector<GLfloat> mVertices;

    QVector<QVector2D> data;

public slots:
    void on_glInitialized();
private slots:

    void on_mTimer_timeout();

    void on_mPerspectiveRadioButton_toggled(bool checked);

    void on_mOrthogonalRadioButton_toggled(bool checked);

    void on_mTopViewPushButton_clicked();

    void on_mLeftViewPushButton_clicked();

    void on_mRightViewPushButton_clicked();

    //void on_mBackwardPushButton_clicked();

    void on_mBackViewPushButton_clicked();

    void on_mFrontViewPushButton_clicked();

    void on_mStartTimerPushButton_clicked();

    void on_mFrameSlider_valueChanged(int value);

    void on_mousePressed(QVector3D pos);
    void on_mouseMove(QVector3D pos);
    void on_mClearPushButton_clicked();

    void on_mRunDFTPushButton_clicked();

    void on_mLoadImagePushButton_clicked();

    void on_mImageOpacitySlider_valueChanged(int value);

    void on_mSampleGroupBox_toggled(bool arg1);

private:
    Ui::Dialog *ui;

    QTimer *mTimer;

    QList<cCircle> mCircles;

    void createFrames();
    void runDFT();
};

#endif // DIALOG_H
