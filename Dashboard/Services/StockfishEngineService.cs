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

        engineProcess.StandardInput.WriteLine("uci");
        engineProcess.StandardInput.WriteLine("setoption name Skill Level value 20");
    }

    public async Task<string> GetMove(string fen, int depth = 15)
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

    public void Quit()
    {
        engineProcess.StandardInput.WriteLine("quit");
        engineProcess.Close();
    }
}
