package chess;

import com.sun.org.apache.xpath.internal.operations.Bool;

import java.awt.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;
import javax.swing.*;
import javax.swing.border.*;
//======================================================Don't modify below===============================================================//
enum PieceType {king, queen, bishop, knight, rook, pawn, none}
enum PlayerColor {black, white, none}

public class ChessBoard_2013_10958 {
    private final JPanel gui = new JPanel(new BorderLayout(3, 3));
    private JPanel chessBoard;
    private JButton[][] chessBoardSquares = new JButton[8][8];
    private Piece[][] chessBoardStatus = new Piece[8][8];
    private ImageIcon[] pieceImage_b = new ImageIcon[7];
    private ImageIcon[] pieceImage_w = new ImageIcon[7];
    private JLabel message = new JLabel("Enter Reset to Start");

    ChessBoard_2013_10958(){
        initPieceImages();
        initBoardStatus();
        initializeGui();
    }

    public final void initBoardStatus(){
        for(int i=0;i<8;i++){
            for(int j=0;j<8;j++) chessBoardStatus[j][i] = new Piece();
        }
    }

    public final void initPieceImages(){
        pieceImage_b[0] = new ImageIcon(new ImageIcon("./img/king_b.png").getImage().getScaledInstance(64, 64, java.awt.Image.SCALE_SMOOTH));
        pieceImage_b[1] = new ImageIcon(new ImageIcon("./img/queen_b.png").getImage().getScaledInstance(64, 64, java.awt.Image.SCALE_SMOOTH));
        pieceImage_b[2] = new ImageIcon(new ImageIcon("./img/bishop_b.png").getImage().getScaledInstance(64, 64, java.awt.Image.SCALE_SMOOTH));
        pieceImage_b[3] = new ImageIcon(new ImageIcon("./img/knight_b.png").getImage().getScaledInstance(64, 64, java.awt.Image.SCALE_SMOOTH));
        pieceImage_b[4] = new ImageIcon(new ImageIcon("./img/rook_b.png").getImage().getScaledInstance(64, 64, java.awt.Image.SCALE_SMOOTH));
        pieceImage_b[5] = new ImageIcon(new ImageIcon("./img/pawn_b.png").getImage().getScaledInstance(64, 64, java.awt.Image.SCALE_SMOOTH));
        pieceImage_b[6] = new ImageIcon(new BufferedImage(64, 64, BufferedImage.TYPE_INT_ARGB));

        pieceImage_w[0] = new ImageIcon(new ImageIcon("./img/king_w.png").getImage().getScaledInstance(64, 64, java.awt.Image.SCALE_SMOOTH));
        pieceImage_w[1] = new ImageIcon(new ImageIcon("./img/queen_w.png").getImage().getScaledInstance(64, 64, java.awt.Image.SCALE_SMOOTH));
        pieceImage_w[2] = new ImageIcon(new ImageIcon("./img/bishop_w.png").getImage().getScaledInstance(64, 64, java.awt.Image.SCALE_SMOOTH));
        pieceImage_w[3] = new ImageIcon(new ImageIcon("./img/knight_w.png").getImage().getScaledInstance(64, 64, java.awt.Image.SCALE_SMOOTH));
        pieceImage_w[4] = new ImageIcon(new ImageIcon("./img/rook_w.png").getImage().getScaledInstance(64, 64, java.awt.Image.SCALE_SMOOTH));
        pieceImage_w[5] = new ImageIcon(new ImageIcon("./img/pawn_w.png").getImage().getScaledInstance(64, 64, java.awt.Image.SCALE_SMOOTH));
        pieceImage_w[6] = new ImageIcon(new BufferedImage(64, 64, BufferedImage.TYPE_INT_ARGB));
    }

    public ImageIcon getImageIcon(Piece piece){
        if(piece.color.equals(PlayerColor.black)){
            if(piece.type.equals(PieceType.king)) return pieceImage_b[0];
            else if(piece.type.equals(PieceType.queen)) return pieceImage_b[1];
            else if(piece.type.equals(PieceType.bishop)) return pieceImage_b[2];
            else if(piece.type.equals(PieceType.knight)) return pieceImage_b[3];
            else if(piece.type.equals(PieceType.rook)) return pieceImage_b[4];
            else if(piece.type.equals(PieceType.pawn)) return pieceImage_b[5];
            else return pieceImage_b[6];
        }
        else if(piece.color.equals(PlayerColor.white)){
            if(piece.type.equals(PieceType.king)) return pieceImage_w[0];
            else if(piece.type.equals(PieceType.queen)) return pieceImage_w[1];
            else if(piece.type.equals(PieceType.bishop)) return pieceImage_w[2];
            else if(piece.type.equals(PieceType.knight)) return pieceImage_w[3];
            else if(piece.type.equals(PieceType.rook)) return pieceImage_w[4];
            else if(piece.type.equals(PieceType.pawn)) return pieceImage_w[5];
            else return pieceImage_w[6];
        }
        else return pieceImage_w[6];
    }

