#include "mainwindow.h"
#include "resources.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QRegularExpression>
#include <type_traits>

template <typename T>
T MainWindow::parseScalar(const QString& str) {
    if constexpr (std::is_same_v<T, Complex>) {
        return parseComplexTerm(str);
    } else if constexpr (std::is_same_v<T, int>) {
        bool ok = false;
        int value = str.toInt(&ok);
        if (!ok) {
            throw LogicErrorException(
                "Не удалось разобрать целочисленный коэффициент: " + str.toStdString());
        }
        return value;
    } else {
        bool ok = false;
        double value = str.toDouble(&ok);
        if (!ok) {
            throw LogicErrorException(
                "Не удалось разобрать вещественный коэффициент: " + str.toStdString());
        }
        return value;
    }
}

Complex MainWindow::parseComplexTerm(const QString& raw) {
    QString s = raw.trimmed();
    s.remove('(');
    s.remove(')');
    QStringList parts = s.split(';');
    if (parts.size() != 2) {
        throw LogicErrorException(
            "Комплексный коэффициент должен быть в формате (re;im), получено: "
            + raw.toStdString());
    }
    bool okRe = false, okIm = false;
    double re = parts[0].trimmed().toDouble(&okRe);
    double im = parts[1].trimmed().toDouble(&okIm);
    if (!okRe || !okIm) {
        throw LogicErrorException(
            "Не удалось разобрать комплексное число: " + raw.toStdString());
    }
    return Complex(re, im);
}

std::vector<Point> MainWindow::parsePoints(const QString& text) {
    std::vector<Point> points;
    QStringList lines = text.split('\n', Qt::SkipEmptyParts);
    for (const QString& rawLine : lines) {
        QString line = rawLine.trimmed();
        if (line.isEmpty()) continue;
        QStringList parts = line.split(QRegularExpression("[\\s,]+"), Qt::SkipEmptyParts);
        if (parts.size() != 2) {
            throw LogicErrorException(
                "Каждая строка должна содержать ровно два числа (x и y): "
                + line.toStdString());
        }
        bool okX = false, okY = false;
        double x = parts[0].toDouble(&okX);
        double y = parts[1].toDouble(&okY);
        if (!okX || !okY) {
            throw LogicErrorException("Не удалось разобрать точку: " + line.toStdString());
        }
        points.push_back(Point{x, y});
    }
    if (points.empty()) {
        throw LogicErrorException("Не задано ни одной точки для интерполяции");
    }
    return points;
}

std::vector<ComplexPoint> MainWindow::parseComplexPoints(const QString& text) {
    std::vector<ComplexPoint> points;
    QStringList lines = text.split('\n', Qt::SkipEmptyParts);
    for (const QString& rawLine : lines) {
        QString line = rawLine.trimmed();
        if (line.isEmpty()) continue;
        QStringList parts = line.split(QRegularExpression("[\\s,]+"), Qt::SkipEmptyParts);
        if (parts.size() != 3) {
            throw LogicErrorException(
                "Каждая строка должна содержать три числа (x, Re(y), Im(y)): "
                + line.toStdString());
        }
        bool okX = false, okRe = false, okIm = false;
        double x = parts[0].toDouble(&okX);
        double re = parts[1].toDouble(&okRe);
        double im = parts[2].toDouble(&okIm);
        if (!okX || !okRe || !okIm) {
            throw LogicErrorException("Не удалось разобрать точку: " + line.toStdString());
        }
        points.push_back(ComplexPoint{x, Complex(re, im)});
    }
    if (points.empty()) {
        throw LogicErrorException("Не задано ни одной точки для интерполяции");
    }
    return points;
}

template <typename V>
QString MainWindow::valueToString(const V& value) {
    if constexpr (std::is_same_v<V, Complex>) {
        return QString::fromStdString(value.toString());
    } else {
        return QString::number(static_cast<double>(value), 'f', 6);
    }
}

template <typename T, template <typename> class Container>
QString MainWindow::describeFunction(const PieceWiseFunction<T, Container>& func) {
    return QString::fromStdString(func.toString());
}

