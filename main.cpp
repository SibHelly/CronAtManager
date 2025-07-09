#include "mainwindow.h"
// #include "atscheduler.h"
// #include "cronscheduler.h"
#include "taskschedulerservice.h"



#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TaskSchedulerService service;

    if (!service.initialize()) {
        std::cerr << "Failed to initialize scheduler" << std::endl;
        return 1;
    }
    MainWindow w(nullptr, &service);

    w.show();
    return a.exec();
}


// scheduler=new CronScheduler();
// // scheduler->addJob("echo Test cron 1 >> Linux_at_command.txt", "*/1 * * * *", "Testing");
// // scheduler->addJob("echo Test cron 2 >> Linux_at_command.txt", "*/1 * * * *", "Testing 5");
// // scheduler->addJob("echo Test cron 3 >> Linux_at_command.txt", "*/1 * * * *", "Testing 6");
// // scheduler->removeJob("5");
// // scheduler->removeJob("6");
// // scheduler->addJob("echo Testing cron task>> Linux_at_command.txt", "21:11");
// scheduler->listJobs();

// // if (scheduler->removeJob("14"))
// //     std::cout<<"sucsessful"<<std::endl;
// // else
// //     std::cout<<"oh no"<<std::endl;
// // if (scheduler->removeJob("16"))
// //     std::cout<<"sucsessful";

// // scheduler->listJobs();
