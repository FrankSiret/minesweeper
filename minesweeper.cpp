#include "minesweeper.h"
#include "ui_minesweeper.h"

Minesweeper::Minesweeper(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Minesweeper)
{
    ui->setupUi(this);

    qsrand(time(0));
    ui->checkBox->setChecked(0);

    Cell[Mine]  = QBrush(Qt::red);
    Cell[Block] = QBrush(QColor(80,80,80,100));
    Cell[Blank] = QBrush(QColor(255,255,255));
    Cell[Mark1]  = QBrush(Qt::red);
    Cell[Mark2]  = QBrush(Qt::white);

    font = symb = ui->tableWidget->font();
    font.setFamily("Tahoma");
    symb.setFamily("Wingdings");

    Number[white]   = QBrush(Qt::white);
    Number[blue]    = QBrush(Qt::blue);
    Number[green]   = QBrush(Qt::green);
    Number[red]     = QBrush(Qt::red);
    Number[cyan]    = QBrush(Qt::darkCyan);
    Number[magenta] = QBrush(Qt::magenta);
    Number[yellow]  = QBrush(Qt::yellow);
    Number[orange]  = QBrush(QColor(255,128,0));
    Number[black]   = QBrush(QColor(32,32,32));

    connect(ui->tableWidget,&TableWidget::press,this,&Minesweeper::onpress);
    connect(ui->tableWidget,&TableWidget::release,this,&Minesweeper::onrelease);

    mouse = 0;
}

int pi[] = {-1,-1,-1, 0,0, 1,1,1};
int pj[] = {-1, 0, 1,-1,1,-1,0,1};

int Minesweeper::mine()
{
    return qrand() % (boardHeight * boardWidth);
}

int Minesweeper::num(int i, int j)
{
    int res = 0;

    for(int k=0; k<8; k++) {
        if(i+pi[k] < 0 || j+pj[k] < 0 || i+pi[k] >= boardHeight || j+pj[k] >= boardWidth) continue;
        res += minas[i+pi[k]][j+pj[k]];
    }

    return res;
}

void Minesweeper::generar()
{
    for(int i=0; i<boardHeight; i++)
        for(int j=0; j<boardWidth; j++) {
            minas[i][j] = 0;
        }

    int pos;
    for(int c=0; c<TOT_MINES; c++) {
        do {
            pos = mine();
        } while (minas[pos / boardWidth][pos % boardWidth]);
        minas[pos / boardWidth][pos % boardWidth] = 1;
    }

    for(int i=0; i<boardHeight; i++)
        for(int j=0; j<boardWidth; j++) {
            if(minas[i][j]) numeros[i][j] = -1;
            else numeros[i][j] = num(i,j);
        }
}

void Minesweeper::check(int i, int j)
{
    mark[i][j] = 1;
    if(posible[i][j]) {
        posible[i][j] = 0;
        marked --;
    }
    ui->mark->setText(QString::number(marked));

    QTableWidgetItem *it = new QTableWidgetItem();
    if(numeros[i][j] > 0) {
        it->setText(QString::number(numeros[i][j]));
        it->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        it->setBackground(Cell[Blank]);
        it->setForeground(Number[numeros[i][j]]);
        it->setFont(font);
    }
    if(numeros[i][j] == 0) {
        it->setText("");
        it->setBackground(Cell[Blank]);
    }
    if(numeros[i][j] == -1) {
        it->setText("M");
        it->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        it->setBackground(Cell[Mine]);
        it->setFont(symb);
        perder();
    }
    ui->tableWidget->setItem(i,j,it);
}

void Minesweeper::perder()
{
    finalizado = 1;

    for(int i=0; i<boardHeight; i++)
        for(int j=0; j<boardWidth; j++) {
            if(minas[i][j] == 1 && mark[i][j] == 0) {
                ui->tableWidget->item(i,j)->setText("M");
                ui->tableWidget->item(i,j)->setFont(symb);
                if(posible[i][j] == 1)
                    ui->tableWidget->item(i,j)->setForeground(Qt::darkGreen);
                else ui->tableWidget->item(i,j)->setForeground(Cell[Mine]);
            }
        }

    QMessageBox::critical(this,"PERDISTE","HAS PERDIDO, JUEGA DE NUEVO");
}

