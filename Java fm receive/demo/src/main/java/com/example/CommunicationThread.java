package com.example;
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * CommunicationThread takes in data from the server and pass it into dataq
 */
public class CommunicationThread extends Thread {
    BlockingQueue<byte[]> dataq;
    Socket socket;
    String IP;
    int PORT;

    public DataInputStream inputStream = null;
    public DataOutputStream outputStream = null;

    public CommunicationThread(String IP, int PORT, BlockingQueue<byte[]> dataq) {
        this.IP = IP;
        this.PORT = PORT;
        this.dataq = dataq;
        socket = null;
    }

    public void run() {
        establishConnection();
        int CHUNK = 512;
        byte[] buffer = new byte[CHUNK];
        AtomicBoolean exitbool = new AtomicBoolean(false);
        // Thread kat = new Thread(new KeepAliveThread(this.socket, exitbool));
        // kat.start();

        while (!exitbool.get()) {
            try {
                inputStream.read(buffer);
                dataq.put(buffer);
                buffer = new byte[CHUNK];
            } catch (Exception e) {
                e.printStackTrace(); 
                return;
            }
        }
        System.out.println("connection timed out");
    }

    public void establishConnection(){
        try {
            System.out.println("CT:\tconnecting to " + IP + ":" + PORT);
            socket = new Socket(IP, PORT);
            System.out.println("CT:\tconnection established to " + IP + ":" + PORT);

            this.inputStream = new DataInputStream(new BufferedInputStream(socket.getInputStream()));
            this.outputStream = new DataOutputStream(new BufferedOutputStream(socket.getOutputStream()));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static byte convert2byte(float by) {
		float sample = by;
		
		float maxSampleVal = (float) Byte.MAX_VALUE;
		float minSampleVal = (float) 0;
		float normalizeFac = maxSampleVal/2;
		float normalizeShift = 1;
		
		
		sample+=normalizeShift;
		sample*=normalizeFac;
		
		sample = Math.max(Math.min(sample, maxSampleVal), minSampleVal);
		return (byte) Math.round(sample);
	}
    public byte compiledata(byte[] bytes){
        float f = ByteBuffer.wrap(bytes).order(ByteOrder.LITTLE_ENDIAN).getFloat();
        return convert2byte(f);
    }
}
