#include "croncreatediolog.h"
#include "ui_croncreatediolog.h"

CronCreateDiolog::CronCreateDiolog(QWidget *parent, function<bool(const string&,
                                                                  const string&,
                                                                  const string&)> callback)
    : QDialog(parent)
    , ui(new Ui::CronCreateDiolog)
{
    add = callback;
    // this->taskManager = new CronScheduler();
    ui->setupUi(this);
    init();
}

void CronCreateDiolog::init(){
    ui->radioButton_Basic->setChecked(true);
    ui->comboBox->setEnabled(true);

    ui->lineEdit_Minuts->setEnabled(false);
    ui->lineEdit_Hours->setEnabled(false);
    ui->lineEdit_Days->setEnabled(false);
    ui->lineEdit_DaysWeek->setEnabled(false);
    ui->lineEdit_Month->setEnabled(false);

    ui->lineEdit_Minuts->setText("*");
    ui->lineEdit_Hours->setText("*");
    ui->lineEdit_Days->setText("*");
    ui->lineEdit_DaysWeek->setText("*");
    ui->lineEdit_Month->setText("*");



    connect(ui->radioButton_Basic, &QRadioButton::toggled, this, [this](bool checked) {
        ui->comboBox->setEnabled(checked);
        ui->lineEdit_Minuts->setEnabled(!checked);
        ui->lineEdit_Hours->setEnabled(!checked);
        ui->lineEdit_Days->setEnabled(!checked);
        ui->lineEdit_DaysWeek->setEnabled(!checked);
        ui->lineEdit_Month->setEnabled(!checked);

        if(checked) {
            ui->lineEdit_Minuts->clear();
            ui->lineEdit_Hours->clear();
            ui->lineEdit_Days->clear();
            ui->lineEdit_DaysWeek->clear();
            ui->lineEdit_Month->clear();

            ui->lineEdit_Minuts->setText("*");
            ui->lineEdit_Hours->setText("*");
            ui->lineEdit_Days->setText("*");
            ui->lineEdit_DaysWeek->setText("*");
            ui->lineEdit_Month->setText("*");
        }
    });

    ui->comboBox->addItem("Выберите расписание...");
    ui->comboBox->addItem("Каждую минуту", "* * * * *");
    ui->comboBox->addItem("Каждый час", "0 * * * *");
    ui->comboBox->addItem("Ежедневно", "0 0 * * *");
    ui->comboBox->addItem("Еженедельно", "0 0 * * 0");
    ui->comboBox->addItem("Ежемесячно", "0 0 1 * *");
}

CronCreateDiolog::~CronCreateDiolog()
{
    delete ui;
}

void CronCreateDiolog::on_pushButton_cancel_clicked()
{
    // delete(taskManager);
    close();
}


void CronCreateDiolog::on_pushButton_create_clicked()
{
    // Получаем значения из полей формы
    QString command = ui->lineEdit_Command->text().trimmed();
    QString comment = ui->lineEdit_Comment->text().trimmed();
    QString schedule;

    // Проверяем, какой режим выбран (базовый или расширенный)
    if (ui->radioButton_Basic->isChecked()) {
        // Базовый режим - берем расписание из комбобокса
        if (ui->comboBox->currentIndex() == 0) {
            QMessageBox::warning(this, "Ошибка", "Выберите тип расписания в базовом режиме");
            return;
        }
        schedule = ui->comboBox->currentData().toString();
    } else {
        // Расширенный режим - собираем расписание из полей ввода
        QString minutes = ui->lineEdit_Minuts->text().trimmed();
        QString hours = ui->lineEdit_Hours->text().trimmed();
        QString days = ui->lineEdit_Days->text().trimmed();
        QString months = ui->lineEdit_Month->text().trimmed();
        QString daysWeek = ui->lineEdit_DaysWeek->text().trimmed();

        // Проверяем, что хотя бы одно поле заполнено
        if (minutes.isEmpty() && hours.isEmpty() && days.isEmpty() &&
            months.isEmpty() && daysWeek.isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Заполните хотя бы одно поле расписания");
            return;
        }

        // Формируем cron-строку (пустые поля заменяем на *)
        schedule = QString("%1 %2 %3 %4 %5")
                       .arg(minutes.isEmpty() ? "*" : minutes)
                       .arg(hours.isEmpty() ? "*" : hours)
                       .arg(days.isEmpty() ? "*" : days)
                       .arg(months.isEmpty() ? "*" : months)
                       .arg(daysWeek.isEmpty() ? "*" : daysWeek);
    }

    // Проверка обязательного поля "Команда"
    if (command.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Поле 'Команда' обязательно для заполнения");
        return;
    }

    if (!add(schedule.toStdString(), command.toStdString(), comment.toStdString())){
        QMessageBox::warning(this, "Ошибка", "Не удалось создать задачу, проверьте расписание");
        return;
    }

    QMessageBox::information(this, "Успех", "Cron-задача успешно создана");
    accept();
}

