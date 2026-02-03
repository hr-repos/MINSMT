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
        return Json(new
        {
            fen = ChessboardService.GetFen(),
            winner = ChessboardService.GetGameResult(),
            checkSquare = ChessboardService.GetCheckSquare(),
            checkAttackers = ChessboardService.GetCheckAttackers()
        });
    }

    [HttpPost]
    public async Task<IActionResult> StartGame([FromBody] StartGameRequest settings)
    {
        ChessboardService.Reset();

        ChessboardService.Mode =
            settings.Mode == "cpu" ? GameMode.HumanVsAI : GameMode.HumanVsHuman;

        ChessboardService.HumanSide =
            settings.Side == "black" ? PlayerSide.Black : PlayerSide.White;

        await MqttListener.ConnectToBoard(settings.BoardCode);

        ChessboardService.Difficulty = settings.Difficulty switch
        {
            "beginner" => AIDifficulty.Beginner,
            "intermediate" => AIDifficulty.Intermediate,
            "expert" => AIDifficulty.Expert,
            "master" => AIDifficulty.Master,
            _ => AIDifficulty.Beginner
        };

        ChessboardService.GameStarted = true;

        if (ChessboardService.Mode == GameMode.HumanVsAI && ChessboardService.HumanSide == PlayerSide.Black)
        {
            string aiMove = await ChessboardService.MakeAIMoveAsync();
        }

        return Json(new
        {
            fen = ChessboardService.GetFen(),
            orientation = ChessboardService.HumanSide == PlayerSide.White ? "white" : "black",
            checkSquare = ChessboardService.GetCheckSquare(),
            checkAttackers = ChessboardService.GetCheckAttackers()
        });
    }

    [HttpPost]
    public async Task<IActionResult> Move([FromBody] MoveRequest move)
    {
        bool success = ChessboardService.ApplyPhysicalMove(move.From, move.To, move.Promotion);

        if (!success) 
        {
            Thread.Sleep(300);
            await MqttListener.SendMoveIsIllegal(move.From + move.To);
            return Json(new
            {
                valid = false,
                fen = ChessboardService.GetFen(),
                winner = ChessboardService.GetGameResult(),
                checkSquare = ChessboardService.GetCheckSquare(),
                checkAttackers = ChessboardService.GetCheckAttackers()
            });
        }

        string winnerBeforeAi = ChessboardService.GetGameResult();
        string aiMove = null;

        if (winnerBeforeAi == null)
        {
            aiMove = await ChessboardService.MakeAIMoveAsync();
            await MqttListener.SendMoveToBoard(aiMove);
        }

        string winner = ChessboardService.GetGameResult();

        if (winner != null)
            await MqttListener.SendGameOverToBoard(winner);

        await MqttListener.SendMoveToBoard(move.From + move.To);
        return Json(new
        {
            valid = true,
            fen = ChessboardService.GetFen(),
            winner = winner,
            aiMove = aiMove,
            checkSquare = ChessboardService.GetCheckSquare(),
            checkAttackers = ChessboardService.GetCheckAttackers()
        });
    }

    [HttpPost]
    public async Task<IActionResult> Resign()
    {
        string winner = ChessboardService.Resign();

    if (winner != null)
            await MqttListener.SendGameOverToBoard(winner);

        return Json(new
        {
            winner = winner,
            fen = ChessboardService.GetFen(),
            checkSquare = ChessboardService.GetCheckSquare(),
            checkAttackers = ChessboardService.GetCheckAttackers()
        });
    }

    [HttpPost]
    public IActionResult Reset()
    {
        ChessboardService.Reset();
        return Json(new
        {
            fen = ChessboardService.GetFen(),
            winner = ChessboardService.GetGameResult(),
            checkSquare = ChessboardService.GetCheckSquare(),
            checkAttackers = ChessboardService.GetCheckAttackers()
        });
    }
}

public class StartGameRequest
{
    public string Mode { get; set; }
    public string Side { get; set; }
    public string Difficulty { get; set; }
    public string BoardCode { get; set; }
}

public class MoveRequest
{
    public string From { get; set; }
    public string To { get; set; }
    public char? Promotion { get; set; }
}

