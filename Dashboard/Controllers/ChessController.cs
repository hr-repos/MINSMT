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
    public async Task<IActionResult> Move([FromBody] MoveRequest req)
    {
        bool success = ChessboardService.ApplyPhysicalMove(req.From, req.To);

        if (!success)
            return Json(new { valid = false, fen = ChessboardService.GetFen() });

        // Make AI move if applicable
        string aiMove = await ChessboardService.MakeAIMoveAsync();

        // Check for winner
        string winner = ChessboardService.GetWinner();

        return Json(new
        {
            valid = true,
            fen = ChessboardService.GetFen(),
            winner = winner,
            aiMove = aiMove
        });
    }

    [HttpPost]
    public IActionResult Reset()
    {
        ChessboardService.Reset();
        return Json(new { fen = ChessboardService.GetFen() });
    }
}

public class MoveRequest
{
    public string From { get; set; }
    public string To { get; set; }
}