    public final void initializeGui(){
        gui.setBorder(new EmptyBorder(5, 5, 5, 5));
        JToolBar tools = new JToolBar();
        tools.setFloatable(false);
        gui.add(tools, BorderLayout.PAGE_START);
        JButton startButton = new JButton("Reset");
        startButton.addActionListener(new ActionListener(){
            public void actionPerformed(ActionEvent e){
                initiateBoard();
            }
        });

        tools.add(startButton);
        tools.addSeparator();
        tools.add(message);

        chessBoard = new JPanel(new GridLayout(0, 8));
        chessBoard.setBorder(new LineBorder(Color.BLACK));
        gui.add(chessBoard);
        ImageIcon defaultIcon = new ImageIcon(new BufferedImage(64, 64, BufferedImage.TYPE_INT_ARGB));
        Insets buttonMargin = new Insets(0,0,0,0);
        for(int i=0; i<chessBoardSquares.length; i++) {
            for (int j = 0; j < chessBoardSquares[i].length; j++) {
                JButton b = new JButton();
                b.addActionListener(new ButtonListener(i, j));
                b.setMargin(buttonMargin);
                b.setIcon(defaultIcon);
                if((j % 2 == 1 && i % 2 == 1)|| (j % 2 == 0 && i % 2 == 0)) b.setBackground(Color.WHITE);
                else b.setBackground(Color.gray);
                b.setOpaque(true);
                b.setBorderPainted(false);
                chessBoardSquares[j][i] = b;
            }
        }

        for (int i=0; i < 8; i++) {
            for (int j=0; j < 8; j++) chessBoard.add(chessBoardSquares[j][i]);

        }
    }

    public final JComponent getGui() {
        return gui;
    }

    public static void main(String[] args) {
        Runnable r = new Runnable() {
            @Override
            public void run() {
                ChessBoard_2013_10958 cb = new ChessBoard_2013_10958();
                JFrame f = new JFrame("Chess");
                f.add(cb.getGui());
                f.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
                f.setLocationByPlatform(true);
                f.setResizable(false);
                f.pack();
                f.setMinimumSize(f.getSize());
                f.setVisible(true);
            }
        };
        SwingUtilities.invokeLater(r);
    }

    //================================Utilize these functions========================================//

    class Piece{
        PlayerColor color;
        PieceType type;

        Piece(){
            color = PlayerColor.none;
            type = PieceType.none;
        }
        Piece(PlayerColor color, PieceType type){
            this.color = color;
            this.type = type;
        }
    }

    public void setIcon(int x, int y, Piece piece){
        chessBoardSquares[y][x].setIcon(getImageIcon(piece));
        chessBoardStatus[y][x] = piece;
    }

    public Piece getIcon(int x, int y){
        return chessBoardStatus[y][x];
    }

    public void markPosition(int x, int y){
        chessBoardSquares[y][x].setBackground(Color.pink);
    }

    public void unmarkPosition(int x, int y){
        if((y % 2 == 1 && x % 2 == 1)|| (y % 2 == 0 && x % 2 == 0)) chessBoardSquares[y][x].setBackground(Color.WHITE);
        else chessBoardSquares[y][x].setBackground(Color.gray);
    }

    public void setStatus(String inpt){
        message.setText(inpt);
    }

