// See https://aka.ms/new-console-template for more information
// Console.WriteLine("Hello, World!");

using System;
using System.Device.I2c;
using System.Text;
using System.Diagnostics;

public class Main_I2C
{
    public static void Main()
    {
        ReaderWriter readerWriter = new ReaderWriter(32, 0x04);

        Stopwatch timer = new Stopwatch();
        Stopwatch timer2 = new Stopwatch();
        var interval = TimeSpan.FromSeconds(2);

        try
        {
            timer2.Start();
            while (TimeSpan.FromHours(1) > timer2.Elapsed)
            {
                timer.Start();

                readerWriter.Tx();

                Thread.Sleep(interval - timer.Elapsed);
                timer.Reset();
            }
            timer.Stop();
            timer2.Stop();
        }
        finally
        {
            readerWriter.Dispose();
        }
    }
}

class ReaderWriter
{
    private int messageLength;
    private I2cConnectionSettings i2cSettings;
    private I2cDevice i2cDev;
    private HttpClient client = new HttpClient();

    public ReaderWriter(int messageLength, int slaveAddress)
    {
        this.messageLength = messageLength;
        i2cSettings = new I2cConnectionSettings(1, slaveAddress);
        i2cDev = I2cDevice.Create(i2cSettings);
    }

    public async void Tx()
    {
        string transfer()
        {
            Span<byte> buf = new byte[messageLength];

            i2cDev.Read(buf);

            return Encoding.ASCII.GetString(buf.ToArray());
        }

        var data = transfer();
        var timestamp = DateTimeOffset.UtcNow.ToUnixTimeSeconds().ToString();
        data = timestamp + "|" + data;
        Console.WriteLine(data);

        Dictionary<string, string> postData = new Dictionary<string, string>(){
            {"node", "node1"}, {"data", data}
        };

        FormUrlEncodedContent payload = new FormUrlEncodedContent(postData);

        await PostAsync(client, "http://192.168.8.185/api", payload);
    }

    private async Task PostAsync(HttpClient httpClient, string url, FormUrlEncodedContent payload)
    {
        HttpResponseMessage responseMessage = await httpClient.PostAsync(url, payload);
        if (!responseMessage.IsSuccessStatusCode)
        {
            Console.WriteLine(responseMessage.ToString());
        }
    }

    public void Dispose()
    {
        i2cDev.Dispose();
    }
}