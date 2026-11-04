# Документация к лабораторной работе: печать графиков

Приложение на C++/Qt для построения графиков по данным из файлов и печати их в PDF. Разработано в рамках курса «Технологии программирования» (4 курс, 2 семестр).

## Описание

Пользователь выбирает папку и файл с данными. Программа не показывает сами данные, а строит по ним график. Тип графика (столбчатая, круговая или линейная диаграмма) и стиль (цветной или чёрно-белый) можно менять. Готовый график сохраняется в PDF, место сохранения выбирает пользователь.

Связность архитектуры снижена с помощью принципа внедрения зависимостей. Зависимости внедряются через IoC-контейнер.

## Требования по заданию

1. Реализовать внедрение зависимостей с помощью IoC-контейнера.
2. Предусмотреть добавление новых графиков (графики отличаются видом и данными).
3. Предусмотреть изменение визуального стиля графиков (цветной, чёрно-белый).
4. Загружать данные, выбирая нужный файл. Данные не отображать, отображать только график.
5. При печати в PDF давать выбрать место сохранения.

## Технологии

- Qt 5 (модули Widgets, Charts, Sql)
- C++17
- CMake 3.16 / qMake

## Формат входных данных

Данные — набор пар `[дата, значение]`.

**SQLite** (`.sqlite`) — из базы читается первая таблица со столбцами `Time` и `Value`:

```sql
CREATE TABLE [HUMIDITY_MOSCOW] ([Time] Datetime, [Value] Float)
```

**JSON** (`.json`) — массив точек с полями `time` (строка) и `value` (число):

```json
[
  { "time": "01.2024", "value": -14.2 },
  { "time": "02.2024", "value": -12.5 }
]
```

## Структура проекта

```
widgets/
├── main.cpp      точка входа: регистрация реализаций по умолчанию в IoC-контейнере
├── ioc/          IoC-контейнер
├── logger/       логирование
├── data/         точки данных и чтение файлов
├── style/        визуальные стили графиков
├── chart/        виды графиков
├── gui/          главное окно
└── test_data/    тестовые данные и тест-кейсы
```

## Архитектура

- `ioc/`
  - `IOCContainer` — IoC-контейнер: хранит для каждого интерфейса фабрику его реализации
- `logger/`
  - `ILogger` — абстрактный интерфейс логирования
  - `ConsoleLogger` — реализация, выводящая сообщения в консоль
- `data/`
  - `DataPoint` — одна точка данных `[дата, значение]`; `groupPoints` — сжатие большого числа точек в группы
  - `IDataReader` — интерфейс чтения данных из файла
  - `SqliteReader` — чтение из базы SQLite
  - `JsonReader` — чтение из JSON-файла
- `style/`
  - `IChartStyle` — интерфейс визуального стиля графика
  - `ColorStyle` — цветной стиль
  - `GrayStyle` — чёрно-белый стиль (оттенки серого)
- `chart/`
  - `IChart` — интерфейс графика
  - `BarChart` — столбчатая диаграмма
  - `PieChart` — круговая диаграмма
  - `LineChart` — линейный график
- `gui/`
  - `MainWindow` — главное окно: список файлов, настройки и область графика

### Внедрение зависимостей

`MainWindow` работает только с интерфейсами `IDataReader`, `IChart` и `IChartStyle` и не создаёт конкретные классы сама. Реализации регистрируются в `IOCContainer`, а окно получает их через `GetObject`:

| Действие пользователя        | Что регистрируется в контейнере                          |
|------------------------------|----------------------------------------------------------|
| запуск программы (`main`)    | `ILogger` → `ConsoleLogger`, `IChart` → `BarChart`, `IChartStyle` → `ColorStyle` |
| выбор файла `.sqlite`        | `IDataReader` → `SqliteReader` (с внедрённым `ILogger`)  |
| выбор файла `.json`          | `IDataReader` → `JsonReader` (с внедрённым `ILogger`)    |
| выбор типа диаграммы         | `IChart` → `BarChart` / `PieChart` / `LineChart`         |
| галочка «Черно-белый график» | `IChartStyle` → `GrayStyle` / `ColorStyle`               |

Перерисовка графика (`redraw`) берёт из контейнера текущие `IChart` и `IChartStyle` и вызывает `buildGraf`.

### Как добавить новый график

1. Создать в папке `chart/` класс-наследник `IChart` и реализовать в нём `buildGraf`.
2. Добавить название в список `chartTypeBox` и одну ветку `RegisterFactory<IChart, НовыйГрафик>()` в `MainWindow::changeChartType`.

Новый стиль или формат файла добавляется так же: наследник `IChartStyle` или `IDataReader` и одна регистрация в контейнере.

