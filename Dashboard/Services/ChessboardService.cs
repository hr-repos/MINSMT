using ChessDotNet;

public static class ChessboardService
{
    public static ChessGame Game { get; private set; } = new ChessGame();

    public static bool GameStarted { get; set; } = false;
    public static GameMode Mode { get; set; } = GameMode.HumanVsHuman;
    public static PlayerSide HumanSide { get; set; } = PlayerSide.White;

    private static StockfishEngineService engine = new StockfishEngineService("StockfishEngine/stockfish-windows-x86-64-avx2.exe");

    public static bool ApplyPhysicalMove(string from, string to, char? promotion)
    {
        if (!GameStarted)
            return false;

        Move move;

        if (promotion.HasValue)
            move = new Move(from, to, Game.WhoseTurn, promotion.Value);
        else
            move = new Move(from, to, Game.WhoseTurn);

        if (!Game.IsValidMove(move))
            return false;

        if (Mode == GameMode.HumanVsAI)
        {
            if ((HumanSide == PlayerSide.White && Game.WhoseTurn == Player.Black) || (HumanSide == PlayerSide.Black && Game.WhoseTurn == Player.White))
            {
                return false;
            }
        }

        Game.MakeMove(move, true);
        return true;
    }

    public static async Task<string> MakeAIMoveAsync()
    {
        if (!GameStarted)
        return null;

        if (Mode != GameMode.HumanVsAI)
            return null;

        if (Game.WhoseTurn != Player.Black)
            return null;

        string fen = Game.GetFen();
        string best = await engine.GetMove(fen); // e.g., "e7e5"

        if (string.IsNullOrWhiteSpace(best) || best.Length < 4)
            return null;

        string from = best.Substring(0, 2);
        string to   = best.Substring(2, 2);

        var aiMove = new Move(from, to, Player.Black);

        if (Game.IsValidMove(aiMove))
        {
            Game.MakeMove(aiMove, true);
            return best;
        }

        return null;
    }

    public static string GetFen() => Game.GetFen();

    public static string Resign()
    {
        if (Game.WhoseTurn == Player.White)
            return "Black";
        else
            return "White";
    }

    public static void Reset()
    {
        Game = new ChessGame();
    }

    public static string GetGameResult()
    {
        if (Game.IsCheckmated(Player.White)) return "Black";
        if (Game.IsCheckmated(Player.Black)) return "White";

        if (Game.IsStalemated(Player.White)) return "Draw";
        if (Game.IsStalemated(Player.Black)) return "Draw";

        return null;
    }
}
