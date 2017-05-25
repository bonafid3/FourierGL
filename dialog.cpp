#include "dialog.h"
#include "ui_dialog.h"

#include "utils.h"

#include <QFileDialog>

#define FREQ_DIV 10.0f

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    mTimer = new QTimer(this);
    mTimer->setObjectName("mTimer");
    mTimer->setInterval(20);

    ui->setupUi(this);

    ui->splitter->setStretchFactor(0, 10);
    ui->splitter->setStretchFactor(1, 1);

    qsrand(QTime::currentTime().msec());

    ui->mGLWidget->setFocus();

    QStandardItemModel *model = new QStandardItemModel(this);
    QStringList hl; hl << "X coord" << "Y coord";
    model->setHorizontalHeaderLabels(hl);
    ui->mTableView->verticalHeader()->hide();
    ui->mTableView->setModel(model);

    model->setItem(0,0, new QStandardItem("0"));
    model->setItem(0,1, new QStandardItem("0"));


    connect(ui->mGLWidget, SIGNAL(glInitialized()), this, SLOT(on_glInitialized()));
    connect(ui->mGLWidget, SIGNAL(mouseMove(QVector3D)), this, SLOT(on_mouseMove(QVector3D)));
    connect(ui->mGLWidget, SIGNAL(mousePressed(QVector3D)), this, SLOT(on_mousePressed(QVector3D)));

    data.append(QVector2D(25,-5));
    data.append(QVector2D( 15, 15));
    data.append(QVector2D( 0,15));
    data.append(QVector2D( -20,15));
    data.append(QVector2D( -15,0));
    data.append(QVector2D( -15,-15));
    data.append(QVector2D( 0,-15));
    data.append(QVector2D( 15,-15));

    runDFT();
    ui->mGLWidget->update();
}


void Dialog::runDFT()
{
    const float N = data.size();

    if(N > 0) {
        QVector<Complex> cnum;
        cnum.resize(N);

        for(int k=0; k<N; k++) {
            for (int n=0; n<N; n++) {
                float angle = -2 * M_PI * k * n / N;
                cnum[k].real += data[n].x() * cos(angle) + data[n].y() * sin(angle);
                cnum[k].imag += data[n].y() * cos(angle) - data[n].x() * sin(angle);
            }

            cnum[k].real *= 1/N;
            cnum[k].imag *= 1/N;

            qd << k << cnum[k].magnitude() << cnum[k].phase();
        }

        int k=0;
        Complex num = cnum.takeFirst();
        mCircles.append(cCircle(num.magnitude(), num.phase(), k++));
        while(cnum.size()) {
            num = cnum.takeLast();
            qd << "F:" << k << num.magnitude() << num.phase();
            mCircles.append(cCircle(num.magnitude(), num.phase(), k/FREQ_DIV));

            if(cnum.size()) {
                num = cnum.takeFirst();
                qd << "F:" << -k << num.magnitude() << num.phase();
                mCircles.append(cCircle(num.magnitude(), num.phase(), -k/FREQ_DIV));
            }
            k++;
        }
    }
}

void Dialog::createFrames()
{
    QVector<GLfloat> outlineGL;

    int frame=0; // vbo index
    for(int tick=0; tick<=1000*FREQ_DIV; tick+=20) {
        QVector<GLfloat> v;
        QVector2D pos(0,0);
        int i;
        for(i=0; i<mCircles.size(); i++) {
            pos = mCircles[i].tick(i, tick, pos);
            v.append(mCircles[i].mVertices);
            mCircles[i].mVertices.clear();
        }

        outlineGL.append(pos.x());
        outlineGL.append(pos.y());
        outlineGL.append(i / 2 + 0.1);

        ui->mGLWidget->createVBO(v); // creates a new vbo with the current frame
        ui->mGLWidget->createOutlineVBO(outlineGL);

        frame++;
    }

    ui->mFrameSlider->setMinimum(0);
    ui->mFrameSlider->setMaximum(frame-1);
}

void Dialog::on_mousePressed(QVector3D pos)
{
    qd << "POS" << pos << data.size();
    data.append(QVector2D(pos.x(), pos.y()));

    QStandardItemModel *model = static_cast<QStandardItemModel*>(ui->mTableView->model());

    int i = model->rowCount();
    model->setItem(i, 0, new QStandardItem(toStr(pos.x())));
    model->setItem(i, 1, new QStandardItem(toStr(pos.y())));

    ui->mTableView->resizeRowsToContents();
    ui->mTableView->scrollToBottom();
}

void Dialog::on_mouseMove(QVector3D pos)
{
    QStandardItemModel *model = static_cast<QStandardItemModel*>(ui->mTableView->model());
    if(model->rowCount() > 0) {
        int i = model->rowCount() - 1;

        model->setData(model->index(i, 0),QString::number(pos.x()),Qt::DisplayRole);
        model->setData(model->index(i, 1),QString::number(pos.y()),Qt::DisplayRole);

        ui->mTableView->resizeRowsToContents();
    }
}

void Dialog::on_glInitialized()
{
    createFrames();
    ui->mGLWidget->update();
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_mPerspectiveRadioButton_toggled(bool perspectiveProjection)
{
    ui->mGLWidget->updateProjection(perspectiveProjection);
    ui->mGLWidget->update();
}

void Dialog::on_mOrthogonalRadioButton_toggled(bool orthogonalProjection)
{
    ui->mGLWidget->updateProjection(!orthogonalProjection);
    ui->mGLWidget->update();
}

void Dialog::on_mTopViewPushButton_clicked()
{
    ui->mGLWidget->topView();
}

void Dialog::on_mLeftViewPushButton_clicked()
{
    ui->mGLWidget->leftView();
}

void Dialog::on_mRightViewPushButton_clicked()
{
    ui->mGLWidget->rightView();
}

void Dialog::on_mBackViewPushButton_clicked()
{
    ui->mGLWidget->backView();
}

void Dialog::on_mFrontViewPushButton_clicked()
{
    ui->mGLWidget->frontView();
}

void Dialog::on_mStartTimerPushButton_clicked()
{
    if(mTimer->isActive())
    {
        mTimer->stop();
        ui->mStartTimerPushButton->setText("Start timer");
    }
    else
    {
        ui->mGLWidget->mCurrFrame=0;
        mTimer->start();
        ui->mStartTimerPushButton->setText("Stop timer");
    }
}

void Dialog::on_mTimer_timeout()
{
    ui->mFrameSlider->setValue(ui->mGLWidget->mCurrFrame+1);
    ui->mGLWidget->update();

}

void Dialog::on_mFrameSlider_valueChanged(int value)
{
    ui->mGLWidget->mCurrFrame = value;
    ui->mGLWidget->update();
}

void Dialog::on_mClearPushButton_clicked()
{
    QStandardItemModel *model = static_cast<QStandardItemModel*>(ui->mTableView->model());

    model->removeRows(1, model->rowCount()-1);


    data.clear();
    mCircles.clear();
    ui->mGLWidget->destroyVBOs();
}

void Dialog::on_mRunDFTPushButton_clicked()
{
    runDFT();
    createFrames();
    ui->mGLWidget->update();
}

void Dialog::on_mLoadImagePushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(0,
                                                    tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
    QImage image(fileName);
    ui->mGLWidget->createTexture(image);
}

void Dialog::on_mImageOpacitySlider_valueChanged(int value)
{
    ui->mGLWidget->mOpacity = value;
    ui->mGLWidget->update();
}

void Dialog::on_mSampleGroupBox_toggled(bool arg1)
{
    ui->mGLWidget->setSamplingMode(arg1);
}
