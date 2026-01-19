using ChessDotNet;

public static class ChessboardService
{
    public static ChessGame Game { get; private set; } = new ChessGame();
    private static StockfishEngineService engine = new StockfishEngineService("StockfishEngine/stockfish-windows-x86-64-avx2.exe");

    public static bool ApplyPhysicalMove(string from, string to)
    {
        var move = new Move(from, to, Game.WhoseTurn);

        if (!Game.IsValidMove(move))
            return false;

        Game.MakeMove(move, true);
        return true;
    }

    public static async Task<string> MakeAIMoveAsync()
    {
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

    public static void Reset()
    {
        Game = new ChessGame();
    }

    public static string GetWinner()
    {
        if (Game.IsCheckmated(Player.White)) return "Black";
        if (Game.IsCheckmated(Player.Black)) return "White";
        return null;
    }
}
