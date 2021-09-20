package com.example;

import java.net.Socket;
import java.util.concurrent.atomic.AtomicBoolean;
import java.io.BufferedOutputStream;
import java.io.IOException;

public class KeepAliveThread implements Runnable{
	Socket socket = null;
	AtomicBoolean exitbool = null;
	BufferedOutputStream out = null;
	public KeepAliveThread(Socket s, AtomicBoolean ab){
		this.socket = s;
		this.exitbool = ab;
	}

	@Override
	public void run() {
		System.out.println("Keep alive started.");
		byte[] keepalive = new byte[1];
		try{
			this.out = new BufferedOutputStream(this.socket.getOutputStream());
		}catch(Exception e){
			return;
		}
		while(!exitbool.get()){
			try{
				Thread.sleep(1000);
				System.out.println("writing");
				out.write(keepalive);
			}catch(InterruptedException inte){
				continue;
			}catch(IOException ie){
				System.out.println("Cannot send");
				exitbool.set(true);
			}
		}
	}
	
}
