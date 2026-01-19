using MQTTnet;
using System.Text;
using System.Text.Json;

public class MqttListener
{
    private static IMqttClient _client;

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
        await _client.SubscribeAsync("move");
        Console.WriteLine("MQTT CONNECTED & SUBSCRIBED");
    }

    private static void HandleBoardMessage(string payload)
    {
        var from = payload.Substring(0,2);
        var to = payload.Substring(2,2);
        var success = ChessboardService.ApplyPhysicalMove(from, to);

        Console.WriteLine(success
            ? $"MOVE APPLIED: {from} -> {to}"
            : $"ILLEGAL MOVE: {from} -> {to}");
    }
}
