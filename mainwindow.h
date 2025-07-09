#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QComboBox>
#include <QDateTime>
#include <QHBoxLayout>
#include <QMenu>
#include <QAction>
#include <QTimer>

#include <memory>

#include "atcreatediolog.h"
#include "croncreatediolog.h"
#include "taskschedulerservice.h"
#include "utils.h"
#include "ui_mainwindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr, TaskSchedulerService* s = nullptr);
    ~MainWindow();
    TaskSchedulerService *service;


private slots:
    void on_pushButton_AtCreate_clicked();
    void on_pushButton_CronCreate_clicked();
    void on_pushButton_AtList_clicked();
    void on_pushButton_CronList_clicked();

    void on_pushButton_clear_clicked();

private:
    void populateTableAt(QTableWidget* table, const std::vector<AtTask>& tasks);
    void applyFilters();
    void applyFiltersCron();
    void populateTableCron(QTableWidget* table, const std::vector<CronTask>& tasks);
    QHBoxLayout* createTaskFiltersLayout();
    QHBoxLayout* createTaskFiltersLayoutCron();
    void switcher();
    Ui::MainWindow *ui;
    AtCreateDiolog *atCreateWindow;
    CronCreateDiolog *cronCreateWindow;

    QComboBox* queueCombo;
    QComboBox* statusCombo;
    QComboBox* sortCombo;
    QComboBox* sortDirectionCombo;
    QComboBox* cronStatusCombo;

    QTimer* syncTimer;
    bool flag_type = true;

    //для фильтров
    bool filtersInitialized = false;
    QLayout* currentFiltersLayout = nullptr;
    AtTaskFilter filter;
    AtTaskSort sort;
    bool show_all = true;
    bool show_active = true;

    // Переменные для сохранения состояния фильтров AT задач
    int saved_at_queue_index = -1;
    int saved_at_status_index = -1;
    int saved_at_sort_index = -1;
    int saved_at_sort_direction_index = -1;

    // Переменные для сохранения состояния фильтров Cron задач
    int saved_cron_status_index = -1;

    // Новые методы
    void refreshDataWithFilters();
    void saveAtFiltersState();
    void restoreAtFiltersState();
    void saveCronFiltersState();
    void restoreCronFiltersState();
};
#endif // MAINWINDOW_H