    public void initiateBoard(){
        for(int i=0;i<8;i++){
            for(int j=0;j<8;j++) setIcon(i, j, new Piece());
        }
        setIcon(0, 0, new Piece(PlayerColor.black, PieceType.rook));
        setIcon(0, 1, new Piece(PlayerColor.black, PieceType.knight));
        setIcon(0, 2, new Piece(PlayerColor.black, PieceType.bishop));
        setIcon(0, 3, new Piece(PlayerColor.black, PieceType.queen));
        setIcon(0, 4, new Piece(PlayerColor.black, PieceType.king));
        setIcon(0, 5, new Piece(PlayerColor.black, PieceType.bishop));
        setIcon(0, 6, new Piece(PlayerColor.black, PieceType.knight));
        setIcon(0, 7, new Piece(PlayerColor.black, PieceType.rook));
        for(int i=0;i<8;i++){
            setIcon(1, i, new Piece(PlayerColor.black, PieceType.pawn));
            setIcon(6, i, new Piece(PlayerColor.white, PieceType.pawn));
        }
        setIcon(7, 0, new Piece(PlayerColor.white, PieceType.rook));
        setIcon(7, 1, new Piece(PlayerColor.white, PieceType.knight));
        setIcon(7, 2, new Piece(PlayerColor.white, PieceType.bishop));
        setIcon(7, 3, new Piece(PlayerColor.white, PieceType.queen));
        setIcon(7, 4, new Piece(PlayerColor.white, PieceType.king));
        setIcon(7, 5, new Piece(PlayerColor.white, PieceType.bishop));
        setIcon(7, 6, new Piece(PlayerColor.white, PieceType.knight));
        setIcon(7, 7, new Piece(PlayerColor.white, PieceType.rook));
        for(int i=0;i<8;i++){
            for(int j=0;j<8;j++) unmarkPosition(i, j);
        }
        onInitiateBoard();
    }
//======================================================Don't modify above==============================================================//




    //======================================================Implement below=================================================================//
    enum MagicType {MARK, CHECK, CHECKMATE};
    private int selX, selY;
    private boolean check, checkmate, end;

    int state = 0;
    private PlayerColor turn_color = PlayerColor.black; // black first turn

    class ButtonListener implements ActionListener{
        int x;
        int y;
        ButtonListener(int x, int y){
            this.x = x;
            this.y = y;
        }

        public void actionPerformed(ActionEvent e) {	// Only modify here
            // (x, y) is where the click event occured

            if(state == 0){ // pawn select state
                // Choose current turn piece
                selX = x;
                selY = y;
                Piece select = getIcon(selX, selY);
                Boolean move_available = false; // selected piece can move or not? (if not, stay in state 0)

                if(select.type == PieceType.none || select.color == PlayerColor.none){
                    state = 0;
                }

                else{
                    // case of piece type
                    switch (select.type){
                        case pawn:
                            move_available = PawnMark(x, y, turn_color);
                            break;
                        case rook:
                            move_available = RookMark(x, y, turn_color);
                            break;
                        case knight:
                            move_available = KnightMark(x, y, turn_color);
                            break;
                        case bishop:
                            move_available = BishopMark(x, y, turn_color);
                            break;
                        case queen:
                            move_available = QueenMark(x, y, turn_color);
                            break;
                        case king:
                            move_available = KingMark(x, y, turn_color);
                            break;
                    }

                    if(!move_available) state = 0;
                    else state = 1;
                }
            }

            else if(state == 1){ // mark state
                Color background_color = chessBoardSquares[y][x].getBackground();
                if(background_color != Color.pink){ // not movable space
                    background_reset();
                    state = 0;
                }

                else{ // movable space
                    Piece select = getIcon(selX, selY); // this piece will move to new space
                    Piece moved = new Piece(select.color, select.type); // moved piece
                    Piece before = getIcon(x, y); // original piece in (x, y)
                    setIcon(x, y, moved);
                    setIcon(selX, selY, new Piece());
                    background_reset();

                    turn_color_change();
                    if(turn_color == PlayerColor.black) setStatus("BLACK's TURN");
                    else if(turn_color == PlayerColor.white) setStatus("WHITE's TURN");

                    // king is dead case - game end
                    if(before.type == PieceType.king) end = true;

                    if(end) state = 2;
                    else { // king is alive
                        // check or checkmate should be considered
                        check = check(turn_color);

                        PlayerColor rest_color = PlayerColor.white;
                        if(turn_color == PlayerColor.black) rest_color = PlayerColor.white;
                        else if(turn_color == PlayerColor.white) rest_color = PlayerColor.black;

                        Boolean checkmate_ally = !check_escape(turn_color);
                        Boolean chechmate_enemy = !check_escape(rest_color);

                        // if ally or enemy king is in checkmate, checkmate is true
                        checkmate = checkmate_ally || chechmate_enemy;

                        if(checkmate){ // check mate! - game end (check mate prevails check condition)
                            if(turn_color == PlayerColor.black) setStatus("BLACK's TURN / CHECK MATE");
                            else if(turn_color == PlayerColor.white) setStatus("WHITE's TURN / CHECK MATE");
                            state = 2; // should be state = 2
                        }

                        else if(check){ // if in check state (but not in checkmate)
                            if(turn_color == PlayerColor.black) setStatus("BLACK's TURN / CHECK");
                            else if(turn_color == PlayerColor.white) setStatus("WHITE's TURN / CHECK");
                            state = 0;
                        }

                        // not in check state or checkmate state
                        else{
                            state = 0;
                        }
                    }
                }
            }

            else if(state == 2){ // final state : check mate or end, stuck in this state
                state = 2;
            }

        }
    }

