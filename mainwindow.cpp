#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent, TaskSchedulerService* s)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), syncTimer(new QTimer(this))
{
    service = s;
    ui->setupUi(this);
    // Настройка таймера синхронизации
    connect(syncTimer, &QTimer::timeout, this, [this]() {
        if (service) {
            qDebug() << "Синхронизация данных...";
            service->sync_all_tasks();
            // Обновляем только данные, сохраняя фильтры
            refreshDataWithFilters();
        }
    });

    // Запускаем таймер с интервалом 5 секунд (5000 мс)
    syncTimer->start(5000);
    switcher();
}

MainWindow::~MainWindow()
{
    if (syncTimer) {
        syncTimer->stop();
    }
    delete ui;
}

void MainWindow::switcher() {
    QSignalBlocker blocker(ui->table);
    ui->table->clear();
    ui->table->setRowCount(0);
    ui->table->setColumnCount(0);

    // Удаляем старый layout и виджеты
    QLayout* oldLayout = ui->widget->layout();
    if (oldLayout) {
        QLayoutItem* item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                item->widget()->deleteLater();
            }
            delete item;
        }
        delete oldLayout;
    }

    // Отключаем все предыдущие соединения сигналов от таблицы
    disconnect(ui->table, &QTableWidget::cellChanged, nullptr, nullptr);
    disconnect(ui->table, &QTableWidget::itemChanged, this, nullptr);

    if (!flag_type) {
        ui->widget->setLayout(createTaskFiltersLayout());
        // Восстанавливаем состояние фильтров для AT задач
        restoreAtFiltersState();
        applyFilters();
        ui->labe_type->setText("At задачи");
        ui->pushButton_clear->setText("Очистить at");
    } else {
        ui->widget->setLayout(createTaskFiltersLayoutCron());
        // Восстанавливаем состояние фильтров для Cron задач
        restoreCronFiltersState();
        populateTableCron(ui->table, service->get_cron_tasks());
        ui->labe_type->setText("Cron задачи");
        ui->pushButton_clear->setText("Очистить cron");
    }
}

void MainWindow::refreshDataWithFilters() {
    // Обновляем данные без пересоздания интерфейса
    if (!flag_type) {
        // Для AT задач применяем текущие фильтры
        applyFilters();
    } else {
        // Для Cron задач применяем текущие фильтры
        applyFiltersCron();
    }
}

void MainWindow::saveAtFiltersState() {
    if (queueCombo) {
        saved_at_queue_index = queueCombo->currentIndex();
    }
    if (statusCombo) {
        saved_at_status_index = statusCombo->currentIndex();
    }
    if (sortCombo) {
        saved_at_sort_index = sortCombo->currentIndex();
    }
    if (sortDirectionCombo) {
        saved_at_sort_direction_index = sortDirectionCombo->currentIndex();
    }
}

void MainWindow::restoreAtFiltersState() {
    if (queueCombo) {
        // Если сохраненный индекс есть, используем его, иначе устанавливаем по умолчанию
        if (saved_at_queue_index >= 0) {
            queueCombo->setCurrentIndex(saved_at_queue_index);
        } else {
            queueCombo->setCurrentIndex(0); // "Все очереди"
        }
    }
    if (statusCombo) {
        if (saved_at_status_index >= 0) {
            statusCombo->setCurrentIndex(saved_at_status_index);
        } else {
            statusCombo->setCurrentIndex(0); // "Все задачи"
        }
    }
    if (sortCombo) {
        if (saved_at_sort_index >= 0) {
            sortCombo->setCurrentIndex(saved_at_sort_index);
        } else {
            sortCombo->setCurrentIndex(3); // "По id" - по умолчанию
        }
    }
    if (sortDirectionCombo) {
        if (saved_at_sort_direction_index >= 0) {
            sortDirectionCombo->setCurrentIndex(saved_at_sort_direction_index);
        } else {
            sortDirectionCombo->setCurrentIndex(0); // "По возрастанию"
        }
    }
}

void MainWindow::saveCronFiltersState() {
    if (cronStatusCombo) {
        saved_cron_status_index = cronStatusCombo->currentIndex();
    }
}

