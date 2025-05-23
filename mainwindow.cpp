//mainwindow.cpp
#include "mainwindow.h"
//#include "CanonicalLRParser.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    createConnections();

    // Window properties
    setWindowTitle("CLR Parser GUI");
    resize(800, 600);
}

MainWindow::~MainWindow()
{
    // Qt handles deletion of child widgets automatically
}

void MainWindow::setupUI()
{
    // Main central widget and layout
    QWidget *centralWidget = new QWidget(this);
    mainLayout = new QVBoxLayout(centralWidget);

    // Grammar Group (unchanged)
    grammarGroup = new QGroupBox("Grammar Input", centralWidget);
    grammarLayout = new QVBoxLayout(grammarGroup);
    grammarButtonLayout = new QHBoxLayout();
    loadGrammarButton = new QPushButton("Load Grammar", grammarGroup);
    generateButton = new QPushButton("Generate Parser", grammarGroup);
    grammarButtonLayout->addWidget(loadGrammarButton);
    grammarButtonLayout->addWidget(generateButton);
    grammarInputEdit = new QPlainTextEdit(grammarGroup);
    grammarInputEdit->setPlainText("Enter your grammar here (e.g., E -> E + T | T\nT -> T * F | F\nF -> ( E ) | id)");
    grammarLayout->addLayout(grammarButtonLayout);
    grammarLayout->addWidget(grammarInputEdit);

    // Input Group (unchanged)
    inputGroup = new QGroupBox("Input String", centralWidget);
    inputLayout = new QVBoxLayout(inputGroup);
    inputButtonLayout = new QHBoxLayout();
    loadInputButton = new QPushButton("Load Input", inputGroup);
    simulateButton = new QPushButton("Simulate Parsing", inputGroup);
    inputButtonLayout->addWidget(loadInputButton);
    inputButtonLayout->addWidget(simulateButton);
    inputStringEdit = new QLineEdit(inputGroup);
    inputStringEdit->setPlaceholderText("Enter input string to parse (e.g., id + id * id)");
    inputLayout->addLayout(inputButtonLayout);
    inputLayout->addWidget(inputStringEdit);

    // New Output Sections
    QHBoxLayout *outputLayout = new QHBoxLayout();

    // Left Output - Grammar and First/Follow
    QGroupBox *grammarOutputGroup = new QGroupBox("Grammar Analysis", centralWidget);
    QVBoxLayout *grammarOutputLayout = new QVBoxLayout(grammarOutputGroup);
    grammarOutputDisplay = new QPlainTextEdit(grammarOutputGroup);
    grammarOutputDisplay->setReadOnly(true);
    grammarOutputDisplay->setPlainText("Grammar and First/Follow sets will appear here...");
    grammarOutputLayout->addWidget(grammarOutputDisplay);

    // Middle Output - Parse Tables
    QGroupBox *tableOutputGroup = new QGroupBox("Parse Tables", centralWidget);
    QVBoxLayout *tableOutputLayout = new QVBoxLayout(tableOutputGroup);
    tableOutputDisplay = new QPlainTextEdit(tableOutputGroup);
    tableOutputDisplay->setReadOnly(true);
    tableOutputDisplay->setPlainText("ACTION and GOTO tables will appear here...");
    tableOutputLayout->addWidget(tableOutputDisplay);

    // Right Output - Parsing Simulation
    QGroupBox *simulationOutputGroup = new QGroupBox("Parsing Simulation", centralWidget);
    QVBoxLayout *simulationOutputLayout = new QVBoxLayout(simulationOutputGroup);
    simulationOutputDisplay = new QPlainTextEdit(simulationOutputGroup);
    simulationOutputDisplay->setReadOnly(true);
    simulationOutputDisplay->setPlainText("Parsing steps will appear here...");
    /*****************************************/
    // Add navigation buttons to simulation group
    QHBoxLayout *simulationButtonLayout = new QHBoxLayout();
    previousStepButton = new QPushButton("<< Previous", simulationOutputGroup);
    nextStepButton = new QPushButton("Next >>", simulationOutputGroup);
    resetButton = new QPushButton("Reset", simulationOutputGroup);

    simulationButtonLayout->addWidget(previousStepButton);
    simulationButtonLayout->addWidget(resetButton);
    simulationButtonLayout->addWidget(nextStepButton);

    simulationOutputLayout->addLayout(simulationButtonLayout);
    /*****************************************/
    simulationOutputLayout->addWidget(simulationOutputDisplay);

    // Add output groups to the layout
    outputLayout->addWidget(grammarOutputGroup);
    outputLayout->addWidget(tableOutputGroup);
    outputLayout->addWidget(simulationOutputGroup);

    // Set stretch factors to make them equal width
    outputLayout->setStretch(0, 1);
    outputLayout->setStretch(1, 1);
    outputLayout->setStretch(2, 1);

    // Add groups to main layout
    mainLayout->addWidget(grammarGroup);
    mainLayout->addWidget(inputGroup);
    mainLayout->addLayout(outputLayout);

    setCentralWidget(centralWidget);
}

