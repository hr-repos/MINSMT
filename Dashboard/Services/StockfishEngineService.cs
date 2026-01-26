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

    public async Task<string> GetMove(string fen, int depth)
    {
        engineProcess.StandardInput.WriteLine($"position fen {fen}");
        engineProcess.StandardInput.WriteLine($"go depth {depth}");

        while (true)
        {
            string line = await engineProcess.StandardOutput.ReadLineAsync();

            if (line.StartsWith("bestmove"))
            {
                string[] parts = line.Split(' ');
                return parts[1];   // Example: "e2e4" or "g1f3"
            }
        }
    }

    public void SetSkillLevel(int level)
    {
        engineProcess.StandardInput.WriteLine($"setoption name Skill Level value {level}");
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