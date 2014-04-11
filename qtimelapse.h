#ifndef QTIMELAPSE_H
#define QTIMELAPSE_H

#include <QMainWindow>

namespace Ui {
class QTimeLapse;
}

class QTimeLapse : public QMainWindow
{
    Q_OBJECT

public:
    explicit QTimeLapse(QWidget *parent = 0);
    ~QTimeLapse();

private:
    Ui::QTimeLapse *ui;
};

#endif // QTIMELAPSE_H