## Как работает программа

### Что такое MVC и как он используется

**MVC (Model–View–Controller)** — способ разделить программу на три части, чтобы они меньше зависели друг от друга:

- **Model (модель)** хранит данные и ничего не знает о том, как они показываются.
- **View (представление)** показывает данные пользователю и не меняет их.
- **Controller (контроллер)** принимает действия пользователя (клики, выбор в списке), меняет модель и просит представление обновиться.

Так можно поменять внешний вид (например, столбики на круговую диаграмму) и не трогать данные. А данные можно загрузить из другого источника, не трогая отображение.

В Qt используется вариант **Model/View**: контроллер отдельно не выделяется. Его роль делят представление, которое ловит клики, и модель выделения `QItemSelectionModel`.

В программе MVC встречается на двух уровнях:

| Роль | Список файлов (готовый Qt Model/View) | График (своя схема) |
|------|--------------------------------------|----------------------------|
| **Model** | `QFileSystemModel fileModel`: файлы выбранной папки | `QVector<DataPoint> points` в `MainWindow`: точки загруженного файла |
| **View** | `QTableView fileView`: таблица с файлами | `QChartView chartView`, на котором лежит `QChart`, созданный `IChart::buildGraf` |
| **Controller** | `QItemSelectionModel`: запоминает выбранную строку и шлёт `selectionChanged` | слоты `MainWindow`: `selectFile`, `changeChartType`, `changeStyle`, `printChart` |

Модель графика не отдельный класс, а поле окна. Но правило MVC выполняется: точки не знают, как их нарисуют, а график строится из них заново при каждом изменении и сам данные не меняет.

### Как хранятся данные

- Одна точка — структура `DataPoint { QString time; double value; }`. Дата хранится строкой как есть (`"01.2024"`, `"2023-05-01 12:00"`), потому что используется только как подпись.
- Все точки открытого файла лежат в одном месте, в поле `MainWindow::points` (`QVector<DataPoint>`). Файл читается целиком один раз, при выборе. При смене типа диаграммы или стиля файл не перечитывается, график строится из `points`.
- Если файл не прочитался (ошибка или пустой), `points` не меняется и остаётся последний удачный график.
- Сами `points` никогда не изменяются. Для графика `groupPoints` делает **временную копию**: 10 средних значений для столбчатой и круговой диаграмм, до 1000 для линейного графика.
- Время жизни объектов из контейнера:
  - `ILogger` и `IChartStyle` регистрируются через `RegisterInstance`, это один объект на всё приложение;
  - `IDataReader` и `IChart` регистрируются через `RegisterFactory`, и при каждом `GetObject` создаётся новый объект. Он живёт, пока работает метод, и затем удаляется (`shared_ptr`).
- Новый `QChart` передаётся в `chartView`, а предыдущий удаляется в `redraw()`.

### Путь данных: от файла до графика и PDF

```mermaid
flowchart LR
    file[("Файл .sqlite / .json")]
    reader["IDataReader<br/>SqliteReader / JsonReader"]
    points["MainWindow::points<br/>QVector#lt;DataPoint#gt;"]
    chart["IChart<br/>BarChart / PieChart / LineChart"]
    groups["groupPoints()<br/>временная копия: 10 или 1000 точек"]
    style["IChartStyle<br/>ColorStyle / GrayStyle"]
    qchart["QChart<br/>серии + оси"]
    view["QChartView"]
    pdf[("PDF")]

    file -->|"read(path)"| reader
    reader -->|"QVector#lt;DataPoint#gt;"| points
    points -->|"buildGraf(points, style)"| chart
    chart --> groups
    style -->|"colorAt(i, n)"| chart
    groups --> qchart
    qchart -->|"setChart()"| view
    view -->|"render() в QPdfWriter"| pdf
```

По шагам:

1. **Выбор файла.** `QItemSelectionModel` шлёт `selectionChanged`, и вызывается слот `selectFile`. По расширению он регистрирует в контейнере `SqliteReader` или `JsonReader`.
2. **Чтение.** `GetObject<IDataReader>()` создаёт читателя и передаёт ему `ILogger`. `read(path)` возвращает `QVector<DataPoint>`, и этот вектор сохраняется в `points`.
   - `SqliteReader` открывает базу только для чтения и выполняет `SELECT Time, Value` из первой таблицы.
   - `JsonReader` разбирает массив и проверяет у каждой точки поля `time` (строка) и `value` (число).
