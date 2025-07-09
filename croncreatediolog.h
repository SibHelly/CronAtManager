#ifndef CRONCREATEDIOLOG_H
#define CRONCREATEDIOLOG_H

#include <QDialog>
#include <QMessageBox>

#include <iostream>
#include <chrono>
#include <set>

using namespace std;

namespace Ui {
class CronCreateDiolog;
}

class CronCreateDiolog : public QDialog
{
    Q_OBJECT

public:
    explicit CronCreateDiolog(QWidget *parent = nullptr, function<bool(const string&,
                                                                       const string&,
                                                                       const string&)> callback = nullptr);
    ~CronCreateDiolog();

private slots:
    void on_pushButton_cancel_clicked();

    void on_pushButton_create_clicked();

private:
    Ui::CronCreateDiolog *ui;
    function<bool(const string&,
                  const string&,
                  const string&)> add;
    void init();
};

#endif // CRONCREATEDIOLOG_H
