using System.Diagnostics;
using System.IO;
using System.Threading.Tasks;

public class StockfishEngineService
{
    private Process engineProcess;

    public StockfishEngineService(string enginePath)
    {
        engineProcess = new Process();
        engineProcess.StartInfo.FileName = enginePath;
        engineProcess.StartInfo.RedirectStandardInput = true;
        engineProcess.StartInfo.RedirectStandardOutput = true;
        engineProcess.StartInfo.UseShellExecute = false;
        engineProcess.StartInfo.CreateNoWindow = true;
        engineProcess.Start();

        // Welcome Read
        engineProcess.StandardOutput.ReadLine();
    }

    public async Task<string> GetMove(string fen)
    {
        var (elo, movetime) = ChessboardService.GetDifficultySettings();
        // Limit strength by ELO
        engineProcess.StandardInput.WriteLine("setoption name UCI_LimitStrength value true");
        engineProcess.StandardInput.WriteLine($"setoption name UCI_Elo value {elo}");

        // Higher MultiPV = worse moves (max 5)
        engineProcess.StandardInput.WriteLine("setoption name MultiPV value 5");

        engineProcess.StandardInput.WriteLine($"position fen {fen}");
        // Time = how long to think (ms)
        engineProcess.StandardInput.WriteLine($"go movetime {movetime}");

        var pvMoves = new List<string>();

        while (true)
        {
            string line = await engineProcess.StandardOutput.ReadLineAsync();

            if (line.StartsWith("info") && line.Contains(" multipv "))
            {
                var pvIndex = line.IndexOf(" pv ");
                if (pvIndex > 0)
                {
                    var move = line.Substring(pvIndex + 4).Split(' ')[0];
                    if (!pvMoves.Contains(move))
                        pvMoves.Add(move);
                }
            }

            if (line.StartsWith("bestmove"))
                break;
        }
        if (ChessboardService.Difficulty == AIDifficulty.Beginner && pvMoves.Count > 1 && Random.Shared.NextDouble() < 0.5)
            return pvMoves[Random.Shared.Next(1, pvMoves.Count)];  // Return a random move from the list for beginner difficulty
        return pvMoves[0];  // Return the best move
    }

    public void Quit()
    {
        engineProcess.StandardInput.WriteLine("quit");
        engineProcess.Close();
    }
}

public enum AIDifficulty
{
    Beginner,
    Intermediate,
    Expert,
    Master
}