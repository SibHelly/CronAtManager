#ifndef ATCREATEDIOLOG_H
#define ATCREATEDIOLOG_H

#include <QDialog>
#include <QMessageBox>
#include <QButtonGroup>
#include <QProcess>

#include <iostream>
#include <chrono>
#include <memory>


#include "utils.h"

using namespace std;

namespace Ui {
class AtCreateDiolog;
}

class AtCreateDiolog : public QDialog
{
    Q_OBJECT

public:
    explicit AtCreateDiolog(QWidget *parent = nullptr, function<bool(
                                                            const chrono::system_clock::time_point&,
                                                            const string&,
                                                            const string&,
                                                            const string&)> callback = nullptr);
    ~AtCreateDiolog();

private slots:
    void on_pushButton_create_clicked();
    void on_pushButton_cancel_clicked();
    void updateDate(QAbstractButton* button);

private:
    Ui::AtCreateDiolog *ui;
    QButtonGroup *dateOptionsGroup;
    function<bool(const chrono::system_clock::time_point&,
                  const string&,
                  const string&,
                  const string&)> add;
};

#endif // ATCREATEDIOLOG_H