    void onInitiateBoard(){ // reset chess game
        state = 0;
        turn_color = PlayerColor.black;
        setStatus("BLACK's TURN"); // default first turn is black

        end = false;
        check = false;
        checkmate = false;
    }

    Boolean PawnMark(int x, int y, PlayerColor turn_color){
        int move_count = 0; // count of movable space

        if(turn_color == PlayerColor.black){ // black turn
            Piece current = getIcon(x, y);
            if( current.color == turn_color && x <= 6){
                markPosition(x + 1, y);
                move_count++;
                if(x == 1) { // pawn start position
                    markPosition(x + 2, y);
                    move_count++;
                }

                // check for ally or enemy piece
                Piece piece1 = getIcon(x + 1, y);
                Piece piece2 = new Piece();
                if(x == 1) piece2 = getIcon(x + 2, y); // only consider piece2 in start

                if(piece1.color != PlayerColor.none) {
                    unmarkPosition(x + 1, y);
                    move_count--;
                    if(x == 1) {
                        unmarkPosition(x + 2, y);
                        move_count--;
                    }
                }

                if(x == 1) { // only consider piece2 in start
                    if(piece2.color != PlayerColor.none){
                        unmarkPosition(x + 2, y);
                        move_count--;
                    }
                }

                // check for enemy piece
                if(y == 0){ // left boundary case
                    Piece enemy = getIcon(x + 1, y + 1);
                    if(enemy.color == PlayerColor.white) {
                        markPosition(x + 1, y + 1);
                        move_count++;
                    }
                }

                else if(y == 7){ // right boundary case
                    Piece enemy = getIcon(x + 1, y - 1);
                    if(enemy.color == PlayerColor.white) {
                        markPosition(x + 1, y - 1);
                        move_count++;
                    }
                }

                else{ // intermediate case
                    Piece enemy1 = getIcon(x + 1, y + 1);
                    Piece enemy2 = getIcon(x + 1, y - 1);
                    if(enemy1.color == PlayerColor.white) {
                        markPosition(x + 1, y + 1);
                        move_count++;
                    }
                    if(enemy2.color == PlayerColor.white) {
                        markPosition(x + 1, y - 1);
                        move_count++;
                    }
                }
            }
        }

        else if(turn_color == PlayerColor.white){ // white turn
            Piece current = getIcon(x, y);
            if(current.color == turn_color && x >= 1){
                markPosition(x - 1, y);
                move_count++;
                if(x == 6) { // pawn start position
                    markPosition(x - 2, y);
                    move_count++;
                }

                // check for ally or enemy piece
                Piece piece1 = getIcon(x - 1, y);
                Piece piece2 = new Piece();
                if(x == 6) piece2 = getIcon(x - 2, y); // only consider piece2 in start

                if(piece1.color != PlayerColor.none) {
                    unmarkPosition(x - 1, y);
                    move_count--;
                    if(x == 6) {
                        unmarkPosition(x - 2, y);
                        move_count--;
                    }
                }

                if(x == 6) { // only consider piece2 in start
                    if(piece2.color != PlayerColor.none) {
                        unmarkPosition(x - 2, y);
                        move_count--;
                    }
                }

                // check for enemy piece
                if(y == 0){ // left boundary case
                    Piece enemy = getIcon(x - 1, y + 1);
                    if(enemy.color == PlayerColor.black) {
                        markPosition(x - 1, y + 1);
                        move_count++;
                    }
                }

                else if(y == 7){ // right boundary case
                    Piece enemy = getIcon(x - 1, y - 1);
                    if(enemy.color == PlayerColor.black) {
                        markPosition(x - 1, y - 1);
                        move_count++;
                    }
                }

                else{ // intermediate case
                    Piece enemy1 = getIcon(x - 1, y + 1);
                    Piece enemy2 = getIcon(x - 1, y - 1);
                    if(enemy1.color == PlayerColor.black) {
                        markPosition(x - 1, y + 1);
                        move_count++;
                    }
                    if(enemy2.color == PlayerColor.black) {
                        markPosition(x - 1, y - 1);
                        move_count++;
                    }
                }
            }
        }

        if(move_count == 0) return false;
        else return true;
    }

