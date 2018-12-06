#include "widget.h"
#include "ui_widget.h"
#include "cradar.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    CRadar* radar = new CRadar(this);
    radar->setFixedSize(300,300);
    radar->move(50,50);
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}