void MainWindow::createConnections()
{
    connect(generateButton, &QPushButton::clicked, this, &MainWindow::onGenerateClicked);
    connect(simulateButton, &QPushButton::clicked, this, &MainWindow::onSimulateClicked);
    connect(loadGrammarButton, &QPushButton::clicked, this, &MainWindow::onLoadGrammarClicked);
    connect(loadInputButton, &QPushButton::clicked, this, &MainWindow::onLoadInputClicked);
    /******************************************/
    connect(nextStepButton, &QPushButton::clicked, this, &MainWindow::onNextStepClicked);
    connect(previousStepButton, &QPushButton::clicked, this, &MainWindow::onPreviousStepClicked);
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::onResetClicked);
    /*******************************************/
}

void MainWindow::onLoadGrammarClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Grammar File", "", "Text Files (*.txt)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            grammarInputEdit->setPlainText(file.readAll());
            file.close();
        }
    }
}

void MainWindow::onLoadInputClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Input File", "", "Text Files (*.txt)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            inputStringEdit->setText(file.readLine().trimmed());
            file.close();
        }
    }
}

void MainWindow::onGenerateClicked()
{
    QString grammarText = grammarInputEdit->toPlainText();
    if (grammarText.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter a grammar.");
        return;
    }

    // Save grammar to temporary file
    QFile grammarFile("grammar.txt");
    if (grammarFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&grammarFile);
        out << grammarText;
        grammarFile.close();
    }
    try {
        parser.run();

        // Display grammar analysis in left panel
        grammarOutputDisplay->setPlainText(QString::fromStdString(parser.getOutput()));
        parser.clearOutput();//new
        parser.generateParseTable();
        // Display parse tables in middle panel
        tableOutputDisplay->setPlainText(QString::fromStdString(parser.getOutput()));
        parser.clearOutput();

    } catch (std::exception &e) {
        QMessageBox::critical(this, "Error", e.what());
    }
}

/*void MainWindow::onSimulateClicked()
{
    QString inputString = inputStringEdit->text();
    if (inputString.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter an input string.");
        return;
    }

    // Save input to temporary file
    QFile inputFile("input.txt");
    if (inputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&inputFile);
        out << inputString;
        inputFile.close();
    }

    try {
        parser.simulateParser();
        // Display parsing steps in right panel
        simulationOutputDisplay->setPlainText(QString::fromStdString(parser.getOutput()));
        parser.clearOutput();
    } catch (std::exception &e) {
        QMessageBox::critical(this, "Error", e.what());
    }
}*/
/**********************************************************/

void MainWindow::updateSimulationButtons()
{
    nextStepButton->setEnabled(parser.hasNextStep());
    previousStepButton->setEnabled(parser.hasPreviousStep());
}

void MainWindow::onNextStepClicked()
{
    parser.nextStep();
    /*simulationOutputDisplay->setPlainText(QString::fromStdString(parser.getCurrentStepOutput()));
    updateSimulationButtons();*/
    updateSimulationDisplay();
}

void MainWindow::onPreviousStepClicked()
{
    parser.previousStep();
    /*simulationOutputDisplay->setPlainText(QString::fromStdString(parser.getCurrentStepOutput()));
    updateSimulationButtons();*/
    updateSimulationDisplay();
}

void MainWindow::onResetClicked()
{
    parser.resetSimulation();
    /*simulationOutputDisplay->setPlainText(QString::fromStdString(parser.getCurrentStepOutput()));
    updateSimulationButtons();*/
    updateSimulationDisplay();
}

void MainWindow::onSimulateClicked()
{
    QString inputString = inputStringEdit->text();
    if (inputString.trimmed().isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter an input string.");
        return;
    }

    // Save input to temporary file
    QFile inputFile("input.txt");
    if (inputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&inputFile);
        out << inputString;
        inputFile.close();
    }

    try {
        parser.simulateParser();
        /*simulationOutputDisplay->setPlainText(QString::fromStdString(parser.getCurrentStepOutput()));
        updateSimulationButtons();*/
        updateSimulationDisplay();
    } catch (std::exception &e) {
        QMessageBox::critical(this, "Error", e.what());
    }
}

void MainWindow::updateSimulationDisplay() {
    std::string output = parser.getCurrentStepOutput();

    // Convert to HTML for better formatting
    QString htmlOutput;
    htmlOutput += "<pre style='font-family: monospace; font-size: 12pt;'>";

    // Split into lines and process each line
    std::istringstream iss(output);
    std::string line;
    while (std::getline(iss, line)) {
        // Highlight action lines
        if (line.find("Action:") != std::string::npos) {
            htmlOutput += "<span style='color: blue; font-weight: bold;'>" + QString::fromStdString(line) + "</span><br>";
        }
        // Highlight success/error messages
        else if (line.find("Parsing successful") != std::string::npos) {
            htmlOutput += "<span style='color: green; font-weight: bold;'>" + QString::fromStdString(line) + "</span><br>";
        }
        else if (line.find("Error:") != std::string::npos) {
            htmlOutput += "<span style='color: red; font-weight: bold;'>" + QString::fromStdString(line) + "</span><br>";
        }
        // Format stack lines
        else if (line.find("┌") != std::string::npos ||
                 line.find("├") != std::string::npos ||
                 line.find("└") != std::string::npos ||
                 line.find("│") != std::string::npos) {
            htmlOutput += "<span style='color: #555;'>" + QString::fromStdString(line) + "</span><br>";
        }
        else {
            htmlOutput += QString::fromStdString(line) + "<br>";
        }
    }

    htmlOutput += "</pre>";

    simulationOutputDisplay->clear();
    simulationOutputDisplay->appendHtml(htmlOutput);
    updateSimulationButtons();
}