3. **Построение.** `redraw()` берёт из контейнера текущие `IChart` и `IChartStyle` и вызывает `buildGraf(points, style)`. График сжимает точки через `groupPoints`, создаёт серию Qt Charts (`QBarSeries` / `QPieSeries` / `QLineSeries`), красит элементы через `style.colorAt(i, n)` и возвращает готовый `QChart`.
4. **Показ.** `chartView->setChart(...)` показывает новый график, старый `QChart` удаляется.
5. **Смена типа или стиля.** В контейнере заменяется регистрация `IChart` / `IChartStyle`, и снова вызывается `redraw()` с теми же `points`. Файл не перечитывается.
6. **Печать.** `printChart` спрашивает путь, создаёт `QPdfWriter` (A4, альбомная) и рисует в него то, что сейчас показывает `chartView` (`render`).

## Сборка

### Qt Creator (qMake)

Открыть файл `widgets.pro` в Qt Creator и собрать проект.

### CMake

```bash
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=<путь к Qt>/5.15.0/mingw81_64
cmake --build .
```

## Использование

1. Нажать «Открыть папку» и выбрать папку с данными (например, `test_data`).
2. Выбрать файл `.sqlite` или `.json` в списке слева — справа построится график.
3. В списке «Выберите тип диаграммы» выбрать `BarChart`, `PieChart` или `LineChart`.
4. Галочкой «Черно-белый график» переключить стиль.
5. Нажать «Печать графика» и выбрать, куда сохранить PDF.

В статус-баре выводится имя файла и число точек или сообщение об ошибке. Подробный лог пишется в консоль.

Чтобы график оставался читаемым, для столбчатой и круговой диаграмм точки усредняются до 10 групп, для линейного графика — до 1000. В круговой диаграмме берётся модуль значения, потому что сектор не может быть отрицательным.

## UML-диаграммы

### Диаграмма классов

Классы сгруппированы по папкам проекта.

```mermaid
classDiagram
    namespace ioc {
        class IOCContainer {
            -factories : map~int, FactoryBase~
            +RegisterInstance~TInterface, TConcrete, TArguments~() void
            +RegisterFactory~TInterface, TConcrete, TArguments~() void
            +RegisterFunctor~TInterface, TS~(functor) void
            +GetObject~T~() shared_ptr~T~
        }
    }

    namespace logger {
        class ILogger {
            <<interface>>
            +logInfo(message : QString) void
            +logError(message : QString) void
        }
        class ConsoleLogger {
            +logInfo(message : QString) void
            +logError(message : QString) void
        }
    }

    namespace data {
        class DataPoint {
            <<struct>>
            +time : QString
            +value : double
        }
        class IDataReader {
            <<interface>>
            +read(path : QString) QVector~DataPoint~
        }
        class SqliteReader {
            -logger : ILogger
            +SqliteReader(logger : ILogger)
            +read(path : QString) QVector~DataPoint~
        }
        class JsonReader {
            -logger : ILogger
            +JsonReader(logger : ILogger)
            +read(path : QString) QVector~DataPoint~
        }
    }

    namespace style {
        class IChartStyle {
            <<interface>>
            +colorAt(i : int, n : int) QColor
        }
        class ColorStyle {
            +colorAt(i : int, n : int) QColor
        }
        class GrayStyle {
            +colorAt(i : int, n : int) QColor
        }
    }

    namespace chart {
        class IChart {
            <<interface>>
            +buildGraf(points : QVector~DataPoint~, style : IChartStyle) QChart
        }
        class BarChart {
            +buildGraf(points : QVector~DataPoint~, style : IChartStyle) QChart
        }
        class PieChart {
            +buildGraf(points : QVector~DataPoint~, style : IChartStyle) QChart
        }
        class LineChart {
            +buildGraf(points : QVector~DataPoint~, style : IChartStyle) QChart
        }
    }

    namespace gui {
        class MainWindow {
            -container : IOCContainer
            -logger : ILogger
            -points : QVector~DataPoint~
            -fileModel : QFileSystemModel
            -fileView : QTableView
            -chartTypeBox : QComboBox
            -grayCheck : QCheckBox
            -chartView : QChartView
            +MainWindow(container : IOCContainer, parent : QWidget)
            -«slot» openFolder() void
            -«slot» selectFile(selected : QItemSelection, deselected : QItemSelection) void
            -«slot» changeChartType(index : int) void
            -«slot» changeStyle(gray : bool) void
            -«slot» printChart() void
            -redraw() void
        }
    }

    ILogger <|.. ConsoleLogger
    IDataReader <|.. SqliteReader
    IDataReader <|.. JsonReader
    IChartStyle <|.. ColorStyle
    IChartStyle <|.. GrayStyle
    IChart <|.. BarChart
    IChart <|.. PieChart
    IChart <|.. LineChart
    SqliteReader "1" --> "1" ILogger : logger
    JsonReader "1" --> "1" ILogger : logger
    MainWindow "1" --> "1" IOCContainer : container
    MainWindow "1" --> "1" ILogger : logger
    MainWindow "1" *-- "0..*" DataPoint : points
    MainWindow ..> IDataReader : use
    MainWindow ..> IChart : use
    MainWindow ..> IChartStyle : use
    IDataReader ..> DataPoint : create
    IChart ..> DataPoint : use
    IChart ..> IChartStyle : use
```

