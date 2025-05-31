package com.example;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

public class App {
    public static void main(String[] args){

        String addr;
        int PORT; 
        if(args.length == 0){
            addr = "72.194.1.168";
            PORT = 4500;
        }else{
            addr = args[0];
            PORT = Integer.parseInt(args[1]);
        }
        BlockingQueue<byte[]> dataq = new LinkedBlockingQueue<>();
        CommunicationThread ct = new CommunicationThread(addr, PORT, dataq);
		AudioThread at = new AudioThread(dataq);

        ct.start();
		at.start();
    }
}
