#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "CanonicalLRParser.h"

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>  // Add this include

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onGenerateClicked();
    void onSimulateClicked();
    void onLoadGrammarClicked();
    void onLoadInputClicked();

    /**********************************/
    void onNextStepClicked();
    void onPreviousStepClicked();
    void onResetClicked();
    /************************************/

private:
    // Input Widgets
    QPlainTextEdit *grammarInputEdit;
    QLineEdit *inputStringEdit;

    // Output Widgets (updated for three panels)
    QPlainTextEdit *grammarOutputDisplay;  // For grammar and First/Follow sets
    QPlainTextEdit *tableOutputDisplay;    // For ACTION/GOTO tables
    QPlainTextEdit *simulationOutputDisplay; // For parsing steps

    // Buttons
    QPushButton *loadGrammarButton;
    QPushButton *generateButton;
    QPushButton *loadInputButton;
    QPushButton *simulateButton;

    // Layouts
    QVBoxLayout *mainLayout;
    QVBoxLayout *grammarLayout;
    QVBoxLayout *inputLayout;
    QHBoxLayout *outputLayout;  // Changed to HBox for side-by-side panels
    QVBoxLayout *grammarOutputLayout;
    QVBoxLayout *tableOutputLayout;
    QVBoxLayout *simulationOutputLayout;
    QHBoxLayout *grammarButtonLayout;
    QHBoxLayout *inputButtonLayout;

    // Group Boxes
    QGroupBox *grammarGroup;
    QGroupBox *inputGroup;
    QGroupBox *grammarOutputGroup;  // New group for grammar analysis
    QGroupBox *tableOutputGroup;    // New group for parse tables
    QGroupBox *simulationOutputGroup; // New group for parsing simulation

    CanonicalLRParser parser;
    /********************************/
    // Navigation buttons
    QPushButton *nextStepButton;
    QPushButton *previousStepButton;
    QPushButton *resetButton;

    /****************************/

    void setupUI();
    void createConnections();
    /***************/
    void updateSimulationButtons();
    void updateSimulationDisplay();
    /*********************/
};

#endif // MAINWINDOW_H