    Boolean RookMark(int x, int y, PlayerColor turn_color){
        int move_count = 0; // count of movable space
        Piece current = getIcon(x, y);
        if(current.color != turn_color) return false;

        for(int i = x + 1; i <= 7 ; i++){ // move down
            Piece piece = getIcon(i, y);
            // check for ally
            if(current.color == piece.color) break;
            // check for enemy
            else if(current.color != piece.color && piece.color != PlayerColor.none ){
                markPosition(i, y);
                break;
            }
            else markPosition(i, y);
            move_count++;
        }

        for(int i = x - 1; i >= 0 ; i--){ // move up
            Piece piece = getIcon(i, y);
            // check for ally
            if(current.color == piece.color) break;
            // check for enemy
            else if(current.color != piece.color && piece.color != PlayerColor.none ){
                markPosition(i, y);
                break;
            }
            else markPosition(i, y);
            move_count++;
        }

        for(int j = y + 1; j <= 7 ; j++){ // move right
            Piece piece = getIcon(x, j);
            // check for ally
            if(current.color == piece.color) break;
            // check for enemy
            else if(current.color != piece.color && piece.color != PlayerColor.none ){
                markPosition(x, j);
                break;
            }
            else markPosition(x, j);
            move_count++;
        }

        for(int j = y - 1; j >= 0 ; j--){ // move left
            Piece piece = getIcon(x, j);
            // check for ally
            if(current.color == piece.color) break;
            // check for enemy
            else if(current.color != piece.color && piece.color != PlayerColor.none ){
                markPosition(x, j);
                break;
            }
            else markPosition(x, j);
            move_count++;
        }

        if(move_count == 0) return false;
        else return true;
    }

    Boolean KnightMark(int x, int y, PlayerColor turn_color){
        int move_count = 0; // count of movable space
        Piece current = getIcon(x, y);
        if(current.color != turn_color) return false;

        // right up
        if((x - 2) >= 0 && (y + 1) <= 7){
            Piece piece = getIcon(x - 2, y + 1);
            // check for ally
            if(current.color != piece.color){
                markPosition(x - 2, y + 1);
                move_count++;
            }
        }

        if((x - 1) >= 0 && (y + 2) <= 7){
            Piece piece = getIcon(x - 1, y + 2);
            // check for ally
            if(current.color != piece.color){
                markPosition(x - 1, y + 2);
                move_count++;
            }
        }

        // left up
        if((x - 1) >= 0 && (y - 2) >= 0){
            Piece piece = getIcon(x - 1, y - 2);
            // check for ally
            if(current.color != piece.color){
                markPosition(x - 1, y - 2);
                move_count++;
            }
        }

        if((x - 2) >= 0 && (y - 1) >= 0){
            Piece piece = getIcon(x - 2, y - 1);
            // check for ally
            if(current.color != piece.color){
                markPosition(x - 2, y - 1);
                move_count++;
            }
        }

        // down right
        if((x + 2) <= 7 && (y + 1) <= 7){
            Piece piece = getIcon(x + 2, y + 1);
            // check for ally
            if(current.color != piece.color){
                markPosition(x + 2, y + 1);
                move_count++;
            }
        }

        if((x + 1) <= 7 && (y + 2) <= 7){
            Piece piece = getIcon(x + 1, y + 2);
            // check for ally
            if(current.color != piece.color){
                markPosition(x + 1, y + 2);
                move_count++;
            }
        }

        // down left
        if((x + 2) <= 7 && (y - 1) >= 0){
            Piece piece = getIcon(x + 2, y - 1);
            // check for ally
            if(current.color != piece.color){
                markPosition(x + 2, y - 1);
                move_count++;
            }
        }

        if((x + 1) <= 7 && (y - 2) >= 0){
            Piece piece = getIcon(x + 1, y - 2);
            // check for ally
            if(current.color != piece.color){
                markPosition(x + 1, y - 2);
                move_count++;
            }
        }

        if(move_count == 0) return false;
        else return true;
    }