template <typename T, template <typename> class Container>
void MainWindow::doFurtherSpecification(PieceWiseFunction<T, Container>& func) {
    bool okB = false, okE = false;
    double begin = lineBegin->text().toDouble(&okB);
    double end = lineEnd->text().toDouble(&okE);
    if (!okB || !okE) {
        throw LogicErrorException("Некорректные границы отрезка");
    }

    QStringList parts = lineCoeffs->text().split(QRegularExpression("[\\s,]+"), Qt::SkipEmptyParts);
    if (parts.isEmpty()) {
        throw LogicErrorException("Не заданы коэффициенты многочлена");
    }

    MutableArraySequence<T> coeffs;
    for (const QString& part : parts) {
        coeffs.Append(parseScalar<T>(part));
    }

    Fragment<T> frag(begin, end, &coeffs);
    func.FurtherSpecification(frag);
}

template <typename T, template <typename> class Container>
QString MainWindow::doEstimation(const PieceWiseFunction<T, Container>& func, double point) {
    auto value = func.Estimation(point);
    return valueToString(value);
}

void MainWindow::updateFunctionState() {
    int type = comboDataType->currentData().toInt();
    int container = comboContainer->currentData().toInt();
    QString state;
    switch (type) {
    case kTypeInt:
        state = (container == kContainerArray) ? describeFunction(funcIntArray)
                                                 : describeFunction(funcIntList);
        break;
    case kTypeDouble:
        state = (container == kContainerArray) ? describeFunction(funcDoubleArray)
                                                 : describeFunction(funcDoubleList);
        break;
    case kTypeComplex:
        state = (container == kContainerArray) ? describeFunction(funcComplexArray)
                                                 : describeFunction(funcComplexList);
        break;
    }
    textState->setPlainText(state);
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle(APP_TITLE);
    setMinimumSize(800, 660);
    setStyleSheet(piecewiseStyle());
    setupUi();
    onOperationChanged();
    updateFunctionState();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(16, 16, 16, 16);

    groupSettings = new QGroupBox(TEXT_SETTINGS);
    QFormLayout *setLay = new QFormLayout(groupSettings);
    comboDataType = new QComboBox();
    comboDataType->addItem(TYPE_INT, kTypeInt);
    comboDataType->addItem(TYPE_DOUBLE, kTypeDouble);
    comboDataType->addItem(TYPE_COMPLEX, kTypeComplex);
    comboContainer = new QComboBox();
    comboContainer->addItem(TYPE_CONTAINER_ARRAY, kContainerArray);
    comboContainer->addItem(TYPE_CONTAINER_LIST, kContainerList);
    comboOperation = new QComboBox();
    comboOperation->addItem(OP_SPECIFY, kOpSpecify);
    comboOperation->addItem(OP_MONOTONIC, kOpMonotonic);
    comboOperation->addItem(OP_CONTINUOUS, kOpContinuous);
    comboOperation->addItem(OP_ESTIMATE, kOpEstimate);
    comboOperation->addItem(OP_INTERPOLATE, kOpInterpolate);
    setLay->addRow(TEXT_DATA_TYPE, comboDataType);
    setLay->addRow(TEXT_CONTAINER, comboContainer);
    setLay->addRow(TEXT_OPERATION, comboOperation);
    mainLayout->addWidget(groupSettings);

    groupFragment = new QGroupBox(TEXT_GROUP_FRAGMENT);
    QFormLayout *fragLay = new QFormLayout(groupFragment);
    labelBegin = new QLabel(TEXT_BEGIN);
    lineBegin = new QLineEdit();
    lineBegin->setPlaceholderText("0");
    fragLay->addRow(labelBegin, lineBegin);
    labelEnd = new QLabel(TEXT_END);
    lineEnd = new QLineEdit();
    lineEnd->setPlaceholderText("1");
    fragLay->addRow(labelEnd, lineEnd);
    labelCoeffs = new QLabel(TEXT_COEFFS);
    lineCoeffs = new QLineEdit();
    lineCoeffs->setPlaceholderText(PLACEHOLDER_COEFFS_REAL);
    fragLay->addRow(labelCoeffs, lineCoeffs);
    mainLayout->addWidget(groupFragment);

    groupPoint = new QGroupBox(TEXT_GROUP_POINT);
    QFormLayout *pointLay = new QFormLayout(groupPoint);
    labelPointX = new QLabel(TEXT_POINT_X);
    linePointX = new QLineEdit();
    linePointX->setPlaceholderText("0.5");
    pointLay->addRow(labelPointX, linePointX);
    mainLayout->addWidget(groupPoint);

    groupPoints = new QGroupBox(TEXT_GROUP_POINTS);
    QVBoxLayout *pointsLay = new QVBoxLayout(groupPoints);
    textPoints = new QTextEdit();
    textPoints->setPlaceholderText(PLACEHOLDER_POINTS);
    textPoints->setFixedHeight(100);
    pointsLay->addWidget(textPoints);
    mainLayout->addWidget(groupPoints);

    QHBoxLayout *btnLay = new QHBoxLayout();
    btnRun = new QPushButton(TEXT_RUN);
    btnReset = new QPushButton(TEXT_RESET);
    btnLay->addWidget(btnRun);
    btnLay->addWidget(btnReset);
    mainLayout->addLayout(btnLay);

    groupResult = new QGroupBox(TEXT_GROUP_RESULT);
    QVBoxLayout *resLay = new QVBoxLayout(groupResult);
    textResult = new QTextEdit();
    textResult->setReadOnly(true);
    resLay->addWidget(textResult);
    mainLayout->addWidget(groupResult);

    groupState = new QGroupBox(TEXT_GROUP_STATE);
    QVBoxLayout *stateLay = new QVBoxLayout(groupState);
    textState = new QTextEdit();
    textState->setReadOnly(true);
    stateLay->addWidget(textState);
    mainLayout->addWidget(groupState);

    connect(comboOperation, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onOperationChanged);
    connect(comboDataType, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onDataTypeChanged);
    connect(comboContainer, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::updateFunctionState);
    connect(btnRun, &QPushButton::clicked, this, &MainWindow::onRun);
    connect(btnReset, &QPushButton::clicked, this, &MainWindow::onReset);
}

