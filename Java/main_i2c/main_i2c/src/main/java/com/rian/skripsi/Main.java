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

import java.io.OutputStream;
import java.lang.reflect.InvocationTargetException;
import java.time.Duration;
import java.time.Instant;
import java.net.URL;
import java.net.HttpURLConnection;

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

        URL url = new URL("http://192.168.8.185/api");

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
            console.clearScreen();

            console.box("I2C READ/WRITE");
            while (true) {
                HttpURLConnection con = (HttpURLConnection) url.openConnection();
                con.setRequestMethod("POST");
                con.setDoOutput(true);

                Instant start = Instant.now();
                String str = new String(arduinoI2c.readNBytes(32), "UTF-8");
                str = String.valueOf(Instant.now().getEpochSecond()) + "|" + str;
                console.println(str);
                String payload = "node=node1&data=" + str;

                OutputStream os = con.getOutputStream();
                os.write(payload.getBytes());
                os.flush();
                os.close();
                int responseCode = con.getResponseCode();
                Instant end = Instant.now();
                Duration elapsed = Duration.between(start, end);
                console.println("Takes " + String.valueOf(elapsed.toMillis()) + " ms returning "
                        + String.valueOf(responseCode));
                if (elapsed.compareTo(Duration.ofSeconds(2)) > 0) {
                    continue;
                }
                Thread.sleep(2000 - elapsed.toMillis());
            }
        }
    }
}
