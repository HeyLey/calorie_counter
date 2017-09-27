#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql>
#include <QFile>
#include <iostream>
#include <QStandardPaths>

class Database
{
public:
    static Database& getInstance()
    {
        static Database ourInstance;
        return ourInstance;
    }

    QSqlError ExecuteQuery(const QString& queryString)
    {
        QSqlQuery q(driver);
        bool ok = q.exec(queryString);

        if(!ok)
        {
            return q.lastError();
        }
        else
        {
            return QSqlError();
        }
    }

    QSqlError ExecuteScript(QFile& file)
    {
        while (!file.atEnd())
        {
            QByteArray readLine="";
            QString cleanedLine;
            QString line="";
            bool finished=false;
            while(!finished)
            {
                readLine = file.readLine();
                cleanedLine=readLine.trimmed();
                // remove comments at end of line
                QStringList strings=cleanedLine.split("--");
                cleanedLine=strings.at(0);

                // remove lines with only comment, and DROP lines
                if(!cleanedLine.startsWith("--")
                        && !cleanedLine.isEmpty())
                {
                    line+=" " + cleanedLine;
                }
                if(cleanedLine.endsWith(";"))
                {
                    break;
                }
                if(cleanedLine.startsWith("COMMIT"))
                {
                    finished=true;
                }
            }

            if(!line.isEmpty())
            {
                std::cout << line.toStdString() << std::endl;

                QSqlError result = ExecuteQuery(line);

                if(result.type() != QSqlError::NoError)
                {
                    return result;
                }
            }
        }

        return QSqlError();
    }

    QSqlDatabase& getDriver()
    {
        return driver;
    }

private:
    Database()
    {
        QString databaseLocationPath = qApp->applicationDirPath() + QDir::separator() + "caloriedb.sqlite";

        driver = QSqlDatabase::addDatabase("QSQLITE");
        driver.setDatabaseName(databaseLocationPath);
        driver.open();
    }

    QSqlDatabase driver;
};

#endif // DATABASE_H