void MainWindow::restoreCronFiltersState() {
    if (cronStatusCombo) {
        if (saved_cron_status_index >= 0) {
            cronStatusCombo->setCurrentIndex(saved_cron_status_index);
        } else {
            cronStatusCombo->setCurrentIndex(0); // "Все задачи"
        }
    }
}

void MainWindow::on_pushButton_AtList_clicked() {
    if (flag_type) {
        // Сохраняем состояние Cron фильтров перед переключением
        saveCronFiltersState();
    }
    flag_type = false;
    switcher();
}

void MainWindow::on_pushButton_CronList_clicked() {
    if (!flag_type) {
        // Сохраняем состояние AT фильтров перед переключением
        saveAtFiltersState();
    }
    flag_type = true;
    switcher();
}

QHBoxLayout* MainWindow::createTaskFiltersLayoutCron() {
    QHBoxLayout* filtersLayout = new QHBoxLayout();

    // Фильтр по статусу
    QLabel* statusLabel = new QLabel("Статус:");
    cronStatusCombo = new QComboBox();
    cronStatusCombo->addItem("Все задачи", 0);
    cronStatusCombo->addItem("Только активные", 1);
    cronStatusCombo->addItem("Только остановленные", 2);

    // Подключаем автоматическое применение фильтра
    connect(cronStatusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::applyFiltersCron);

    // Добавляем элементы в layout
    filtersLayout->addWidget(statusLabel);
    filtersLayout->addWidget(cronStatusCombo);
    filtersLayout->addStretch(); // Растягиваем пространство

    // Настройки внешнего вида
    filtersLayout->setContentsMargins(5, 5, 5, 5);
    filtersLayout->setSpacing(5);

    return filtersLayout;
}

QHBoxLayout* MainWindow::createTaskFiltersLayout() {
    QHBoxLayout* filtersLayout = new QHBoxLayout();

    // 1. Фильтр по очереди
    QLabel* queueLabel = new QLabel("Очередь:");
    queueCombo = new QComboBox();
    queueCombo->addItem("Все очереди", "");
    for (char c = 'a'; c <= 'z'; ++c) {
        QString letter = QString(QChar(c));
        queueCombo->addItem(letter, letter);
    }

    // 2. Фильтр по статусу
    QLabel* statusLabel = new QLabel("Статус:");
    statusCombo = new QComboBox();
    statusCombo->addItem("Все задачи", 0);
    statusCombo->addItem("Только ожидающие", 1);
    statusCombo->addItem("Только выполненные", 2);

    // 3. Сортировка
    QLabel* sortLabel = new QLabel("Сортировка:");
    sortCombo = new QComboBox();
    sortCombo->addItem("По времени", 0);
    sortCombo->addItem("По очереди", 1);
    sortCombo->addItem("По статусу", 2);
    sortCombo->addItem("По id", 3);

    // 4. Направление сортировки
    sortDirectionCombo = new QComboBox();
    sortDirectionCombo->addItem("По возрастанию", Qt::AscendingOrder);
    sortDirectionCombo->addItem("По убыванию", Qt::DescendingOrder);

    // Подключаем автоматическое применение фильтров
    connect(queueCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::applyFilters);
    connect(statusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::applyFilters);
    connect(sortCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::applyFilters);
    connect(sortDirectionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::applyFilters);

    // Добавляем элементы в layout с отступами
    filtersLayout->addWidget(queueLabel);
    filtersLayout->addWidget(queueCombo);
    filtersLayout->addSpacing(10);

    filtersLayout->addWidget(statusLabel);
    filtersLayout->addWidget(statusCombo);
    filtersLayout->addSpacing(10);

    filtersLayout->addWidget(sortLabel);
    filtersLayout->addWidget(sortCombo);
    filtersLayout->addSpacing(5);
    filtersLayout->addWidget(sortDirectionCombo);

    filtersLayout->addStretch(); // Растягиваем пространство

    // Настройки внешнего вида
    filtersLayout->setContentsMargins(5, 5, 5, 5);
    filtersLayout->setSpacing(5);

    return filtersLayout;
}

