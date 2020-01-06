#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <QColor>
#include <QLabel>
#include <QMainWindow>
#include <QPixmap>
#include <QResizeEvent>
#include <QRgb>
#include <QDebug>
#include <QMessageBox>
#include <time.h>
#include <bits/stdc++.h>
#include <QTableWidget>
#include <QVector>
#include <QPair>
#include <QLinkedList>
#include <QDateTime>

#define DB qDebug()

typedef QPair<int,int> Pair;
typedef QVector<int> VectorInt;
typedef QVector<bool> VectorBool;
typedef QVector<Pair> VectorPair;
typedef QVector<QVector<int> > BoardInt;
typedef QVector<QVector<bool> > BoardFlags;
typedef QVector<QVector<Pair> > BoardPair;

class TableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit TableWidget(QWidget *parent = 0) : QTableWidget(parent) { }
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE {
        emit press((int)e->button());
        QTableWidget::mousePressEvent(e);
    }
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE {
        emit release((int)e->button());
        QTableWidget::mouseReleaseEvent(e);
    }

signals:
    void press(int); // left 1, right 2
    void release(int); // left 1, right 2
};

namespace Ui {
class Minesweeper;
}

class Minesweeper : public QMainWindow
{
    Q_OBJECT

public:
    explicit Minesweeper(QWidget *parent = 0);
    int mine();
    int num(int,int);
    void generar();
    void check(int,int);
    void perder();
    void win();
    void resize();
    ~Minesweeper();

public slots:
    // Click on a given square, given i and j
    void ClickOn(int i, int j);

    // Manually flag some mine
    void FlagOn(int i, int j);

    // Click on it with both mouse buttons in order to "chord"
    void ChordOn(int i, int j);

    // Try to read what number's in this position
    int Detect(int i, int j);

    // Remove the need for edge detection every fricking time
    int OnScreen(int i, int j);

    // take a screenshot and update the onScreen array
    int UpdateOnScreen();

    // tries to detect problems with screenshotting
    bool CheckConsistency();

    // Handle clicking the first square
    void FirstSquare();

    // How many flags exist around this square?
    int CountFlagsAround(BoardFlags array, int i, int j);

    // How many unopened squares around this square?
    int CountFreeSquaresAround(BoardInt board, int i, int j);

    // A boundry square is an unopened square with opened squares near it.
    bool IsBoundry(BoardInt board, int i, int j);

    // Attempt to deduce squares that we know have mines
    // More specifically if number of squares around it = its number
    void AttemptFlagMine();

    // Attempt to deduce a spot that should be free and click it
    // More specifically:
    // Find a square where the number of flags around it is the same as it
    // Then click every empty square around it
    void AttemptMove();

    // Exactly what it says on the tin
    void GuessRandomly();

    // TANK solver: slow and heavyweight backtrack solver designed to
    // solve any conceivable position! (in development)
    void TankSolver();

    // Segregation routine: if two regions are independant then consider
    // them as separate regions
    BoardPair TankSegregate(VectorPair borderTiles);

    // Recurse from depth k (0 is root)
    // Assumes the tank variables are already set; puts solutions in
    // the static arraylist.
    void TankRecurse(VectorPair borderTiles, int k);


private slots:
    void on_tableWidget_cellClicked(int,int);
    void on_tableWidget_cellPressed(int,int);
    void resizeEvent(QResizeEvent *);
    void onpress(int); // left 1, right 2
    void onrelease(int); // left 1, right 2
    void on_niv1_clicked();
    void on_niv2_clicked();
    void on_niv3_clicked();
    void on_nuevo_clicked();
    void on_play_clicked();
    void on_tank_clicked();
    void on_rand_clicked();
    void on_board_clicked();
    void on_flag_clicked();
    void on_mine_clicked();
    void on_boardHeight_valueChanged(int arg1);
    void on_boardWidth_valueChanged(int arg1);
    void on_repetir_clicked();

private:
    Ui::Minesweeper *ui;

    // Internal representation of the board state as displayed on the screen.
    // 1-8 means that the square there is that number
    // 0 means that it's actually empty
    // -1 means it's not opened yet
    // -2 means it's outside the boundries of the board
    // -3 means a mine
    // -10 means that something went wrong and we should exit the program
    BoardInt onScreen;

    // List of squares in which we know there are mines
    BoardFlags flags;

    int numMines = 0;
    int TOT_MINES = 99;

    BoardInt tank_board;
    BoardFlags knownMine;
    BoardFlags knownEmpty;
    BoardFlags tank_solutions;

    // Should be true -- if false, we're bruteforcing the endgame
    bool borderOptimization;
    int BF_LIMIT = 8;

    BoardInt numeros;
    BoardInt minas;
    BoardInt mark;
    BoardInt posible;

    int marked,clock;
    int first;
    QBrush Cell[10];
    QBrush Number[10];
    QFont font, symb;
    bool finalizado;
    int mouse;

    int boardHeight = 1;
    int boardWidth = 1;

    enum BackgroundCell {
        Blank,
        Mark1,
        Mark2,
        Block,
        Mine
    };

    enum ForegroundNumber {
        white,
        blue,
        green,
        red,
        cyan,
        magenta,
        yellow,
        orange,
        black
    };
};

#endif // MINESWEEPER_H
