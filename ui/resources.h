#pragma once
#include <QString>

#define APP_TITLE           "Piecewise function calculator"

#define TEXT_SETTINGS       "Настройки"
#define TEXT_DATA_TYPE      "Тип данных"
#define TEXT_CONTAINER      "Тип последовательности"
#define TEXT_OPERATION      "Операция"

#define TYPE_INT            "int"
#define TYPE_DOUBLE         "double"
#define TYPE_COMPLEX        "complex"

#define TYPE_CONTAINER_ARRAY "Массив (ArraySequence)"
#define TYPE_CONTAINER_LIST  "Список (ListSequence)"

#define OP_SPECIFY          "Доопределение / переопределение на отрезке"
#define OP_MONOTONIC        "Проверка на монотонность"
#define OP_CONTINUOUS       "Проверка на непрерывность"
#define OP_ESTIMATE         "Вычисление значения в точке"
#define OP_INTERPOLATE      "Интерполяция по точкам (кубический сплайн)"

#define TEXT_GROUP_FRAGMENT "Фрагмент"
#define TEXT_BEGIN          "Начало отрезка"
#define TEXT_END            "Конец отрезка"
#define TEXT_COEFFS         "Коэффициенты (по возрастанию степени)"
#define PLACEHOLDER_COEFFS_REAL     "Например: 1 2 3 4  ->  1 + 2x + 3x^2 + 4x^3"
#define PLACEHOLDER_COEFFS_COMPLEX  "Например: (1;2) (3;-1)  ->  (1+2i) + (3-1i)x"

#define TEXT_GROUP_POINT    "Точка"
#define TEXT_POINT_X        "x"

#define TEXT_GROUP_POINTS   "Точки для интерполяции"
#define PLACEHOLDER_POINTS  "По одной точке на строку, например: 1 2"
#define PLACEHOLDER_POINTS_COMPLEX "По одной точке на строку: x Re(y) Im(y), например: 1 2 3"

#define TEXT_RUN            "Выполнить"
#define TEXT_RESET          "Сбросить функцию текущего типа"
#define TEXT_GROUP_RESULT   "Результат"
#define TEXT_GROUP_STATE    "Функция:"

#define ERR_TITLE           "Ошибка"
#define ERR_INVALID_INPUT   "Некорректный ввод или неверная операция"

#define MSG_SPECIFY_DONE    "Фрагмент добавлен / переопределён."
#define MSG_RESET_DONE      "Функция текущего типа сброшена."
#define MSG_INTERPOLATE_DONE "Построен кубический сплайн по %1 точкам."
#define MSG_MONOTONIC_YES   "Функция монотонна."
#define MSG_MONOTONIC_NO    "Функция немонотонна."
#define MSG_CONTINUOUS_YES  "Функция непрерывна."
#define MSG_CONTINUOUS_NO   "Функция разрывна (или не определена на части области)."

inline QString piecewiseStyle() {
    return R"(
        QMainWindow, QWidget {
            background-color: #1e1e1e;
            color: #ffffff;
        }
        QGroupBox {
            color: #ffffff;
            border: 1px solid #555555;
            border-radius: 4px;
            margin-top: 10px;
            padding-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 3px 0 3px;
        }
        QLineEdit, QTextEdit, QComboBox {
            background-color: #2d2d2d;
            color: #ffffff;
            border: 1px solid #555555;
            border-radius: 3px;
            padding: 4px;
        }
        QPushButton {
            background-color: #0d6efd;
            color: #ffffff;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #0b5ed7;
        }
        QPushButton:pressed {
            background-color: #0a58ca;
        }
        QPushButton:disabled {
            background-color: #3a3a3a;
            color: #888888;
        }
        QComboBox:disabled, QLineEdit:disabled {
            color: #888888;
        }
        QComboBox::drop-down {
            border: 0px;
        }
        QComboBox QAbstractItemView {
            background-color: #2d2d2d;
            color: #ffffff;
            selection-background-color: #0d6efd;
        }
    )";
}