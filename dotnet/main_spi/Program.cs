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
        var interval = TimeSpan.FromSeconds(2);

        try
        {
            while (true)
            {
                timer.Start();

                // begin transactions
                readerWriter.Tx();

                // calculate elapsed time interval
                Thread.Sleep(interval - timer.Elapsed);
                timer.Reset();
            }
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

    public void Tx()
    {
        // allocate memrory for buffer, read and write, which both needs to be the same size
        Span<byte> read = new byte[messageLength];
        Span<byte> write = new byte[messageLength];
        write[0] = 0x10;

        // begin transaction
        spidev.TransferFullDuplex(write, read);

        // convert bytes to string and print them
        Console.WriteLine(Encoding.ASCII.GetString(read.ToArray()));
        write.Clear();
        read.Clear();
    }

    public void Dispose()
    {
        spidev.Dispose();
    }
}

// See https://aka.ms/new-console-template for more information

