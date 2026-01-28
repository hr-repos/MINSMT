using MQTTnet;
using System.Text;
using System.Text.Json;

public class MqttListener
{
    private static IMqttClient _client;
    private static string _currentBoardCode = null;

    public static async Task Connect()
    {
        var mqttFactory = new MqttClientFactory();
        _client = mqttFactory.CreateMqttClient();

        var options = new MqttClientOptionsBuilder()
            .WithTcpServer("145.24.237.4", 8883)
            .Build();

        _client.ApplicationMessageReceivedAsync += e =>
        {
            var payload = Encoding.UTF8.GetString(e.ApplicationMessage.Payload);
            Console.WriteLine($"MQTT RECEIVED: {payload}");

            HandleBoardMessage(payload);
            return Task.CompletedTask;
        };
        
        using var timeout = new CancellationTokenSource(5000);
        await _client.ConnectAsync(options, timeout.Token);
        Console.WriteLine("MQTT CONNECTED");
    }

    public static async Task ConnectToBoard(string boardCode)
    {
        if (_currentBoardCode != null)
        {
            await _client.UnsubscribeAsync($"{_currentBoardCode}/move");
        }

        await Connect();

        _currentBoardCode = boardCode;

        await _client.SubscribeAsync($"{boardCode}/move");

        Console.WriteLine($"SUBSCRIBED TO BOARD {boardCode}");
    }

    public static async Task SendMoveToBoard(string move)
    {
        if (_client == null || !_client.IsConnected || _currentBoardCode == null)
            return;

        var message = new MqttApplicationMessageBuilder()
            .WithTopic($"{_currentBoardCode}/move")
            .WithPayload(move)
            .Build();

        await _client.PublishAsync(message);
    }

    private static async void HandleBoardMessage(string payload)
    {
        var from = payload.Substring(0,2);
        var to = payload.Substring(2,2);
        char? promotion = null;

        // If the payload has a 5th character, it's the promotion piece
        if (payload.Length >= 5)
        {
            promotion = payload[4]; // 'q', 'r', 'b', 'n'
        }

        ChessboardService.ApplyPhysicalMove(from, to, promotion);
        Console.WriteLine($"PLAYER MOVE: {from} to {to}");

        var aiMove = await ChessboardService.MakeAIMoveAsync();
        Console.WriteLine($"AI MOVE: {aiMove}");
    }
}