void MainWindow::updateVisibility() {
    int op = comboOperation->currentData().toInt();

    groupFragment->setVisible(op == kOpSpecify);
    groupPoint->setVisible(op == kOpEstimate);
    groupPoints->setVisible(op == kOpInterpolate);

    if (op == kOpInterpolate) {
        int currentType = comboDataType->currentData().toInt();
        if (currentType == kTypeInt) {
            int idx = comboDataType->findData(kTypeDouble);
            if (idx >= 0) comboDataType->setCurrentIndex(idx);
        }
    }

    int type = comboDataType->currentData().toInt();

    lineCoeffs->setPlaceholderText(
        type == kTypeComplex ? PLACEHOLDER_COEFFS_COMPLEX : PLACEHOLDER_COEFFS_REAL);

    if (op == kOpInterpolate) {
        textPoints->setPlaceholderText(
            type == kTypeComplex ? PLACEHOLDER_POINTS_COMPLEX : PLACEHOLDER_POINTS);
    }
}

void MainWindow::onOperationChanged() {
    updateVisibility();
}

void MainWindow::onDataTypeChanged() {
    updateVisibility();
    updateFunctionState();
}

void MainWindow::onReset() {
    int type = comboDataType->currentData().toInt();
    int container = comboContainer->currentData().toInt();
    switch (type) {
    case kTypeInt:
        if (container == kContainerArray) funcIntArray = PieceWiseFunction<int, MutableArraySequence>();
        else                              funcIntList = PieceWiseFunction<int, MutableListSequence>();
        break;
    case kTypeDouble:
        if (container == kContainerArray) funcDoubleArray = PieceWiseFunction<double, MutableArraySequence>();
        else                              funcDoubleList = PieceWiseFunction<double, MutableListSequence>();
        break;
    case kTypeComplex:
        if (container == kContainerArray) funcComplexArray = PieceWiseFunction<Complex, MutableArraySequence>();
        else                              funcComplexList = PieceWiseFunction<Complex, MutableListSequence>();
        break;
    }
    textResult->setPlainText(MSG_RESET_DONE);
    updateFunctionState();
}