void Minesweeper::win()
{
    finalizado = 1;

    for(int i=0; i<boardHeight; i++)
        for(int j=0; j<boardWidth; j++) {
            if(minas[i][j] == 1 && mark[i][j] == 0) {
                ui->tableWidget->item(i,j)->setText("M");
                ui->tableWidget->item(i,j)->setFont(symb);
                if(posible[i][j] == 1)
                    ui->tableWidget->item(i,j)->setForeground(Qt::darkGreen);
                else ui->tableWidget->item(i,j)->setForeground(Cell[Mine]);
            }
        }

    QMessageBox::information(this,"GANASTE","HAS GANADO, JUEGA DE NUEVO");
}

Minesweeper::~Minesweeper()
{
    delete ui;
}

void Minesweeper::ClickOn(int i, int j)
{
    on_tableWidget_cellClicked(i, j);
}

void Minesweeper::FlagOn(int i, int j)
{
    on_tableWidget_cellPressed(i, j);
}

void Minesweeper::ChordOn(int i, int j)
{
    if(mark[i][j] && numeros[i][j] == 0) {
        for(int k=0; k<8; k++) {
            int ii = i + pi[k];
            int jj = j + pj[k];
            if(ii < 0 || jj < 0 || ii >= boardHeight || jj >= boardWidth) continue;
            if(!mark[ii][jj] && posible[i][j] == 0) {
                ClickOn(ii, jj);
                ChordOn(ii, jj);
            }
        }
    }
}

int Minesweeper::Detect(int i, int j)
{
    if(mark[i][j]) return numeros[i][j];
    if(posible[i][j] == 1) return -3;
    if(!mark[i][j]) return -1;
    return -10;
}

int Minesweeper::OnScreen(int i, int j)
{
    if(i < 0 || j < 0 || i > boardHeight-1 || j > boardWidth-1)
        return -10;
    return onScreen[i][j];
}

int Minesweeper::UpdateOnScreen()
{
    int numMines_t = 0;
    for(int i = 0; i < boardHeight; i++){
        for(int j=0; j < boardWidth; j++){

            int d = Detect(i,j);

            if(d == -10) return d; // death
            onScreen[i][j] = d;

            // Special case for flags
            if(d == -3 || flags[i][j]){
                onScreen[i][j] = -1;
                flags[i][j] = true;
            }
            if(d == -1){
                flags[i][j] = false;
            }

            // Update mines count
            if(flags[i][j]){
                numMines_t++;
            }
        }
    }

    //if(numMines_t < numMines - 2) exit();
    numMines = numMines_t;
    return 0;
}

bool Minesweeper::CheckConsistency(){
    for(int i=0; i<boardHeight; i++){
        for(int j=0; j<boardWidth; j++){

            int freeSquares = CountFreeSquaresAround(onScreen, i, j);
            int numFlags = CountFlagsAround(flags, i, j);

            if(OnScreen(i,j) == 0 && freeSquares > 0){
                return false;
            }
            if((OnScreen(i,j) - numFlags) > 0 && freeSquares == 0){
                return false;
            }
        }
    }
    return true;
}

void Minesweeper::FirstSquare()
{
    UpdateOnScreen();
    bool isUntouched = true;
    for(int i=0; i<boardHeight; i++){
        for(int j=0; j<boardWidth; j++){
            if(OnScreen(i,j) != -1)
                isUntouched = false;
        }
    }
    if(!isUntouched){
        return;
    }
    // Click the middle
    ClickOn(boardHeight/2-1, boardWidth/2-1);

    for(int i=0; i<boardHeight; i++)
        for(int j=0; j<boardWidth; j++)
            if(mark[i][j] && numeros[i][j] == 0)
                ChordOn(i, j);
}