    Boolean BishopMark(int x, int y, PlayerColor turn_color){
        int move_count = 0; // count of movable space
        Piece current = getIcon(x, y);
        if(current.color != turn_color) return false;

        int j = y;
        for(int i = x + 1; i <= 7 ; i++){ // move right down
            j++;
            if(j > 7) break;
            Piece piece = getIcon(i, j);
            // check for ally
            if(current.color == piece.color) break;
            // check for enemy
            else if(current.color != piece.color && piece.color != PlayerColor.none ){
                markPosition(i, j);
                break;
            }
            else markPosition(i, j);
            move_count++;
        }

        j = y;
        for(int i = x + 1; i <= 7 ; i++){ // move left down
            j--;
            if(j < 0) break;
            Piece piece = getIcon(i, j);
            // check for ally
            if(current.color == piece.color) break;
            // check for enemy
            else if(current.color != piece.color && piece.color != PlayerColor.none ){
                markPosition(i, j);
                break;
            }
            else markPosition(i, j);
            move_count++;
        }

        j = y;
        for(int i = x - 1; i >= 0 ; i--){ // move right up
            j++;
            if(j > 7) break;
            Piece piece = getIcon(i, j);
            // check for ally
            if(current.color == piece.color) break;
            // check for enemy
            else if(current.color != piece.color && piece.color != PlayerColor.none ){
                markPosition(i, j);
                break;
            }
            else markPosition(i, j);
            move_count++;
        }

        j = y;
        for(int i = x - 1; i >= 0 ; i--){ // move left up
            j--;
            if(j < 0) break;
            Piece piece = getIcon(i, j);
            // check for ally
            if(current.color == piece.color) break;
            // check for enemy
            else if(current.color != piece.color && piece.color != PlayerColor.none ){
                markPosition(i, j);
                break;
            }
            else markPosition(i, j);
            move_count++;
        }

        if(move_count == 0) return false;
        else return true;
    }

    Boolean QueenMark(int x, int y, PlayerColor turn_color){
        int move_count = 0; // count of movable space
        Piece current = getIcon(x, y);
        if(current.color != turn_color) return false;

        // combine rook and bishop
        for(int i = x + 1; i <= 7 ; i++){ // move down
            Piece piece = getIcon(i, y);
            // check for ally
            if(current.color == piece.color) break;
                // check for enemy
            else if(current.color != piece.color && piece.color != PlayerColor.none ){
                markPosition(i, y);
                break;
            }
            else markPosition(i, y);
            move_count++;
        }

        for(int i = x - 1; i >= 0 ; i--){ // move up
            Piece piece = getIcon(i, y);
            // check for ally
            if(current.color == piece.color) break;
                // check for enemy
            else if(current.color != piece.color && piece.color != PlayerColor.none ){
                markPosition(i, y);
                break;
            }
            else markPosition(i, y);
            move_count++;
        }

        for(int j = y + 1; j <= 7 ; j++){ // move right
            Piece piece = getIcon(x, j);
            // check for ally
            if(current.color == piece.color) break;
                // check for enemy
            else if(current.color != piece.color && piece.color != PlayerColor.none ){
                markPosition(x, j);
                break;
            }
            else markPosition(x, j);
            move_count++;
        }

        for(int j = y - 1; j >= 0 ; j--){ // move left
            Piece piece = getIcon(x, j);
            // check for ally
            if(current.color == piece.color) break;
                // check for enemy
            else if(current.color != piece.color && piece.color != PlayerColor.none ){
                markPosition(x, j);
                break;
            }
            else markPosition(x, j);
            move_count++;
        }

        int j = y;
        for(int i = x + 1; i <= 7 ; i++){ // move right down
            j++;
            if(j > 7) break;
            Piece piece = getIcon(i, j);
            // check for ally
            if(current.color == piece.color) break;
                // check for enemy
            else if(current.color != piece.color && piece.color != PlayerColor.none ){
                markPosition(i, j);
                break;
            }
            else markPosition(i, j);
            move_count++;
        }

        j = y;
        for(int i = x + 1; i <= 7 ; i++){ // move left down
            j--;
            if(j < 0) break;
            Piece piece = getIcon(i, j);
            // check for ally
            if(current.color == piece.color) break;
                // check for enemy
            else if(current.color != piece.color && piece.color != PlayerColor.none ){
                markPosition(i, j);
                break;
            }
            else markPosition(i, j);
            move_count++;
        }

        j = y;
        for(int i = x - 1; i >= 0 ; i--){ // move right up
            j++;
            if(j > 7) break;
            Piece piece = getIcon(i, j);
            // check for ally
            if(current.color == piece.color) break;
                // check for enemy
            else if(current.color != piece.color && piece.color != PlayerColor.none ){
                markPosition(i, j);
                break;
            }
            else markPosition(i, j);
            move_count++;
        }

        j = y;
        for(int i = x - 1; i >= 0 ; i--){ // move left up
            j--;
            if(j < 0) break;
            Piece piece = getIcon(i, j);
            // check for ally
            if(current.color == piece.color) break;
                // check for enemy
            else if(current.color != piece.color && piece.color != PlayerColor.none ){
                markPosition(i, j);
                break;
            }
            else markPosition(i, j);
            move_count++;
        }

        if(move_count == 0) return false;
        else return true;
    }

