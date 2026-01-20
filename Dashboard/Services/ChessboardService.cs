using ChessDotNet;

public static class ChessboardService
{
    public static ChessGame Game { get; private set; } = new ChessGame();

    public static bool GameStarted { get; set; } = false;
    public static GameMode Mode { get; set; } = GameMode.HumanVsHuman;
    public static PlayerSide HumanSide { get; set; } = PlayerSide.White;

    private static StockfishEngineService engine =
        new StockfishEngineService("StockfishEngine/stockfish-windows-x86-64-avx2.exe");

    public static bool ApplyPhysicalMove(string from, string to, char? promotion)
    {
        if (!GameStarted)
            return false;

        if (Mode == GameMode.HumanVsAI)
        {
            var humanPlayer = HumanSide == PlayerSide.White ? Player.White : Player.Black;
            if (Game.WhoseTurn != humanPlayer)
                return false;
        }

        Move move;
        if (promotion.HasValue)
            move = new Move(from, to, Game.WhoseTurn, promotion.Value);
        else
            move = new Move(from, to, Game.WhoseTurn);

        if (!Game.IsValidMove(move))
            return false;

        Game.MakeMove(move, true);
        return true;
    }

    public static async Task<string> MakeAIMoveAsync()
    {
        if (!GameStarted)
            return null;

        if (Mode != GameMode.HumanVsAI)
            return null;

        if (GetGameResult() != null)
            return null;

        var aiPlayer = HumanSide == PlayerSide.White ? Player.Black : Player.White;

        if (Game.WhoseTurn != aiPlayer)
            return null;

        string fen = Game.GetFen();
        string best = await engine.GetMove(fen);

        if (string.IsNullOrWhiteSpace(best) || best.Length < 4)
            return null;

        string from = best.Substring(0, 2);
        string to = best.Substring(2, 2);

        Move aiMove;
        var aiMove = new Move(from, to, Game.WhoseTurn);

        if (best.Length >= 5)
        {
            char promo = best[4];
            aiMove = new Move(from, to, aiPlayer, promo);
        }
        else
        {
            aiMove = new Move(from, to, aiPlayer);
        }

        if (!Game.IsValidMove(aiMove))
            return null;

        Game.MakeMove(aiMove, true);
        return best;
    }

    public static string GetFen() => Game.GetFen();

    public static string Resign()
    {
        GameStarted = false;

        if (Mode == GameMode.HumanVsAI)
            return HumanSide == PlayerSide.White ? "Black" : "White";

        return Game.WhoseTurn == Player.White ? "Black" : "White";
    }

    public static void Reset()
    {
        Game = new ChessGame();
        GameStarted = false;
        Mode = GameMode.HumanVsHuman;
        HumanSide = PlayerSide.White;
    }

    public static string GetGameResult()
    {
        if (Game.IsCheckmated(Player.White)) return "Black";
        if (Game.IsCheckmated(Player.Black)) return "White";

        if (Game.IsStalemated(Player.White)) return "Draw";
        if (Game.IsStalemated(Player.Black)) return "Draw";

        return null;
    }

    public static string GetCheckSquare()
    {
        string fen = Game.GetFen();
        FindKingSquaresFromFen(fen, out string whiteKing, out string blackKing);

        if (Game.IsInCheck(Player.White)) return whiteKing;
        if (Game.IsInCheck(Player.Black)) return blackKing;

        return null;
    }