int Minesweeper::CountFreeSquaresAround(BoardInt, int i, int j) {
    int freeSquares = 0;

    if(OnScreen(i-1,j)==-1) freeSquares++;
    if(OnScreen(i+1,j)==-1) freeSquares++;
    if(OnScreen(i,j-1)==-1) freeSquares++;
    if(OnScreen(i,j+1)==-1) freeSquares++;
    if(OnScreen(i-1,j-1)==-1) freeSquares++;
    if(OnScreen(i-1,j+1)==-1) freeSquares++;
    if(OnScreen(i+1,j-1)==-1) freeSquares++;
    if(OnScreen(i+1,j+1)==-1) freeSquares++;

    return freeSquares;
}

bool Minesweeper::IsBoundry(BoardInt board, int i, int j) {
    if(board[i][j] != -1) return false;

    bool oU = false, oD = false, oL = false, oR = false;
    if(i == 0) oU = true;
    if(j == 0) oL = true;
    if(i == boardHeight-1) oD = true;
    if(j == boardWidth-1) oR = true;
    bool isBoundry = false;

    if(!oU && board[i-1][j]>=0) isBoundry = true;
    if(!oL && board[i][j-1]>=0) isBoundry = true;
    if(!oD && board[i+1][j]>=0) isBoundry = true;
    if(!oR && board[i][j+1]>=0) isBoundry = true;
    if(!oU && !oL && board[i-1][j-1]>=0) isBoundry = true;
    if(!oU && !oR && board[i-1][j+1]>=0) isBoundry = true;
    if(!oD && !oL && board[i+1][j-1]>=0) isBoundry = true;
    if(!oD && !oR && board[i+1][j+1]>=0) isBoundry = true;

    return isBoundry;
}

