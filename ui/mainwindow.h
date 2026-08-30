#pragma once
#include <QMainWindow>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <vector>

#include "../headers/piecewisefunction.hpp"
#include "../headers/fragment.hpp"
#include "../headers/spline.hpp"
#include "../headers/arraysequence.hpp"
#include "../headers/listsequence.hpp"
#include "../headers/complex.hpp"
#include "../headers/exceptions.h"

enum DataTypeId { kTypeInt = 0, kTypeDouble = 1, kTypeComplex = 2 };

enum ContainerId { kContainerArray = 0, kContainerList = 1 };

enum OperationId {
    kOpSpecify = 0,
    kOpMonotonic = 1,
    kOpContinuous = 2,
    kOpEstimate = 3,
    kOpInterpolate = 4
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onOperationChanged();
    void onDataTypeChanged();
    void onRun();
    void onReset();

private:
    void setupUi();
    void updateVisibility();
    void updateFunctionState();

    template <typename T, template <typename> class Container>
    void doFurtherSpecification(PieceWiseFunction<T, Container>& func);

    template <typename T, template <typename> class Container>
    QString doEstimation(const PieceWiseFunction<T, Container>& func, double point);

    template <typename T, template <typename> class Container>
    QString describeFunction(const PieceWiseFunction<T, Container>& func);

    template <typename T>
    T parseScalar(const QString& str);

    Complex parseComplexTerm(const QString& raw);
    std::vector<Point> parsePoints(const QString& text);
    std::vector<ComplexPoint> parseComplexPoints(const QString& text);

    template <typename V>
    QString valueToString(const V& value);

    PieceWiseFunction<int, MutableArraySequence> funcIntArray;
    PieceWiseFunction<int, MutableListSequence> funcIntList;
    PieceWiseFunction<double, MutableArraySequence> funcDoubleArray;
    PieceWiseFunction<double, MutableListSequence> funcDoubleList;
    PieceWiseFunction<Complex, MutableArraySequence> funcComplexArray;
    PieceWiseFunction<Complex, MutableListSequence> funcComplexList;

    QVBoxLayout *mainLayout;

    QGroupBox *groupSettings;
    QComboBox *comboDataType, *comboContainer, *comboOperation;

    QGroupBox *groupFragment;
    QLabel *labelBegin, *labelEnd, *labelCoeffs;
    QLineEdit *lineBegin, *lineEnd, *lineCoeffs;

    QGroupBox *groupPoint;
    QLabel *labelPointX;
    QLineEdit *linePointX;

    QGroupBox *groupPoints;
    QTextEdit *textPoints;

    QPushButton *btnRun, *btnReset;

    QGroupBox *groupResult;
    QTextEdit *textResult;

    QGroupBox *groupState;
    QTextEdit *textState;
};