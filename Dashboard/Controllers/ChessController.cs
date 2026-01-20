using ChessDotNet;
using Microsoft.AspNetCore.Mvc;

namespace TestCode.Controllers;

public class ChessController : Controller
{
    public IActionResult Index()
    {
        ViewBag.FEN = ChessboardService.GetFen();
        return View();
    }

    [HttpGet]
    public IActionResult State()
    {
        return Json(new { fen = ChessboardService.GetFen() });
    }

    [HttpPost]
public async Task<IActionResult> StartGame([FromBody] StartGameRequest settings)
{
    ChessboardService.Reset();

    ChessboardService.Mode =
        settings.Mode == "cpu" ? GameMode.HumanVsAI : GameMode.HumanVsHuman;

    ChessboardService.HumanSide =
        settings.Side == "black" ? PlayerSide.Black : PlayerSide.White;

    ChessboardService.GameStarted = true;

    // If playing vs AI and human chose Black → AI moves first
    if (ChessboardService.Mode == GameMode.HumanVsAI &&
        ChessboardService.HumanSide == PlayerSide.Black)
    {
        await ChessboardService.MakeAIMoveAsync();
    }

    return Json(new
    {
        fen = ChessboardService.GetFen(),
        orientation = ChessboardService.HumanSide == PlayerSide.White ? "white" : "black"
    });
}


    [HttpPost]
    public async Task<IActionResult> Move([FromBody] MoveRequest move)
    {
        bool success = ChessboardService.ApplyPhysicalMove(move.From, move.To);

        if (!success)
            return Json(new { valid = false, fen = ChessboardService.GetFen(), winner = ChessboardService.GetGameResult() });

        // Make AI move if applicable
        string aiMove = await ChessboardService.MakeAIMoveAsync();

        // Check for winner
        string winner = ChessboardService.GetGameResult();

        return Json(new
        {
            valid = true,
            fen = ChessboardService.GetFen(),
            winner = winner,
            aiMove = aiMove
        });
    }

    [HttpPost]
    public IActionResult Resign()
    {
        string winner = ChessboardService.Resign();
        return Json(new { winner = winner });
    }

    [HttpPost]
    public IActionResult Reset()
    {
        ChessboardService.Reset();
        return Json(new { fen = ChessboardService.GetFen() });
    }
}

public class StartGameRequest
{
    public string Mode { get; set; } // vs human or vs ai
    public string Side { get; set; } // start as white or black
}

public class MoveRequest
{
    public string From { get; set; }
    public string To { get; set; }
}
