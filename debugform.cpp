#include "debugform.h"
#include "ui_debugform.h"
#include "objectrecog.h"

#include <QTableWidget>
#include <opencv2/core.hpp>
#include <QElapsedTimer>


DebugForm::DebugForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DebugForm)
{
    ui->setupUi(this);
}

DebugForm::~DebugForm()
{
    delete ui;
}
void DebugForm::printGoodMatches(std::vector<std::vector<ObjectRecog::goodMatchEntry> > vec)
{
    //prepare Tables
    ui->tableWidget->setRowCount(static_cast<int>( vec.at(0).size() ) );
    ui->tableWidget->setColumnCount(static_cast<int>( vec.size() ) );

    ui->tableWidget_2->setRowCount(static_cast<int>( vec.at(0).size() ) );
    ui->tableWidget_2->setColumnCount(static_cast<int>( vec.size() ) );


    //ad content
    for ( int i = 0; i < vec.size() ; i++)
    {
        std::vector<ObjectRecog::goodMatchEntry > vec_inner = vec.at(i);
        for ( int j = 0; j < vec_inner.size() ; j++)
        {
            ObjectRecog::goodMatchEntry val = vec_inner.at(j);

            //table 1: source of Object
            QTableWidgetItem *newItem1 = new QTableWidgetItem();
            newItem1->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            newItem1->setText(QString::number(val.object_index));
            ui->tableWidget->setItem(j, i, newItem1);


            //table 2: some data of content

            QTableWidgetItem *newItem2 = new QTableWidgetItem();
            newItem2->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
            newItem2->setText(QString::number(static_cast<double>(vec_inner.size())/static_cast<double>(vec_inner.at(0).object_size)));
            ui->tableWidget_2->setItem(j, i, newItem2);
        }
    }
}
