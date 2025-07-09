#include "atcreatediolog.h"
#include "ui_atcreatediolog.h"


AtCreateDiolog::AtCreateDiolog(QWidget *parent, function<bool(const std::chrono::system_clock::time_point&,
                                        const std::string&,
                                        const std::string&,
                                        const std::string&)> callback)
    : QDialog(parent)
    , ui(new Ui::AtCreateDiolog)
{
    ui->setupUi(this);
    add = callback;
    // инициализация знаечний
    QDate date = QDate::currentDate();
    ui->dateEdit->setDate(date);

    dateOptionsGroup = new QButtonGroup(this);

    dateOptionsGroup->addButton(ui->radioTommorow, 1);     // "Завтра"
    dateOptionsGroup->addButton(ui->radioNextWeek, 2);     // "На следующей неделе"
    dateOptionsGroup->addButton(ui->radioNextMonth, 3);    // "Через месяц"
    dateOptionsGroup->addButton(ui->pushButton_cancelDate, 0);    // "Снять выделение"
    connect(dateOptionsGroup, &QButtonGroup::buttonClicked, this, &AtCreateDiolog::updateDate);
}


AtCreateDiolog::~AtCreateDiolog()
{
    delete ui;

}

void AtCreateDiolog::on_pushButton_create_clicked()
{
    QString command = ui->lineEdit_command->text().trimmed();
    QTime time = ui->timeEdit->time();
    QDate date = ui->dateEdit->date();
    QString comment = ui->lineEdit_comment->text().trimmed();
    QString queue = ui ->comboBox->currentText().trimmed();
    // taskManager->setQueue(queue.toStdString());
    if (command.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Поле 'Команда' обязательно для заполнения");
        return;
    }

    if (!date.isValid() || !time.isValid()) {
        QMessageBox::warning(this, "Ошибка", "Укажите корректные дату и время выполнения");
        return;
    }

    QDateTime executeTime(date, time);

    if (executeTime < QDateTime::currentDateTime()) {
        QMessageBox::warning(this, "Ошибка", "Укажите время в будущем");
        return;
    }

    QString atTime = executeTime.toString("HH:mm dd.MM.yyyy");

    chrono::system_clock::time_point scheduleTime;
    scheduleTime = TaskUtils::stringToTimePoint(atTime.toStdString());

    if (!add(scheduleTime, command.toStdString(), comment.toStdString(), queue.toStdString())){
        QMessageBox::warning(this, "Ошибка", "Не удалось создать задачу");
        return;
    }

    QMessageBox::information(this, "Успех", "Задача успешно создана");
    accept();
}

void AtCreateDiolog::updateDate(QAbstractButton* button) {
    int id = button->group()->id(button);  // Получаем ID кнопки
    QDate currentDate = QDate::currentDate();
    switch (id) {
    case 1: // Завтра
        ui->dateEdit->setDate(currentDate.addDays(1));
        break;
    case 2: // На следующей неделе
        ui->dateEdit->setDate(currentDate.addDays(7));
        break;
    case 3: // Через месяц
        ui->dateEdit->setDate(currentDate.addMonths(1));
        break;
    default:
        // Если ни один не выбран (например, сняли выбор)
        dateOptionsGroup->setExclusive(false);
        ui->radioTommorow->setChecked(false);
        ui->radioNextWeek->setChecked(false);
        ui->radioNextMonth->setChecked(false);
        dateOptionsGroup->setExclusive(true);
        ui->dateEdit->setDate(currentDate);
        break;
    }
}


void AtCreateDiolog::on_pushButton_cancel_clicked()
{
    // delete(taskManager);
    close();
}