void MainWindow::applyFilters() {
    if (flag_type) {
        applyFiltersCron();
    } else {
        // Сохраняем текущее состояние фильтров
        saveAtFiltersState();

        // Устанавливаем фильтр очереди
        filter.queue_filter = queueCombo->currentData().toString().toStdString();

        // Устанавливаем фильтр статуса
        switch(statusCombo->currentData().toInt()) {
        case 1: filter.show_only_pending = true; break;
        case 2: filter.show_only_completed = true; break;
        }

        // Устанавливаем параметры сортировки
        switch(sortCombo->currentData().toInt()) {
        case 0: sort.by_scheduled_time = true; break;
        case 1: sort.by_queue = true; break;
        case 2: sort.by_status = true; break;
        case 3: sort.by_at_id = true; break;
        }

        sort.ascending = (sortDirectionCombo->currentData().toInt() == Qt::AscendingOrder);

        // Получаем отфильтрованные задачи
        auto filteredTasks = service->get_at_tasks_filtred(filter, sort);
        // Обновляем таблицу
        populateTableAt(ui->table, filteredTasks);
    }
}


void MainWindow::applyFiltersCron() {
    // Сохраняем текущее состояние фильтров
    saveCronFiltersState();

    if (cronStatusCombo) {
        switch(cronStatusCombo->currentData().toInt()) {
        case 1: // Только активные
            show_all = false;
            show_active = true;
            break;
        case 2: // Только остановленные
            show_all = false;
            show_active = false;
            break;
        case 0: // Все задачи
        default:
            show_all = true;
            break;
        }
    }
    auto filteredTasks = service->get_cron_tasks_filtred(show_all, show_active);
    populateTableCron(ui->table, filteredTasks);
}


void MainWindow::on_pushButton_AtCreate_clicked()
{
    auto lambda = [this](const auto& tp, const auto& name, const auto& desc, const auto& que) {
        return service ? service->add_at_task(tp, name, desc, que) : false;
    };

    hide();
    atCreateWindow = new AtCreateDiolog(this, lambda);
    atCreateWindow->show();
    atCreateWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(atCreateWindow, &AtCreateDiolog::finished, this, [this](int ) {
        show();
        switcher();
    });
}


void MainWindow::on_pushButton_CronCreate_clicked()
{
    auto lambda = [this](const auto& tp, const auto& name, const auto& desc) {
        return service ? service->add_cron_task(tp, name, desc) : false;
    };

    hide();
    cronCreateWindow = new CronCreateDiolog(this, lambda);
    cronCreateWindow->show();
    cronCreateWindow->setAttribute(Qt::WA_DeleteOnClose);
    connect(cronCreateWindow, &CronCreateDiolog::finished, this, [this](int ){
        show();
        switcher();
    });
}

