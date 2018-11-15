#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QtCharts/QChart"
#include "QLineSeries"
#include "QValueAxis"
#include "QTimer"
#include "QTime"
#include "QList"
#include "qmath.h"
#include "QPointF"
#include "QDebug"

//#include <QtCharts/QChartGlobal>
#include "QChartView"
QT_CHARTS_USE_NAMESPACE
QChart *m_chart;
QLineSeries *m_series;
//QList<double> dataList;//存储业务数据
int maxSize = 5000;

//QTimer updateTimer;
int timeId;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),//默认初始化？
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_chart = new QChart;
    QChartView *chartView = new QChartView(m_chart);
//    v.setRubberBand(QChartView::HorizontalRubberBand);
    chartView->setRubberBand(QChartView::RectangleRubberBand);
//    chartView->setRubberBand();

    m_series = new QLineSeries;
    m_chart->addSeries(m_series);

    for(int i=0;i<maxSize;++i){
       m_series->append(i,0);
    }
    m_series->setUseOpenGL(true);//openGl 加速
    qDebug()<<m_series->useOpenGL();

    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0,maxSize);
    axisX->setLabelFormat("%g");
    axisX->setTitleText("axisX");

    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(-1.5,1.5);
    axisY->setTitleText("axisY");

    m_chart->setAxisX(axisX,m_series);
    m_chart->setAxisY(axisY,m_series);
    m_chart->legend()->hide();
    m_chart->setTitle("demo");

    QVBoxLayout *layout = ui->verticalLayout;
    layout->addWidget(chartView);

//    connect(&updateTimer,SIGNAL(timeout()),this,SLOT(updatedataSlot()));
//    updateTimer.start(0);
    timeId = startTimer(30);
}


double MainWindow::getData(double time){

    double s = qCos( time * M_PI * 2 ) ;
    return s;
}

void MainWindow::timerEvent(QTimerEvent *event){
    if(event->timerId()==timeId){//定时器到时间,//模拟数据填充
        static QTime dataTime(QTime::currentTime());
        long int eltime = dataTime.elapsed();
        static int lastpointtime = 0;
        int size = (eltime - lastpointtime);//数据个数
        //size=1;
        qDebug()<<"size-->"<<size;
        if(isVisible()){
            QVector<QPointF> oldPoints = m_series->pointsVector();//Returns the points in the series as a vector
            QVector<QPointF> points;

            for(int i=size;i<oldPoints.count();++i){
                points.append(QPointF(i-size ,oldPoints.at(i).y()));//替换数据用
            }
            qint64 sizePoints = points.count();
            for(int k=0;k<size;++k){
                points.append(QPointF(k+sizePoints,getData((((double)lastpointtime+k+1)/1000))));
                //qDebug()<<getData((((double)lastpointtime+k+1)/1000));
            }
            m_series->replace(points);
            lastpointtime = eltime;
       }
    }
}
MainWindow::~MainWindow()
{
    delete ui;
}