Обозначения: `<|..` — реализация интерфейса, `*--` — композиция (агрегат владеет частью),
`-->` — направленная ассоциация, `..>` — зависимость; в кавычках указана кратность концов связей.

### Диаграмма последовательности

Сценарий «выбор файла → построение графика»: окно регистрирует реализацию в контейнере, получает объекты через `GetObject` и строит график, не зная конкретных классов.

```mermaid
sequenceDiagram
    actor User as Пользователь
    participant W as MainWindow
    participant C as IOCContainer
    participant R as SqliteReader
    participant G as BarChart
    participant S as ColorStyle
    participant V as QChartView

    User->>W: выбирает файл .sqlite в списке
    activate W
    Note over W: слот selectFile()

    W->>C: RegisterFactory: IDataReader → SqliteReader (зависимость ILogger)
    W->>C: GetObject: IDataReader
    C->>C: GetObject: ILogger
    C-->>W: новый SqliteReader(logger)

    W->>R: read(path)
    R-->>W: QVector of DataPoint

    Note over W: redraw()
    W->>C: GetObject: IChart
    C-->>W: BarChart
    W->>C: GetObject: IChartStyle
    C-->>W: ColorStyle

    W->>G: buildGraf(points, style)
    loop для каждого столбика
        G->>S: colorAt(i, n)
        S-->>G: QColor
    end
    G-->>W: QChart

    W->>V: setChart(chart)
    deactivate W
    V-->>User: на экране новый график
```

### Диаграмма сигналов и слотов

Сплошная стрелка — `connect(сигнал → слот)` из конструктора `MainWindow`. Пунктир — прямой вызов метода. Точечная стрелка — сигналы, которые Qt соединяет сам внутри Model/View при `fileView->setModel(fileModel)`.

```mermaid
flowchart LR
    subgraph MV["Qt Model/View (соединяется автоматически)"]
        fileModel["QFileSystemModel fileModel"]
        fileView["QTableView fileView"]
        selectionModel["QItemSelectionModel"]
    end

    subgraph Widgets["Виджеты (источники сигналов)"]
        folderButton["QPushButton «Открыть папку»"]
        chartTypeBox["QComboBox chartTypeBox"]
        grayCheck["QCheckBox grayCheck"]
        printButton["QPushButton «Печать графика»"]
    end

    subgraph Window["MainWindow (слоты)"]
        openFolder["openFolder()"]
        selectFile["selectFile()"]
        changeChartType["changeChartType()"]
        changeStyle["changeStyle()"]
        printChart["printChart()"]
        redraw["redraw()"]
    end

    fileModel -. "rowsInserted(), dataChanged(), layoutChanged()" .-> fileView
    fileView -. "клик по строке → select()" .-> selectionModel

    folderButton -- "clicked()" --> openFolder
    selectionModel -- "selectionChanged(selected, deselected)" --> selectFile
    chartTypeBox -- "currentIndexChanged(int)" --> changeChartType
    grayCheck -- "toggled(bool)" --> changeStyle
    printButton -- "clicked()" --> printChart

    openFolder -. "setRootPath(dir)" .-> fileModel
    selectFile -. "вызов" .-> redraw
    changeChartType -. "вызов" .-> redraw
    changeStyle -. "вызов" .-> redraw
```

| Источник (сигнал)                         | Приёмник (слот)               | Что происходит |
|-------------------------------------------|-------------------------------|----------------|
| `folderButton::clicked()`                 | `MainWindow::openFolder`      | выбор папки, модель файлов перечитывает каталог |
| `QItemSelectionModel::selectionChanged()` | `MainWindow::selectFile`      | чтение файла в `points` и `redraw()` |
| `chartTypeBox::currentIndexChanged(int)`  | `MainWindow::changeChartType` | новая регистрация `IChart` и `redraw()` |
| `grayCheck::toggled(bool)`                | `MainWindow::changeStyle`     | новая регистрация `IChartStyle` и `redraw()` |
| `printButton::clicked()`                  | `MainWindow::printChart`      | сохранение `chartView` в PDF |

## Тестирование

Пользовательские тест-кейсы описаны в [`test_data/README.md`](test_data/README.md).
