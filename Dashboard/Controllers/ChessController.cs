using ChessDotNet;
using Microsoft.AspNetCore.Mvc;

public class ChessController : Controller
{
    private static ChessGame game = new ChessGame();
    private static StockfishEngineService engine = new StockfishEngineService("StockfishEngine/stockfish-windows-x86-64-avx2.exe");

    public IActionResult Index()
    {
        ViewBag.FEN = game.GetFen();
        return View();
    }

    [HttpGet]
    public IActionResult State()
    {
        return Json(new { fen = game.GetFen() });
    }

    [HttpPost]
    public async Task<IActionResult> Move([FromBody] MoveRequest req)
    {
        // Player move
        var move = new Move(req.From, req.To, game.WhoseTurn);

        if (!game.IsValidMove(move))
            return Json(new { valid = false });

        game.MakeMove(move, true);

        // AI move
        if (game.WhoseTurn == Player.Black)
        {
            string fen = game.GetFen();
            string best = await engine.GetMove(fen);

            // Convert "e2e4" into ChessDotNet Move
            string from = best.Substring(0, 2);
            string to   = best.Substring(2, 2);

            var aiMove = new Move(from, to, Player.Black);

            if (game.IsValidMove(aiMove))
                game.MakeMove(aiMove, true);
        }

        string winner = null;
        if (game.IsCheckmated(Player.White)) winner = "Black";
        if (game.IsCheckmated(Player.Black)) winner = "White";

        return Json(new
        {
            valid = true,
            fen = game.GetFen(),
            winner = winner
        });
    }

    [HttpPost]
    public IActionResult Reset()
    {
        game = new ChessGame();   // Completely reset the game state
        return Json(new { fen = game.GetFen() });
    }
}

public class MoveRequest
{
    public string From { get; set; }
    public string To { get; set; }
}
