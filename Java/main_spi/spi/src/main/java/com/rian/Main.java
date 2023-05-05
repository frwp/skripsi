/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.rian;

import com.pi4j.Pi4J;
import com.pi4j.context.Context;
import com.pi4j.io.spi.Spi;
import com.pi4j.io.spi.SpiBus;
import com.pi4j.io.spi.SpiConfig;
import com.pi4j.io.spi.SpiMode;
import com.pi4j.plugin.pigpio.provider.spi.*;
import com.pi4j.library.pigpio.PiGpio;
import com.pi4j.platform.Platforms;
import com.pi4j.util.Console;
import java.lang.reflect.InvocationTargetException;
import java.time.Duration;
import java.time.Instant;
import java.util.Arrays;
import java.nio.*;

/**
 *
 * @author Rian Wardana
 */
public class Main {

    private static final Console console = new Console();

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) throws Exception {
        console.box("Hello Rasbian world !");
        Context pi4j = null;
        try {
            var piGpio = PiGpio.newNativeInstance();
            pi4j = Pi4J.newContextBuilder()
                    .noAutoDetect()
                    .add(PiGpioSpiProvider.newInstance(piGpio))
                    .build();
            new Main().run(pi4j);
        } catch (InvocationTargetException e) {
            console.println("Error: " + e.getTargetException().getMessage());
        } catch (Exception e) {
            console.println("Error: " + e.getMessage());
            e.printStackTrace();
        } finally {
            if (pi4j != null) {
                pi4j.shutdown();
            }
        }
    }

    private void run(Context pi4j) throws Exception {
        Platforms platforms = pi4j.platforms();

        console.box("Pi4J Platforms");
        console.println();
        platforms.describe().print(System.out);
        console.println();

        SpiConfig spiConfig = Spi.newConfigBuilder(pi4j)
                .id("NANO")
                .address(0)
                .baud(400000)
                .mode(SpiMode.MODE_0)
                .bus(SpiBus.BUS_0)
                .build();

        try (Spi arduinoSpi = pi4j.create(spiConfig)) {
            console.clearScreen();
            console.box("SPI");
            console.println();
            console.println("SPI: " + arduinoSpi);
            console.println();

            console.box("SPI Configuration");
            console.println();
            console.println("SPI Configuration: " + arduinoSpi.config());
            console.println();

            console.box("SPI Provider");
            console.println();
            console.println("SPI Provider: " + arduinoSpi.provider());
            console.println();

            console.box("SPI READ/WRITE");
            while (true) {
                // byte data[] = new byte[40];
                // data[0] = 0x10;
                ByteBuffer data = ByteBuffer.allocate(40);
                data.put((byte) 0x10);
                console.println(Arrays.toString(data.array()));
                Instant start = Instant.now();
                arduinoSpi.transfer(data, 0, 36);
                String str = Arrays.toString(data.array());
                Instant end = Instant.now();
                Duration elapsed = Duration.between(start, end);
                console.println("SPI READ/WRITE: " + str + " in " + elapsed.toMillis() + " ms");
                Thread.sleep(2000 - elapsed.toMillis());
            }
        }
    }
}
