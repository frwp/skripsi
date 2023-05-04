using System.Device.Spi;
using System.Text;
using System.Diagnostics;

public class Main_Spi
{
    public static void Main()
    {
        // instantiate our spi reader class, with data length = 58
        ReaderWriter readerWriter = new ReaderWriter(58);

        // instantiate stopwatch object, for measuring delays
        Stopwatch timer = new Stopwatch();
        Stopwatch timer2 = new Stopwatch();
        var interval = TimeSpan.FromSeconds(2);

        try
        {
            timer2.Start();
            while (TimeSpan.FromHours(1) > timer2.Elapsed)
            {
                timer.Start();

                // begin transactions
                readerWriter.Tx();

                // calculate elapsed time interval
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
    private SpiConnectionSettings spiSettings;
    private SpiDevice spidev;
    private HttpClient client = new HttpClient();

    // Constructor
    // @params int messageLength
    // determine max data length of sent and received buffer
    // also instantiate spi driver.
    public ReaderWriter(int messageLength)
    {
        this.messageLength = messageLength;
        spiSettings = new SpiConnectionSettings(0, 0)
        {
            ClockFrequency = 400000,
            Mode = SpiMode.Mode0,
            DataFlow = DataFlow.MsbFirst
        };
        spidev = SpiDevice.Create(spiSettings);
    }

    public async void Tx()
    {
        string transfer()
        {
            // allocate memrory for buffer, read and write, which both needs to be the same size
            Span<byte> read = new byte[messageLength];
            Span<byte> write = new byte[messageLength];
            write[0] = 0x10;

            // begin transaction
            spidev.TransferFullDuplex(write, read);

            // convert bytes to string and return them
            return Encoding.ASCII.GetString(read.ToArray());
        }

        var data = transfer();
        var timestamp = DateTimeOffset.UtcNow.ToUnixTimeSeconds().ToString();
        data = timestamp + "|" + data;
        Console.WriteLine(data);

        Dictionary<string, string> postData = new Dictionary<string, string>();

        postData.Add("node", "node1");
        postData.Add("data", data);

        FormUrlEncodedContent payload = new FormUrlEncodedContent(postData);

        await PostAsync(client, "http://192.168.8.185/api", payload);

    }

    private async Task PostAsync(HttpClient httpClient, string url, FormUrlEncodedContent payload)
    {
        HttpResponseMessage response = await httpClient.PostAsync(url, payload);
        if (!response.IsSuccessStatusCode)
        {
            Console.WriteLine(response.ToString());
        }
    }

    public void Dispose()
    {
        spidev.Dispose();
    }
}

// See https://aka.ms/new-console-template for more information

