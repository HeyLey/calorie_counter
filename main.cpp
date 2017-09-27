#include "mainwindow.h"
#include <QApplication>
#include "database.h"
#include <iostream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile initDbScript("/Users/leyla/Qt/script.sql");
    if (!initDbScript.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        std::cout << "Did not open init script" << std::endl;
        return 1;
    }
    auto res = Database::getInstance().ExecuteScript(initDbScript);

    //if(res.type() != QSqlError::NoError)
    //{
    //    std::cout << res.text().toStdString() << std::endl;
    //    return 1;
    //}

    MainWindow w;
    w.show();

    return a.exec();
}