    public static System.Collections.Generic.List<string> GetCheckAttackers()
    {
        string fen = Game.GetFen();
        if (string.IsNullOrWhiteSpace(fen)) return new System.Collections.Generic.List<string>();

        char[,] board = ParseFenBoard(fen);
        bool whiteInCheck = Game.IsInCheck(Player.White);
        bool blackInCheck = Game.IsInCheck(Player.Black);

        if (!whiteInCheck && !blackInCheck)
            return new System.Collections.Generic.List<string>();

        char kingChar = whiteInCheck ? 'K' : 'k';
        bool attackersAreWhite = blackInCheck;

        if (!TryFindPiece(board, kingChar, out int kR, out int kC))
            return new System.Collections.Generic.List<string>();

        var attackers = new System.Collections.Generic.List<string>();

        if (attackersAreWhite)
        {
            AddIfPieceAt(board, kR + 1, kC - 1, 'P', attackers);
            AddIfPieceAt(board, kR + 1, kC + 1, 'P', attackers);
        }
        else
        {
            AddIfPieceAt(board, kR - 1, kC - 1, 'p', attackers);
            AddIfPieceAt(board, kR - 1, kC + 1, 'p', attackers);
        }

        int[] drN = { -2, -2, -1, -1, 1, 1, 2, 2 };
        int[] dcN = { -1, 1, -2, 2, -2, 2, -1, 1 };
        char nChar = attackersAreWhite ? 'N' : 'n';
        for (int i = 0; i < 8; i++)
            AddIfPieceAt(board, kR + drN[i], kC + dcN[i], nChar, attackers);

        char kingAtt = attackersAreWhite ? 'K' : 'k';
        for (int rr = kR - 1; rr <= kR + 1; rr++)
            for (int cc = kC - 1; cc <= kC + 1; cc++)
                if (!(rr == kR && cc == kC))
                    AddIfPieceAt(board, rr, cc, kingAtt, attackers);

        ScanRay(board, kR, kC, -1, 0, attackersAreWhite, attackers, rookLike: true, bishopLike: false);
        ScanRay(board, kR, kC, 1, 0, attackersAreWhite, attackers, rookLike: true, bishopLike: false);
        ScanRay(board, kR, kC, 0, -1, attackersAreWhite, attackers, rookLike: true, bishopLike: false);
        ScanRay(board, kR, kC, 0, 1, attackersAreWhite, attackers, rookLike: true, bishopLike: false);

        ScanRay(board, kR, kC, -1, -1, attackersAreWhite, attackers, rookLike: false, bishopLike: true);
        ScanRay(board, kR, kC, -1, 1, attackersAreWhite, attackers, rookLike: false, bishopLike: true);
        ScanRay(board, kR, kC, 1, -1, attackersAreWhite, attackers, rookLike: false, bishopLike: true);
        ScanRay(board, kR, kC, 1, 1, attackersAreWhite, attackers, rookLike: false, bishopLike: true);

        return attackers;
    }

    private static void ScanRay(char[,] board, int r, int c, int dr, int dc, bool attackersAreWhite, System.Collections.Generic.List<string> attackers, bool rookLike, bool bishopLike)
    {
        int rr = r + dr;
        int cc = c + dc;

        while (rr >= 0 && rr < 8 && cc >= 0 && cc < 8)
        {
            char p = board[rr, cc];
            if (p != '\0')
            {
                if (attackersAreWhite)
                {
                    if (rookLike && (p == 'R' || p == 'Q')) attackers.Add(ToSquare(rr, cc));
                    if (bishopLike && (p == 'B' || p == 'Q')) attackers.Add(ToSquare(rr, cc));
                }
                else
                {
                    if (rookLike && (p == 'r' || p == 'q')) attackers.Add(ToSquare(rr, cc));
                    if (bishopLike && (p == 'b' || p == 'q')) attackers.Add(ToSquare(rr, cc));
                }
                return;
            }

            rr += dr;
            cc += dc;
        }
    }

    private static char[,] ParseFenBoard(string fen)
    {
        var board = new char[8, 8];
        string boardPart = fen.Split(' ')[0];

        int r = 0;
        int c = 0;

        foreach (char ch in boardPart)
        {
            if (ch == '/')
            {
                r++;
                c = 0;
                continue;
            }

            if (char.IsDigit(ch))
            {
                c += (ch - '0');
                continue;
            }

            board[r, c] = ch;
            c++;
        }

        return board;
    }

    private static bool TryFindPiece(char[,] board, char piece, out int r, out int c)
    {
        for (r = 0; r < 8; r++)
            for (c = 0; c < 8; c++)
                if (board[r, c] == piece)
                    return true;

        r = -1;
        c = -1;
        return false;
    }

    private static void AddIfPieceAt(char[,] board, int r, int c, char piece, System.Collections.Generic.List<string> attackers)
    {
        if (r < 0 || r > 7 || c < 0 || c > 7) return;
        if (board[r, c] == piece) attackers.Add(ToSquare(r, c));
    }

    private static string ToSquare(int r, int c)
    {
        char file = (char)('a' + c);
        int rank = 8 - r;
        return $"{file}{rank}";
    }

    private static void FindKingSquaresFromFen(string fen, out string whiteKing, out string blackKing)
    {
        whiteKing = null;
        blackKing = null;

        if (string.IsNullOrWhiteSpace(fen))
            return;

        string boardPart = fen.Split(' ')[0];

        int rank = 8;
        int file = 0;

        foreach (char c in boardPart)
        {
            if (c == '/')
            {
                rank--;
                file = 0;
                continue;
            }

            if (char.IsDigit(c))
            {
                file += (c - '0');
                continue;
            }

            string square = $"{(char)('a' + file)}{rank}";

            if (c == 'K') whiteKing = square;
            else if (c == 'k') blackKing = square;

            file++;
        }
    }
}