void MainWindow::onRun() {
    try {
        int type = comboDataType->currentData().toInt();
        int container = comboContainer->currentData().toInt();
        int op = comboOperation->currentData().toInt();
        bool useArray = (container == kContainerArray);

        QString resultLine;

        switch (op) {
        case kOpSpecify: {
            switch (type) {
            case kTypeInt:
                if (useArray) doFurtherSpecification(funcIntArray);
                else          doFurtherSpecification(funcIntList);
                break;
            case kTypeDouble:
                if (useArray) doFurtherSpecification(funcDoubleArray);
                else          doFurtherSpecification(funcDoubleList);
                break;
            case kTypeComplex:
                if (useArray) doFurtherSpecification(funcComplexArray);
                else          doFurtherSpecification(funcComplexList);
                break;
            }
            resultLine = MSG_SPECIFY_DONE;
            break;
        }
        case kOpMonotonic: {
            bool mono = false;
            switch (type) {
            case kTypeInt:     mono = useArray ? funcIntArray.IsMonotonic() : funcIntList.IsMonotonic(); break;
            case kTypeDouble:  mono = useArray ? funcDoubleArray.IsMonotonic() : funcDoubleList.IsMonotonic(); break;
            case kTypeComplex: mono = useArray ? funcComplexArray.IsMonotonic() : funcComplexList.IsMonotonic(); break;
            }
            resultLine = mono ? MSG_MONOTONIC_YES : MSG_MONOTONIC_NO;
            break;
        }
        case kOpContinuous: {
            bool cont = false;
            switch (type) {
            case kTypeInt:     cont = useArray ? funcIntArray.IsContinuous() : funcIntList.IsContinuous(); break;
            case kTypeDouble:  cont = useArray ? funcDoubleArray.IsContinuous() : funcDoubleList.IsContinuous(); break;
            case kTypeComplex: cont = useArray ? funcComplexArray.IsContinuous() : funcComplexList.IsContinuous(); break;
            }
            resultLine = cont ? MSG_CONTINUOUS_YES : MSG_CONTINUOUS_NO;
            break;
        }
        case kOpEstimate: {
            bool okP = false;
            double point = linePointX->text().toDouble(&okP);
            if (!okP) throw LogicErrorException("Некорректная точка x");

            QString value;
            switch (type) {
            case kTypeInt:
                value = useArray ? doEstimation(funcIntArray, point) : doEstimation(funcIntList, point);
                break;
            case kTypeDouble:
                value = useArray ? doEstimation(funcDoubleArray, point) : doEstimation(funcDoubleList, point);
                break;
            case kTypeComplex:
                value = useArray ? doEstimation(funcComplexArray, point) : doEstimation(funcComplexList, point);
                break;
            }
            resultLine = QString("f(%1) = %2").arg(QString::number(point), value);
            break;
        }
        case kOpInterpolate: {
            if (type == kTypeComplex) {
                std::vector<ComplexPoint> points = parseComplexPoints(textPoints->toPlainText());
                if (useArray) {
                    funcComplexArray = CubicSplineInterpolation<MutableArraySequence>(points.data(), points.size());
                } else {
                    funcComplexList = CubicSplineInterpolation<MutableListSequence>(points.data(), points.size());
                }
                resultLine = QString(MSG_INTERPOLATE_DONE).arg(points.size());
            } else {
                std::vector<Point> points = parsePoints(textPoints->toPlainText());
                if (useArray) {
                    funcDoubleArray = CubicSplineInterpolation<MutableArraySequence>(points.data(), points.size());
                } else {
                    funcDoubleList = CubicSplineInterpolation<MutableListSequence>(points.data(), points.size());
                }
                resultLine = QString(MSG_INTERPOLATE_DONE).arg(points.size());
            }
            break;
        }
        default:
            resultLine = "Неизвестная операция";
        }

        textResult->setPlainText(resultLine);
        updateFunctionState();

    } catch (const LogicErrorException& e) {
        QMessageBox::warning(this, ERR_TITLE, e.what());
    } catch (const std::exception& e) {
        QMessageBox::warning(this, ERR_TITLE, e.what());
    } catch (...) {
        QMessageBox::warning(this, ERR_TITLE, ERR_INVALID_INPUT);
    }
}