void MainWindow::populateTableAt(QTableWidget* table, const std::vector<AtTask>& tasks) {
    table->setRowCount(tasks.size());
    table->setColumnCount(7); // Description, Command, Time, Status, JobID, Queue, Actions

    // Установка заголовков
    QStringList headers = {"Описание", "Команда", "Время запуска", "Статус", "ID из At", "Очередь", "Действия"};
    table->setHorizontalHeaderLabels(headers);

    for (size_t row = 0; row < tasks.size(); ++row) {
        const AtTask& task = tasks[row];

        // Description - редактируемое поле
        QTableWidgetItem* descItem = new QTableWidgetItem();
        if (!task.description.empty()) {
            descItem->setText(QString::fromStdString(task.description));
        } else {
            descItem->setText(QString::fromStdString("At job #" + task.at_job_id));
        }
        descItem->setFlags(descItem->flags() | Qt::ItemIsEditable);
        // Сохраняем ID задачи в data элемента для последующего использования
        descItem->setData(Qt::UserRole, QString::fromStdString(task.id));
        table->setItem(row, 0, descItem);

        // Command - только для чтения
        QTableWidgetItem* cmdItem = new QTableWidgetItem(QString::fromStdString(task.command));
        cmdItem->setFlags(cmdItem->flags() & ~Qt::ItemIsEditable);
        table->setItem(row, 1, cmdItem);

        // Scheduled Time - только для чтения
        auto timeStr = QDateTime::fromSecsSinceEpoch(
                           std::chrono::duration_cast<std::chrono::seconds>(
                               task.scheduled_time.time_since_epoch()
                               ).count()
                           ).toString("yyyy-MM-dd HH:mm:ss");
        QTableWidgetItem* timeItem = new QTableWidgetItem(timeStr);
        timeItem->setFlags(timeItem->flags() & ~Qt::ItemIsEditable);
        table->setItem(row, 2, timeItem);

        // Status - только для чтения
        QTableWidgetItem* statusItem = new QTableWidgetItem(
            task.is_executed ? "Выполнена" : "Ожидает"
            );
        statusItem->setFlags(statusItem->flags() & ~Qt::ItemIsEditable);
        table->setItem(row, 3, statusItem);

        // Job ID - только для чтения
        QTableWidgetItem* jobIdItem = new QTableWidgetItem(
            QString::fromStdString(task.at_job_id)
            );
        jobIdItem->setFlags(jobIdItem->flags() & ~Qt::ItemIsEditable);
        table->setItem(row, 4, jobIdItem);

        QTableWidgetItem* queueItem = new QTableWidgetItem(
            QString::fromStdString(task.queue)
            );
        jobIdItem->setFlags(queueItem->flags() & ~Qt::ItemIsEditable);
        table->setItem(row, 5, queueItem);

        // Action Button - кнопка удаления
        QPushButton* deleteBtn = new QPushButton("Удалить");
        table->setCellWidget(row, 6, deleteBtn);

        connect(deleteBtn, &QPushButton::clicked, [this, taskId = task.id]() {
            qDebug() << "Delete task:" << QString::fromStdString(taskId);
            if (service) {
                service->remove_at_task(taskId);
                switcher(); // Обновляем таблицу после удаления
            }
        });
    }

    // Подключаем обработчик изменения элементов таблицы
    connect(table, &QTableWidget::itemChanged, this, [this](QTableWidgetItem* item) {
        if (item->column() == 0) { // Только для колонки Description
            // Получаем ID задачи из data элемента
            QString taskId = item->data(Qt::UserRole).toString();

            if (!taskId.isEmpty() && service) {
                // Получаем полную задачу по ID
                auto task = service->get_at_task(taskId.toStdString());
                AtTask updatedTask = task;
                updatedTask.description = item->text().toStdString();
                // Вызываем метод обновления
                service->update_at_task(updatedTask);
                qDebug() << "Updated task description:" << item->text();
            }
        }
    });

    table->resizeColumnsToContents();
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
}



