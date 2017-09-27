#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "database.h"

#include <QMessageBox>

void MainWindow::showError(const QString& caption, const QSqlError& err)
{
    QMessageBox::critical(this, caption, err.text());
}

QSqlError MainWindow::fillTableModel(QSqlRelationalTableModel ** model, QTableView * view, const QString &tableName)
{
    delete *model;

    *model = new QSqlRelationalTableModel(view, Database::getInstance().getDriver());
    (**model).setTable(tableName);

    if(!(**model).select())
    {
        return (**model).lastError();
    }

    view->setModel(*model);
    return QSqlError();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    productInfoModel = nullptr;
    productNutirtionInfoModel = nullptr;

    fillTableModel(&productNutirtionInfoModel, ui->productNutritionInfoTable, "DiaryNutritionInfo");
    fillTableModel(&productInfoModel, ui->productInfoTable, "ProductNutritionInfo");

    fillIdMap("Meals", idsOfMeals);
    fillIdMap("Measurements", idsOfMeasurements);
    fillIdMap("Products", idsOfProducts);

    ui->productNutritionInfoTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->productInfoTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    measurementModel = new QSqlQueryModel(this);
    measurementModel->setQuery("SELECT Measurement FROM MeasurementsPerProduct WHERE ProductId = -1;", Database::getInstance().getDriver());
    ui->measurementSelection->setModel(measurementModel);

    mealModel = new QSqlQueryModel(this);
    mealModel->setQuery("SELECT name FROM Meals;", Database::getInstance().getDriver());
    ui->mealSelection->setModel(mealModel);

    statisticsModel = new QSqlQueryModel(this);
    refreshStatistics(QDate::currentDate());

    ui->dateToViewStatistics->setDate(QDate::currentDate());
    ui->statisticsView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui->dateToViewStatistics, SIGNAL(dateChanged(QDate)), this, SLOT(refreshStatistics(QDate)));
    connect(ui->productInfoTable->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(setNewMeasurementList(QModelIndex)));
    connect(ui->addToDiaryButton, SIGNAL(released()), this, SLOT(addSelectedToDiary()));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(refreshStatisticsTab()));
}

void MainWindow::setNewMeasurementList(QModelIndex newIndex)
{
    QSqlQuery newQuery("SELECT Measurement FROM MeasurementsPerProduct WHERE ProductId = ?;", Database::getInstance().getDriver());
    QModelIndex productNameIndex = newIndex.model()->index(newIndex.row(), 0);

    QString productName = productNameIndex.data().toString();

    currentProduct = idsOfProducts[productName];
    newQuery.addBindValue(currentProduct);
    newQuery.exec();

    measurementModel->setQuery(newQuery);
    ui->measurementSelection->setCurrentIndex(0);
}

void MainWindow::addSelectedToDiary()
{
    double amount = ui->amountSelection->value();
    if(amount <= 0)
    {
        showError("Illegal amount!", QSqlError("Amount cannot be less than 0.1"));
        return;
    }

    int mealId = idsOfMeals[ui->mealSelection->currentText()];
    int measurementId = idsOfMeasurements[ui->measurementSelection->currentText()];
    QString date = QDate::currentDate().toString("yyyy-MM-dd");

    QSqlQuery q("", Database::getInstance().getDriver());
    q.prepare("INSERT INTO Diary(productId, measurementId, amount,  mealId, day) VALUES(?, ?, ?, ?, date(?));");
    q.bindValue(0, currentProduct);
    q.bindValue(1, measurementId);
    q.bindValue(2, amount);
    q.bindValue(3, mealId);
    q.bindValue(4, date);

    q.exec();
    if(q.lastError().type() != QSqlError::NoError)
    {
        showError("unable to add to diary", q.lastError());
    }

    if(!productNutirtionInfoModel->select())
    {
        showError("unable to add to diary", productNutirtionInfoModel->lastError());
    }

}

QSqlError MainWindow::fillIdMap(const QString &tableName, std::map<QString, int> &res, const QString &column)
{
    QSqlQuery q("SELECT * FROM " + tableName, Database::getInstance().getDriver());
    q.exec();

    if(q.lastError().type() != QSqlError::NoError)
    {
        return q.lastError();
    }

    while(q.next())
    {
        res[q.value(column).toString()] = q.value("id").toInt();
    }

    return QSqlError();
}

void MainWindow::refreshStatistics(QDate newDate)
{
    QSqlQuery q("", Database::getInstance().getDriver());
    q.prepare("SELECT * FROM Statistics WHERE Date = date(?);");
    q.addBindValue(newDate.toString("yyyy-MM-dd"));

    q.exec();
    statisticsModel->setQuery(q);
    ui->statisticsView->setModel(statisticsModel);
}

void MainWindow::refreshStatisticsTab()
{
    QDate currentSelectedDate = ui->dateToViewStatistics->date();
    refreshStatistics(currentSelectedDate);
}

MainWindow::~MainWindow()
{
    delete mealModel;
    delete measurementModel;
    delete productNutirtionInfoModel;
    delete productInfoModel;
    delete statisticsModel;

    delete ui;
}