    Boolean KingMark(int x, int y, PlayerColor turn_color){
        int move_count = 0; // count of movable space
        Piece current = getIcon(x, y);
        if(current.color != turn_color) return false;

        // move up
        if((x - 1) >= 0){
            Piece piece = getIcon(x - 1, y);
            // check for ally
            if(current.color != piece.color){
                markPosition(x - 1, y );
                move_count++;
            }
        }

        // move down
        if((x + 1) <= 7){
            Piece piece = getIcon(x + 1, y);
            // check for ally
            if(current.color != piece.color){
                markPosition(x + 1, y );
                move_count++;
            }
        }

        // move right
        if((y + 1) <= 7){
            Piece piece = getIcon(x, y + 1);
            // check for ally
            if(current.color != piece.color){
                markPosition(x, y + 1);
                move_count++;
            }
        }

        // move left
        if((y - 1) >= 0){
            Piece piece = getIcon(x, y - 1);
            // check for ally
            if(current.color != piece.color){
                markPosition(x, y - 1);
                move_count++;
            }
        }

        // move right up
        if((x - 1) >= 0 && (y + 1) <= 7){
            Piece piece = getIcon(x - 1, y + 1);
            // check for ally
            if(current.color != piece.color){
                markPosition(x - 1, y + 1);
                move_count++;
            }
        }

        // move left up
        if((x - 1) >= 0 && (y - 1) >= 0){
            Piece piece = getIcon(x - 1, y - 1);
            // check for ally
            if(current.color != piece.color){
                markPosition(x - 1, y - 1);
                move_count++;
            }
        }

        // move right down
        if((x + 1) <= 7 && (y + 1) <= 7){
            Piece piece = getIcon(x + 1, y + 1);
            // check for ally
            if(current.color != piece.color){
                markPosition(x + 1, y + 1);
                move_count++;
            }
        }

        // move left down
        if((x + 1) <= 7 && (y - 1) >= 0){
            Piece piece = getIcon(x + 1, y - 1);
            // check for ally
            if(current.color != piece.color){
                markPosition(x + 1, y - 1);
                move_count++;
            }
        }

        if(move_count == 0) return false;
        else return true;
    }

    void background_reset(){ // background color reset
        for(int i = 0; i < chessBoardSquares.length; i++) { // color update after move
            for (int j = 0; j < chessBoardSquares[i].length; j++) {
                if((j % 2 == 1 && i % 2 == 1)|| (j % 2 == 0 && i % 2 == 0)) {
                    chessBoardSquares[j][i].setBackground(Color.WHITE);
                }
                else chessBoardSquares[j][i].setBackground(Color.GRAY);
            }
        }
    }

    void turn_color_change(){
        if(turn_color == PlayerColor.white) turn_color = PlayerColor.black;
        else if(turn_color == PlayerColor.black) turn_color = PlayerColor.white;
    }

