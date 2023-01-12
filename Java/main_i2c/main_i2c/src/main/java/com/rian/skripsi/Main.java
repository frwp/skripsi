/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package com.rian.skripsi;

import com.pi4j.Pi4J;
import com.pi4j.context.Context;
import com.pi4j.io.i2c.I2C;
import com.pi4j.io.i2c.I2CConfig;
import com.pi4j.io.i2c.I2CProvider;
import com.pi4j.platform.Platforms;
import com.pi4j.util.Console;
import java.lang.reflect.InvocationTargetException;
import java.time.Duration;
import java.time.Instant;

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
            pi4j = Pi4J.newAutoContext();
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

        console.box("Pi4J PLATFORMS");
        console.println();
        platforms.describe().print(System.out);
        console.println();

        I2CProvider i2cProvider = pi4j.provider("linuxfs-i2c");
        I2CConfig i2cConfig = I2C.newConfigBuilder(pi4j)
                .id("NANO")
                .bus(1)
                .device(0x04)
                .build();

        try (I2C arduinoI2c = i2cProvider.create(i2cConfig)) {
            console.box("I2C BUS");
            console.println();
            console.println("I2C BUS: " + arduinoI2c.bus());
            console.println("I2C DEVICE: " + arduinoI2c.device());
            console.println("I2C PROVIDER: " + arduinoI2c.provider().name());
            console.println("I2C ID: " + arduinoI2c.id());
            console.println();

            console.box("I2C READ/WRITE");
            while (true) {
                Instant start = Instant.now();
                String str = new String(arduinoI2c.readNBytes(32), "UTF-8");
                Instant end = Instant.now();
                Duration elapsed = Duration.between(start, end);
                console.println(str);
                console.println("Takes " + String.valueOf(elapsed.toMillis()) + " ms");
                Thread.sleep(2000 - elapsed.toMillis());
            }
        }
    }

}
