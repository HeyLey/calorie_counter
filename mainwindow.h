#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QtWidgets/QTableView>
#include <QModelIndex>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void setNewMeasurementList(QModelIndex newIndex);
    void addSelectedToDiary();

    void refreshStatistics(QDate newDate);
    void refreshStatisticsTab();

private:
    Ui::MainWindow *ui;
    QSqlRelationalTableModel *productNutirtionInfoModel;
    QSqlRelationalTableModel *productInfoModel;
    QSqlQueryModel *mealModel;
    QSqlQueryModel *measurementModel;
    QSqlQueryModel *statisticsModel;

    std::map<QString, int> idsOfMeasurements;
    std::map<QString, int> idsOfMeals;
    std::map<QString, int> idsOfProducts;

    int currentProduct;


    void showError(const QString& caption, const QSqlError& error);
    QSqlError fillTableModel(QSqlRelationalTableModel ** model, QTableView * view, const QString &tableName);

    QSqlError fillIdMap(const QString& tableName, std::map<QString, int>& res, const QString& column = "name");

};

#endif // MAINWINDOW_H