void MainWindow::populateTableCron(QTableWidget* table, const std::vector<CronTask>& tasks) {
    table->setRowCount(tasks.size());
    table->setColumnCount(6); // Description, Command, Cron Expression, Created, Status, Actions

    QStringList headers = {"Описание", "Команда", "Cron выражение", "Время создания", "Статус", "Действия"};
    table->setHorizontalHeaderLabels(headers);

    table->blockSignals(true);
    for (size_t row = 0; row < tasks.size(); ++row) {
        const CronTask& task = tasks[row];

        // Description (редактируемое)
        QTableWidgetItem* descItem = new QTableWidgetItem(
            task.description.empty() ?
                QString::fromStdString("Cron job #" + task.id) :
                QString::fromStdString(task.description)
            );
        descItem->setData(Qt::UserRole, QString::fromStdString(task.id)); // Сохраняем ID
        table->setItem(row, 0, descItem);

        // Command (нередактируемое)
        QTableWidgetItem* cmdItem = new QTableWidgetItem(QString::fromStdString(task.command));
        cmdItem->setFlags(cmdItem->flags() ^ Qt::ItemIsEditable);
        table->setItem(row, 1, cmdItem);

        // Cron Expression (редактируемое)
        QTableWidgetItem* cronItem = new QTableWidgetItem(QString::fromStdString(task.cron_expression));
        cronItem->setData(Qt::UserRole, QString::fromStdString(task.id));
        table->setItem(row, 2, cronItem);

        // Created Time (нередактируемое)
        auto timeStr = QDateTime::fromSecsSinceEpoch(
                           std::chrono::duration_cast<std::chrono::seconds>(
                               task.created_at.time_since_epoch()
                               ).count()
                           ).toString("yyyy-MM-dd HH:mm:ss");
        QTableWidgetItem* timeItem = new QTableWidgetItem(timeStr);
        timeItem->setFlags(timeItem->flags() ^ Qt::ItemIsEditable);
        table->setItem(row, 3, timeItem);

        // Status (нередактируемое)
        QTableWidgetItem* statusItem = new QTableWidgetItem(
            task.is_active ? "Активна" : "Неактивна"
            );
        statusItem->setFlags(statusItem->flags() ^ Qt::ItemIsEditable);
        table->setItem(row, 4, statusItem);

        // Action Menu (выпадающий список действий)
        QPushButton* menuButton = new QPushButton("Действия");
        QMenu* menu = new QMenu(table);

        QAction* toggleAction = menu->addAction(
            task.is_active ? "Остановить" : "Запустить"
            );
        QAction* deleteAction = menu->addAction("Удалить");

        connect(toggleAction, &QAction::triggered, [this, taskId = task.id, isActive = task.is_active]() {
            if (service) {
                service->update_cron_task_status(taskId);
                switcher();
            }
        });

        connect(deleteAction, &QAction::triggered, [this, taskId = task.id]() {
            if (service) {
                if (service->remove_cron_task(taskId))
                    switcher();
                else{
                    QMessageBox::warning(this, "Ошибка", "Не удалось удалить задачу");
                }
            }
        });

        menuButton->setMenu(menu);
        table->setCellWidget(row, 5, menuButton);
    }

    // Настройка внешнего вида таблицы
    table->resizeColumnsToContents();
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);

    // Отключаем все предыдущие соединения для этой таблицы
    disconnect(table, &QTableWidget::cellChanged, nullptr, nullptr);

    connect(table, &QTableWidget::cellChanged, [this, table](int row, int column) {
        // Добавляем статический флаг для предотвращения рекурсивных вызовов
        static bool isUpdating = false;
        if (isUpdating) {
            return;
        }

        cout << "Cell changed: row=" << row << ", column=" << column << endl;


        if (!table) {
            cout << "Table is null" << endl;
            return;
        }

        if (!service) {
            cout << "Service is null" << endl;
            return;
        }

        // Проверяем границы
        if (row < 0 || row >= table->rowCount()) {
            cout << "Row out of bounds: " << row << endl;
            return;
        }

        // Получаем item для ID
        QTableWidgetItem* idItem = table->item(row, 0);
        if (!idItem) {
            cout << "ID item is null at row " << row << endl;
            return;
        }

        // Получаем ID задачи
        QString taskId = idItem->data(Qt::UserRole).toString();
        cout << "Task ID: '" << taskId.toStdString() << "'" << endl;

        if (taskId.isEmpty()) {
            cout << "Task ID is empty" << endl;
            return;
        }

        // Получаем текущие данные задачи
        CronTask task = service->get_cron_task(taskId.toStdString());
        if (task.id.empty()) {
            cout << "Task not found in service" << endl;
            return;
        }

        // Получаем изменённый item
        QTableWidgetItem* changedItem = table->item(row, column);
        if (!changedItem) {
            cout << "Changed item is null" << endl;
            return;
        }

        cout << "New value: " << changedItem->text().toStdString() << endl;

        // Сохраняем старое значение для возможного отката
        QString oldValue;

        // Обновляем поля
        switch (column) {
        case 0: // Описание
            oldValue = QString::fromStdString(task.description);
            task.description = changedItem->text().toStdString();
            cout << "Updating description" << endl;
            break;
        case 2: // Cron выражение
            oldValue = QString::fromStdString(task.cron_expression);
            task.cron_expression = changedItem->text().toStdString();
            cout << "Updating cron expression" << endl;
            break;
        default:
            cout << "Column " << column << " is not editable" << endl;
            return;
        }

        // Сохраняем изменения
        if (!service->update_cron_task(task)) {
            QMessageBox::warning(this, "Ошибка", "Не удалось обновить задачу, проверьте расписание задачи");
            cout << "Failed to update task" << endl;

            // Возвращаем старое значение
            table->blockSignals(true);
            changedItem->setText(oldValue);
            table->blockSignals(false);
        } else {
            cout << "Task updated successfully" << endl;
            // Устанавливаем флаг обновления
            isUpdating = true;
            QTimer::singleShot(0, this, [this]() {
                switcher();
                isUpdating = false;
            });
        }
    });
    table->blockSignals(false);
}

void MainWindow::on_pushButton_clear_clicked()
{
    if (!flag_type){
        service->clear_at();
        switcher();
    }else{
        service->clear_cron();
        switcher();
    }
}