void Minesweeper::AttemptFlagMine() {

    for(int i=0; i<boardHeight; i++){
        for(int j=0; j<boardWidth; j++){

            if(OnScreen(i,j) >= 1){
                int curNum = OnScreen(i,j);

                // Flag necessary squares
                if(curNum == CountFreeSquaresAround(onScreen,i,j)){
                    for(int ii=0; ii<boardHeight; ii++){
                        for(int jj=0; jj<boardWidth; jj++){
                            if(qAbs(ii-i)<=1 && qAbs(jj-j)<=1){
                                if(OnScreen(ii,jj) == -1 && !flags[ii][jj]){
                                    flags[ii][jj] = true;
                                    FlagOn(ii,jj);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void Minesweeper::AttemptMove() {

    bool success = false;
    for(int i=0; i<boardHeight; i++){
        for(int j=0; j<boardWidth; j++){

            if(OnScreen(i,j) >= 1){

                // Count how many mines around it
                int curNum = onScreen[i][j];
                int mines = CountFlagsAround(flags,i,j);
                int freeSquares = CountFreeSquaresAround(onScreen,i,j);


                // Click on the deduced non-mine squares
                if(curNum == mines && freeSquares > mines){
                    success = true;

                    // Use the chord or the classical algorithm
                    if(freeSquares - mines > 1){
                        ChordOn(i,j);
                        onScreen[i][j] = 0; // hack to make it not overclick a square
                        continue;
                    }

                    // Old algorithm: don't chord
                    for(int ii=0; ii<boardHeight; ii++){
                        for(int jj=0; jj<boardWidth; jj++){
                            if(qAbs(ii-i)<=1 && qAbs(jj-j)<=1){
                                if(OnScreen(ii,jj) == -1 && !flags[ii][jj]){
                                    ClickOn(ii,jj);
                                    onScreen[ii][jj] = 0;
                                }
                            }
                        }
                    }

                }
            }
        }
    }

    if(success) return;

    // Bring in the big guns
    TankSolver();
}

void Minesweeper::GuessRandomly() {
    DB << "Attempting to guess randomly";
    while(true){
        int k = qrand() % (boardHeight * boardWidth);
        int i = k / boardWidth;
        int j = k % boardWidth;

        if(OnScreen(i,j) == -1 && !flags[i][j]){
            ClickOn(i,j);
            return;
        }
    }
}

void Minesweeper::TankSolver()
{
    UpdateOnScreen();

    //dumpPosition();
    if(!CheckConsistency()) {
        DB << "NO CONSISTENCY";
        return;
    }

    // Timing
    long tankTime = QDateTime::currentMSecsSinceEpoch();

    VectorPair borderTiles;
    VectorPair allEmptyTiles;

    // Endgame case: if there are few enough tiles, don't bother with border tiles.
    borderOptimization = false;
    for(int i=0; i<boardHeight; i++)
        for(int j=0; j<boardWidth; j++)
            if(OnScreen(i,j) == -1 && !flags[i][j])
                allEmptyTiles.push_back(Pair(i,j));

    // Determine all border tiles
    for(int i=0; i<boardHeight; i++)
        for(int j=0; j<boardWidth; j++)
            if(IsBoundry(onScreen,i,j) && !flags[i][j])
                borderTiles.push_back(Pair(i,j));

    // Count how many squares outside the knowable range
    int numOutSquares = allEmptyTiles.size() - borderTiles.size();
    if(numOutSquares > BF_LIMIT){
        borderOptimization = true;
    }
    else borderTiles = allEmptyTiles;


    // Something went wrong
    if(borderTiles.size() == 0)
        return;


    // Run the segregation routine before recursing one by one
    // Don't bother if it's endgame as doing so might make it miss some cases
    BoardPair segregated;
    if(!borderOptimization){
        segregated = BoardPair();
        segregated.push_back(borderTiles);
    }
    else segregated = TankSegregate(borderTiles);

    int totalMultCases = 1;
    bool success = false;
    double prob_best = 0; // Store information about the best probability
    int prob_besttile = -1;
    int prob_best_s = -1;
    for(int s = 0; s < segregated.size(); s++)
    {
        // Copy everything into temporary constructs
        //tank_solutions = new ArrayList<bool[]>();
        tank_solutions = BoardFlags();
        tank_board = onScreen;
        knownMine = flags;

        knownEmpty = BoardFlags(boardHeight, VectorBool(boardWidth));
        for(int i=0; i<boardHeight; i++)
            for(int j=0; j<boardWidth; j++)
                if(tank_board[i][j] >= 0)
                    knownEmpty[i][j] = true;
                else knownEmpty[i][j] = false;


        // Compute solutions -- here's the time consuming step
        TankRecurse(segregated.at(s),0);

        // Something screwed up
        if(tank_solutions.size() == 0) {
            for(int i=0; i<boardHeight; i++)
                for(int j=0; j<boardWidth; j++)
                    if(mark[i][j] && numeros[i][j] == 0)
                        ChordOn(i, j);
            return;
        }


        // Check for solved squares
        for(int i=0; i<segregated.at(s).size(); i++) {
            bool allMine = true;
            bool allEmpty = true;
            for(int t=0; t<tank_solutions.size(); t++) {
                VectorBool sln = tank_solutions.at(t);
                if(!sln[i]) allMine = false;
                if(sln[i]) allEmpty = false;
            }


            Pair q = segregated.at(s).at(i);
            int qi = q.first;
            int qj = q.second;

            // Muahaha
            if(allMine){
                flags[qi][qj] = true;
                FlagOn(qi,qj);
            }
            if(allEmpty){
                success = true;
                ClickOn(qi,qj);
            }
        }

        totalMultCases *= tank_solutions.size();


        // Calculate probabilities, in case we need it
        if(success) continue;
        int maxEmpty = -10000;
        int iEmpty = -1;
        for(int i=0; i<segregated.at(s).size(); i++){
            int nEmpty = 0;
            for(int t=0; t<tank_solutions.size(); t++) {
                VectorBool sln = tank_solutions.at(t);
                if(!sln[i]) nEmpty++;
            }
            if(nEmpty > maxEmpty){
                maxEmpty = nEmpty;
                iEmpty = i;
            }
        }
        double probability = (double)maxEmpty / (double)tank_solutions.size();

        if(probability > prob_best){
            prob_best = probability;
            prob_besttile = iEmpty;
            prob_best_s = s;
        }

    }

    // But wait! If there's any hope, bruteforce harder (by a factor of 32x)!
    if(BF_LIMIT == 8 && numOutSquares > 8 && numOutSquares <= 13){
        DB << "Extending bruteforce horizon...";
        BF_LIMIT = 13;
        TankSolver();
        BF_LIMIT = 8;
        return;
    }

    tankTime = QDateTime::currentMSecsSinceEpoch() - tankTime;
    if(success){
        DB << "TANK Solver successfully invoked at step" << numMines << "(" << tankTime <<  "ms," <<
              totalMultCases << "cases)" << (borderOptimization?"":"*");
        for(int i=0; i<boardHeight; i++)
            for(int j=0; j<boardWidth; j++)
                if(mark[i][j] && numeros[i][j] == 0)
                    ChordOn(i, j);
        return;
    }


    // Take the guess, since we can't deduce anything useful
    DB << "TANK Solver guessing with probability" << prob_best << "at step" << numMines << "(" << tankTime << "ms," <<
          totalMultCases << "cases)" << (borderOptimization?"":"*");

    Pair q = segregated.at(prob_best_s).at(prob_besttile);
    int qi = q.first;
    int qj = q.second;
    ClickOn(qi,qj);

    for(int i=0; i<boardHeight; i++)
        for(int j=0; j<boardWidth; j++)
            if(mark[i][j] && numeros[i][j] == 0)
                ChordOn(i, j);
}

BoardPair Minesweeper::TankSegregate(VectorPair borderTiles)
{
    BoardPair allRegions;
    VectorPair covered;

    while(true){

        QLinkedList<Pair> queue;
        VectorPair finishedRegion;

        // Find a suitable starting point
        for(int t=0; t<borderTiles.size(); t++) {
            Pair firstT = borderTiles.at(t);
            if(!covered.contains(firstT)){
                queue.push_back(firstT);
                break;
            }
        }

        if(queue.isEmpty())
            break;

        while(!queue.isEmpty()){

            Pair curTile = queue.first();
            queue.removeFirst();

            int ci = curTile.first;
            int cj = curTile.second;

            finishedRegion.push_back(curTile);
            covered.push_back(curTile);

            // Find all connecting tiles
            for(int t=0; t<borderTiles.size(); t++) {
                Pair tile = borderTiles.at(t);
                int ti = tile.first;
                int tj = tile.second;

                bool isConnected = false;

                if(finishedRegion.contains(tile))
                    continue;

                if(qAbs(ci-ti)>2 || qAbs(cj-tj) > 2)
                    isConnected = false;

                else{
                    // Perform a search on all the tiles
                    bool tilesearch = 0;
                    for(int i=0; i<boardHeight; i++){
                        for(int j=0; j<boardWidth; j++){
                            if(OnScreen(i,j) > 0){
                                if(qAbs(ci-i) <= 1 && qAbs(cj-j) <= 1 && qAbs(ti-i) <= 1 && qAbs(tj-j) <= 1){
                                    isConnected = true;
                                    tilesearch = 1;
                                    break;
                                }
                            }
                        }
                        if(tilesearch) break;
                    }
                }

                if(!isConnected) continue;

                if(!queue.contains(tile))
                    queue.push_back(tile);

            }
        }

        allRegions.push_back(finishedRegion);

    }

    return allRegions;

}

void Minesweeper::TankRecurse(VectorPair borderTiles, int k) {

    // Return if at this point, it's already inconsistent
    int flagCount = 0;
    for(int i=0; i<boardHeight; i++)
        for(int j=0; j<boardWidth; j++){

            // Count flags for endgame cases
            if(knownMine[i][j])
                flagCount++;

            int num = tank_board[i][j];
            if(num < 0) continue;

            // Total bordering squares
            int surround = 0;
            if((i==0&&j==0) || (i==boardHeight-1 && j==boardWidth-1))
                surround = 3;
            else if(i==0 || j==0 || i==boardHeight-1 || j==boardWidth-1)
                surround = 5;
            else surround = 8;

            int numFlags = CountFlagsAround(knownMine, i,j);
            int numFree = CountFlagsAround(knownEmpty, i,j);

            // Scenario 1: too many mines
            if(numFlags > num) return;

            // Scenario 2: too many empty
            if(surround - numFree < num) return;
        }

    // We have too many flags
    if(flagCount > TOT_MINES)
        return;


    // Solution found!
    if(k == borderTiles.size()){

        // We don't have the exact mine count, so no
        if(!borderOptimization && flagCount < TOT_MINES)
            return;

        VectorBool solution = VectorBool(borderTiles.size());
        for(int i=0; i<borderTiles.size(); i++){
            Pair s = borderTiles.at(i);
            int si = s.first;
            int sj = s.second;
            solution[i] = knownMine[si][sj];
        }
        tank_solutions.push_back(solution);
        return;
    }

    Pair q = borderTiles.at(k);
    int qi = q.first;
    int qj = q.second;

    // Recurse two positions: mine and no mine
    knownMine[qi][qj] = true;
    TankRecurse(borderTiles, k+1);
    knownMine[qi][qj] = false;

    knownEmpty[qi][qj] = true;
    TankRecurse(borderTiles, k+1);
    knownEmpty[qi][qj] = false;

}

int Minesweeper::CountFlagsAround(BoardFlags array, int i, int j){
    int mines = 0;

    // See if we're on the edge of the board
    bool oU = false, oD = false, oL = false, oR = false;
    if(i == 0) oU = true;
    if(j == 0) oL = true;
    if(i == boardHeight-1) oD = true;
    if(j == boardWidth-1) oR = true;

    if(!oU && array[i-1][j]) mines++;
    if(!oL && array[i][j-1]) mines++;
    if(!oD && array[i+1][j]) mines++;
    if(!oR && array[i][j+1]) mines++;
    if(!oU && !oL && array[i-1][j-1]) mines++;
    if(!oU && !oR && array[i-1][j+1]) mines++;
    if(!oD && !oL && array[i+1][j-1]) mines++;
    if(!oD && !oR && array[i+1][j+1]) mines++;

    return mines;
}

void Minesweeper::on_tableWidget_cellClicked(int i, int j)
{
    if(finalizado) return;
    if(first == 0) {
        do {
            generar();
        } while(numeros[i][j] != 0);
        first = 1;
    }

    if(mark[i][j]) return;

    check(i,j);

    int cant = 0;

    for(int i=0; i<boardHeight; i++)
        for(int j=0; j<boardWidth; j++){
            if(!mark[i][j])
            cant ++;
        }

    if(cant == TOT_MINES) win();
}

void Minesweeper::on_nuevo_clicked()
{
    boardHeight = ui->boardHeight->value();
    boardWidth = ui->boardWidth->value();
    TOT_MINES = ui->totMines->value();

    onScreen = BoardInt(boardHeight, VectorInt(boardWidth));
    flags = BoardFlags(boardHeight, VectorBool(boardWidth));

    minas = numeros = mark = posible = BoardInt(boardHeight, VectorInt(boardWidth));

    for(int i=0; i<boardHeight; i++)
        for(int j=0; j<boardWidth; j++) {
            onScreen[i][j] = 0;
            flags[i][j] = false;
        }

    marked = 0;
    clock = 0;
    ui->mark->clear();
    ui->time->clear();
    finalizado = 0;
    BF_LIMIT = 8;

    ui->tableWidget->setRowCount(boardHeight);
    ui->tableWidget->setColumnCount(boardWidth);

    ui->tableWidget2->setRowCount(boardHeight);
    ui->tableWidget2->setColumnCount(boardWidth);

    first = 0;
    for(int i=0; i<boardHeight; i++)
        for(int j=0; j<boardWidth; j++) {
            QTableWidgetItem *it = new QTableWidgetItem("");
            it->setBackground(Cell[Block]);
            ui->tableWidget->setItem(i,j,it);
            minas[i][j] = numeros[i][j] = mark[i][j] = posible[i][j];
        }
    resize();
}

void Minesweeper::resizeEvent(QResizeEvent *)
{
    resize();
}

void Minesweeper::onpress(int t)
{
    mouse = t;
}

void Minesweeper::onrelease(int)
{
    mouse = 0;
}

void Minesweeper::resize()
{
    if(boardWidth == 1)
        return;

    int size1 = qMin((ui->tableWidget->width() - 2) / boardWidth,(ui->tableWidget->height() - 2) / boardHeight);
    int size2 = qMin((ui->tableWidget2->width() - 2) / boardWidth,(ui->tableWidget2->height() - 2) / boardHeight);

    for(int i=0; i<boardWidth; i++) {
        ui->tableWidget->setColumnWidth(i, size1);
        ui->tableWidget2->setColumnWidth(i, size2);
    }
    for(int i=0; i<boardHeight; i++) {
        ui->tableWidget->setRowHeight(i, size1);
        ui->tableWidget2->setRowHeight(i, size2);
    }

    for(int i=0; i<boardHeight; i++)
        for(int j=0; j<boardWidth; j++) {
            QFont f = ui->tableWidget->item(i,j)->font();
            f.setPointSize(size1*3/5);
            ui->tableWidget->item(i,j)->setFont(f);
        }
    font.setPointSize(size1*3/5);
    symb.setPointSize(size1*3/5);
}

void Minesweeper::on_tableWidget_cellPressed(int i, int j)
{
    if(mouse == 1) return;
    if(finalizado) return;
    if(mark[i][j]) return;

    if(posible[i][j] == 0) {
        posible[i][j] = 1;
        marked ++;
        ui->tableWidget->item(i,j)->setText("O");
        ui->tableWidget->item(i,j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(i,j)->setBackground(Cell[Block]);
        ui->tableWidget->item(i,j)->setForeground(Cell[Mark1]);
        ui->tableWidget->item(i,j)->setFont(symb);
        ui->mark->setText(QString::number(marked));
    }
    else if(posible[i][j] == 1) {
        posible[i][j] = 2;
        ui->tableWidget->item(i,j)->setText("?");
        ui->tableWidget->item(i,j)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(i,j)->setBackground(Cell[Block]);
        ui->tableWidget->item(i,j)->setForeground(Cell[Mark2]);
        ui->tableWidget->item(i,j)->setFont(font);
    }
    else {
        posible[i][j] = 0;
        marked --;
        ui->tableWidget->item(i,j)->setText("");
        ui->tableWidget->item(i,j)->setBackground(Cell[Block]);
        ui->mark->setText(QString::number(marked));
    }
}

void Minesweeper::on_niv1_clicked()
{
    ui->boardHeight->setValue(10);
    ui->boardWidth->setValue(10);
    ui->totMines->setValue(10);
    on_nuevo_clicked();
}

void Minesweeper::on_niv2_clicked()
{
    ui->boardHeight->setValue(16);
    ui->boardWidth->setValue(16);
    ui->totMines->setValue(40);
    on_nuevo_clicked();
}

void Minesweeper::on_niv3_clicked()
{
    ui->boardHeight->setValue(16);
    ui->boardWidth->setValue(30);
    ui->totMines->setValue(99);
    on_nuevo_clicked();
}

void Minesweeper::on_play_clicked()
{
    if(first == 0) {
        FirstSquare();
        return;
    }

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    for(int i=0; i<boardHeight; i++) {
        for(int j=0; j<boardWidth; j++) {
            if(!mark[i][j]) continue;
            int n = numeros[i][j];
            int cont = 0;
            for(int k=0; k<8; k++) {
                if(i+pi[k] < 0 || j+pj[k] < 0 || i+pi[k] >= boardHeight || j+pj[k] >= boardWidth) continue;
                if(!mark[i+pi[k]][j+pj[k]] || posible[i+pi[k]][j+pj[k]] == 1)
                    cont ++;
            }
            if(cont == n) {
                for(int k=0; k<8; k++) {
                    if(i+pi[k] < 0 || j+pj[k] < 0 || i+pi[k] >= boardHeight || j+pj[k] >= boardWidth) continue;
                    if(!mark[i+pi[k]][j+pj[k]] && posible[i+pi[k]][j+pj[k]] == 2)
                        FlagOn(i+pi[k],j+pj[k]);
                    if(!mark[i+pi[k]][j+pj[k]] && posible[i+pi[k]][j+pj[k]] == 0)
                        FlagOn(i+pi[k],j+pj[k]);
                }
            }
        }
    }

    /// seleccionar las celdas con segururidad que no es mina
    for(int i=0; i<boardHeight; i++) {
        for(int j=0; j<boardWidth; j++) {
            if(!mark[i][j]) continue;
            int n = numeros[i][j];
            int cont = 0;
            for(int k=0; k<8; k++) {
                if(i+pi[k] < 0 || j+pj[k] < 0 || i+pi[k] >= boardHeight || j+pj[k] >= boardWidth) continue;
                if(posible[i+pi[k]][j+pj[k]] == 1)
                    cont ++;
            }
            if(cont == n) {
                for(int k=0; k<8; k++) {
                    if(i+pi[k] < 0 || j+pj[k] < 0 || i+pi[k] >= boardHeight || j+pj[k] >= boardWidth) continue;
                    if(!mark[i+pi[k]][j+pj[k]] && posible[i+pi[k]][j+pj[k]] != 1)
                        ClickOn(i+pi[k],j+pj[k]);
                }
            }
        }
    }

    for(int i=0; i<boardHeight; i++)
        for(int j=0; j<boardWidth; j++)
            if(mark[i][j] && numeros[i][j] == 0)
                ChordOn(i, j);

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
}

void Minesweeper::on_tank_clicked()
{
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    TankSolver();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif
}

void Minesweeper::on_rand_clicked()
{
    int c = 0;

    for(int i=0; i<boardHeight; i++)
        for(int j=0; j<boardWidth; j++)
            if(!mark[i][j] && posible[i][j] == 0) {
                c ++;
            }

    int r = qrand() % c;

    int t = 0;
    for(int i=0; i<boardHeight; i++)
        for(int j=0; j<boardWidth; j++) {
            if(!mark[i][j] && posible[i][j] == 0) {
                if(t == r) {
                    ClickOn(i, j);
                    return;
                }
                else t ++;
            }
        }
}

void Minesweeper::on_board_clicked()
{
    QTableWidgetItem *it;
    for(int i=0; i<boardHeight; i++)
        for(int j=0; j<boardWidth; j++) {
            it = new QTableWidgetItem();
            it->setText(QString::number(onScreen[i][j]));
            it->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget2->setItem(i,j,it);
        }
}

void Minesweeper::on_flag_clicked()
{
    QTableWidgetItem *it;
    for(int i=0; i<boardHeight; i++)
        for(int j=0; j<boardWidth; j++) {
            it = new QTableWidgetItem();
            it->setText(QString::number((int)flags[i][j]));
            it->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget2->setItem(i,j,it);
        }
}

void Minesweeper::on_mine_clicked()
{
    QTableWidgetItem *it;
    for(int i=0; i<boardHeight; i++)
        for(int j=0; j<boardWidth; j++) {
            it = new QTableWidgetItem();
            it->setText(QString::number(minas[i][j]));
            it->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget2->setItem(i,j,it);
        }
}

void Minesweeper::on_boardHeight_valueChanged(int arg1)
{
    ui->totMines->setMaximum(arg1 * ui->boardWidth->value() * 4 / 5);
}

void Minesweeper::on_boardWidth_valueChanged(int arg1)
{
    ui->totMines->setMaximum(arg1 * ui->boardHeight->value() * 4 / 5);
}

void Minesweeper::on_repetir_clicked()
{
    onScreen = BoardInt(boardHeight, VectorInt(boardWidth));
    flags = BoardFlags(boardHeight, VectorBool(boardWidth));

    mark = posible = BoardInt(boardHeight, VectorInt(boardWidth));

    for(int i=0; i<boardHeight; i++)
        for(int j=0; j<boardWidth; j++) {
            onScreen[i][j] = 0;
            flags[i][j] = false;
        }

    marked = 0;
    clock = 0;
    ui->mark->clear();
    ui->time->clear();
    finalizado = 0;
    BF_LIMIT = 8;

    first = 1;
    for(int i=0; i<boardHeight; i++)
        for(int j=0; j<boardWidth; j++) {
            QTableWidgetItem *it = new QTableWidgetItem("");
            it->setBackground(Cell[Block]);
            ui->tableWidget->setItem(i,j,it);
            mark[i][j] = posible[i][j];
        }
}