    // information about check state
    Boolean check(PlayerColor turn_color){
        // check for all piece
        Boolean check = false;
        Boolean move_available = false;
        PlayerColor rest_color = PlayerColor.white; // opposite of turn_color, should use this for checking check state

        if(turn_color == PlayerColor.white) rest_color = PlayerColor.black;
        else if(turn_color == PlayerColor.black) rest_color = PlayerColor.white;

        for(int i = 0; i < chessBoardSquares.length; i++) {
            for (int j = 0; j < chessBoardSquares[i].length; j++) {
                Piece piece = getIcon(i, j);

                // for enemy piece, confirm check state
                if(piece.color != turn_color && piece.color != PlayerColor.none){
                    switch (piece.type){
                        case pawn:
                            move_available = PawnMark(i, j, piece.color);
                            for(int i1 = 0; i1 < chessBoardSquares.length; i1++) {
                                for (int j1 = 0; j1 < chessBoardSquares[i1].length; j1++) {
                                    Piece piece_temp = getIcon(i1, j1);
                                    // king is in check state
                                    if(piece_temp.type == PieceType.king && chessBoardSquares[j1][i1].getBackground() == Color.pink){
                                        check = true;
                                    }
                                }
                            }
                            break;

                        case rook:
                            move_available = RookMark(i, j, rest_color);
                            for(int i1 = 0; i1 < chessBoardSquares.length; i1++) {
                                for (int j1 = 0; j1 < chessBoardSquares[i1].length; j1++) {
                                    Piece piece_temp = getIcon(i1, j1);
                                    // king is in check state
                                    if(piece_temp.type == PieceType.king && chessBoardSquares[j1][i1].getBackground() == Color.pink){
                                        check = true;
                                    }
                                }
                            }
                            break;

                        case knight:
                            move_available = KnightMark(i, j, rest_color);
                            for(int i1 = 0; i1 < chessBoardSquares.length; i1++) {
                                for (int j1 = 0; j1 < chessBoardSquares[i1].length; j1++) {
                                    Piece piece_temp = getIcon(i1, j1);
                                    // king is in check state
                                    if(piece_temp.type == PieceType.king && chessBoardSquares[j1][i1].getBackground() == Color.pink){
                                        check = true;
                                    }
                                }
                            }
                            break;

                        case bishop:
                            move_available = BishopMark(i, j, rest_color);
                            for(int i1 = 0; i1 < chessBoardSquares.length; i1++) {
                                for (int j1 = 0; j1 < chessBoardSquares[i1].length; j1++) {
                                    Piece piece_temp = getIcon(i1, j1);
                                    // king is in check state
                                    if(piece_temp.type == PieceType.king && chessBoardSquares[j1][i1].getBackground() == Color.pink){
                                        check = true;
                                    }
                                }
                            }
                            break;

                        case queen:
                            move_available = QueenMark(i, j, rest_color);
                            for(int i1 = 0; i1 < chessBoardSquares.length; i1++) {
                                for (int j1 = 0; j1 < chessBoardSquares[i1].length; j1++) {
                                    Piece piece_temp = getIcon(i1, j1);
                                    // king is in check state
                                    if(piece_temp.type == PieceType.king && chessBoardSquares[j1][i1].getBackground() == Color.pink){
                                        check = true;
                                    }
                                }
                            }
                            break;

                        case king:
                            move_available = KingMark(i, j, rest_color);
                            for(int i1 = 0; i1 < chessBoardSquares.length; i1++) {
                                for (int j1 = 0; j1 < chessBoardSquares[i1].length; j1++) {
                                    Piece piece_temp = getIcon(i1, j1);
                                    // king is in check state
                                    if(piece_temp.type == PieceType.king && chessBoardSquares[j1][i1].getBackground() == Color.pink){
                                        check = true;
                                    }
                                }
                            }
                            break;
                    }
                }
            }
        }
        background_reset(); // after confirming check status, background reset
        return check;
    }

    Boolean check_escape(PlayerColor turn_color){
        Boolean check_escape = false;

        // virtually move every ally piece for every case, and confirm check case
        for(int i = 0; i < chessBoardSquares.length; i++){
            for (int j = 0; j < chessBoardSquares[i].length; j++) {
                Piece select = getIcon(i, j);

                // virtually move ally piece
                if(select.color == turn_color){
                    switch (select.type){
                        case pawn:
                            PawnMark(i, j, turn_color);
                            break;

                        case rook:
                            RookMark(i, j, turn_color);
                            break;

                        case knight:
                            KnightMark(i, j, turn_color);
                            break;

                        case bishop:
                            BishopMark(i, j, turn_color);
                            break;

                        case queen:
                            QueenMark(i, j, turn_color);
                            break;

                        case king:
                            KingMark(i, j, turn_color);
                            break;

                    }

                    // check for all possible move
                    for(int k = 0; k < chessBoardSquares.length; k++) {
                        for (int l = 0; l < chessBoardSquares[k].length; l++) {

                            if(chessBoardSquares[l][k].getBackground() == Color.pink){
                                Piece before = getIcon(k, l);
                                Piece moved = new Piece(select.color, select.type);

                                // virtually move
                                setIcon(k, l, moved);
                                setIcon(i, j, new Piece());
                                background_reset();

                                // king can escape?
                                if(!check(turn_color)) check_escape = true;

                                // move back to original configuration
                                setIcon(i, j, moved);
                                setIcon(k, l, before);

                                switch (select.type){
                                    case pawn:
                                        PawnMark(i, j, turn_color);
                                        break;

                                    case rook:
                                        RookMark(i, j, turn_color);
                                        break;

                                    case knight:
                                        KnightMark(i, j, turn_color);
                                        break;

                                    case bishop:
                                        BishopMark(i, j, turn_color);
                                        break;

                                    case queen:
                                        QueenMark(i, j, turn_color);
                                        break;

                                    case king:
                                        KingMark(i, j, turn_color);
                                        break;
                                }
                            }
                        }
                    }
                    background_reset(); // after checking one ally, background reset
                }
            }
        }

        // king can't escape by any move of ally piece
        background_reset();
        return check_escape;
    }

}