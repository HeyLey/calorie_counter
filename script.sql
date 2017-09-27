DROP TABLE IF EXISTS Products;
DROP TABLE IF EXISTS Measurements;
DROP TABLE IF EXISTS NutritionInfo;
DROP TABLE IF EXISTS Meals;
DROP TABLE IF EXISTS Diary;

DROP VIEW IF EXISTS ProductNutritionInfo;
DROP VIEW IF EXISTS DiaryNutritionInfo;
DROP VIEW IF EXISTS MeasurementsPerProduct;
DROP VIEW IF EXISTS Statistics;


CREATE TABLE Products(id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT);
CREATE TABLE Measurements(id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT);
CREATE TABLE NutritionInfo(productId INTEGER, measurementId INTEGER, calories DOUBLE, proteinGrams DOUBLE, carbsGrams DOUBLE, fatGrams DOUBLE, 
	FOREIGN KEY(productId) REFERENCES Products(id), 
	FOREIGN KEY(measurementId) REFERENCES Measurements(id), 
	PRIMARY KEY(productId, measurementId));
CREATE TABLE Meals(id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT);
CREATE TABLE Diary(productId INTEGER, measurementId INTEGER, amount DOUBLE, mealId INTEGER, day DATE, 
	FOREIGN KEY(productId, measurementId) REFERENCES NutritionInfo(productId, measurementId), 
	FOREIGN KEY(MealId) REFERENCES Meals(id));

CREATE VIEW ProductNutritionInfo AS 
SELECT Products.name AS 'Product', Measurements.name AS 'Measurement', NutritionInfo.calories AS 'Calories', NutritionInfo.proteinGrams AS 'Protein, gr', NutritionInfo.carbsGrams AS 'Carbohydrates, gr', NutritionInfo.fatGrams AS 'Fats, gr' 
FROM NutritionInfo 
JOIN Products ON Products.id == NutritionInfo.productId 
JOIN Measurements ON Measurements.id == NutritionInfo.measurementId;

CREATE VIEW DiaryNutritionInfo AS 
SELECT Diary.day AS 'Date', Meals.name AS 'Meal', Products.name AS 'Product', Measurements.name AS 'Measurement', Diary.amount AS 'Amount', Diary.amount * NutritionInfo.calories AS 'Calories', Diary.amount * NutritionInfo.proteinGrams AS 'Protein, gr', Diary.amount * NutritionInfo.carbsGrams AS 'Carbohydrates, gr', Diary.amount * NutritionInfo.fatGrams AS 'Fats, gr' 
FROM Diary 
JOIN NutritionInfo ON (Diary.productId == NutritionInfo.productId AND Diary.measurementId == NutritionInfo.measurementId) 
JOIN Meals ON Meals.id == Diary.mealId 
JOIN Products ON Products.id == NutritionInfo.productId 
JOIN Measurements ON Measurements.id == NutritionInfo.measurementId;


CREATE VIEW MeasurementsPerProduct AS 
SELECT Products.id as 'ProductId', Measurements.name as 'Measurement' 
FROM NutritionInfo 
JOIN Products ON Products.id == NutritionInfo.productId 
JOIN Measurements ON Measurements.id == NutritionInfo.measurementId;

CREATE VIEW Statistics AS
SELECT Date, TOTAL("Calories") AS 'Total calories', TOTAL("Protein, gr") AS 'Total protein', TOTAL("Carbohydrates, gr") AS 'Total carbohydrates', TOTAL("Fats, gr") AS 'Total fats'
FROM DiaryNutritionInfo
GROUP BY Date;

 INSERT INTO Meals(name) VALUES ('Breakfast'), ('Lunch'), ('Dinner'), ('Snacks');
 INSERT INTO Products(name) VALUES ('banana'), ('coke');
 INSERT INTO Measurements(name) VALUES ('100 gr'), ('1 can'), ('1 pc');
 INSERT INTO NutritionInfo VALUES (1, 3, 105, 1.3, 27, 0.4), (2, 2, 139, 0, 39, 0);
 INSERT INTO Diary VALUES (1, 3, 2, 1, date("2017-08-26")), (2, 2, 1, 4, date("2017-08-26"));