#ifndef DEBUGFORM_H
#define DEBUGFORM_H

#include "objectrecog.h"

#include <QWidget>
#include <opencv2/core.hpp>
#include <vector>
#include <QElapsedTimer>

namespace Ui {
class DebugForm;
}

class DebugForm : public QWidget
{
    Q_OBJECT

public:
    explicit DebugForm(QWidget *parent = 0);
    ~DebugForm();

private:
    Ui::DebugForm *ui;
    QElapsedTimer timer;

public slots:
    void printGoodMatches(std::vector< std::vector< ObjectRecog::goodMatchEntry > > vec);
};

#endif // DEBUGFORM_H
