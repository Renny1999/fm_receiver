package com.example;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingDeque;
import java.util.concurrent.LinkedBlockingQueue;

import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.Mixer;
import javax.sound.sampled.SourceDataLine;
import javax.sound.sampled.TargetDataLine;
import javax.sound.sampled.DataLine.Info;
import javax.xml.transform.SourceLocator;
import javax.sound.sampled.LineUnavailableException;

public class AudioThread extends Thread{
	BlockingQueue<byte[]> queue;
	public AudioThread(BlockingQueue<byte[]> queue){
		this.queue = queue;
	}


	public void run() {
		System.out.println("AUDIO THREAD STARTED");
		Mixer.Info[] mixinfos = AudioSystem.getMixerInfo();
		for(int i = 0; i < mixinfos.length; i++) {
			System.out.println(i + " " + mixinfos[i].getName() + "\t" + mixinfos[i].getDescription());
		}
		
		AudioFormat format = new AudioFormat(
			AudioFormat.Encoding.PCM_SIGNED,
			48000,
			16,
			2,
			4,
			48000,
			false
		);
		
		try{
			DataLine.Info info = new DataLine.Info(SourceDataLine.class, format);
			final SourceDataLine sourceLine = (SourceDataLine) AudioSystem.getLine(info);
			sourceLine.open();

			Thread sourceThread = new Thread(){
				@Override public void run(){
				System.out.println("SOURCE THREAD STARTED");
					sourceLine.start();
					while(queue.size() < 200){
						continue;
					}
					while(true){
						if(queue.size()>0){
							byte[] data = queue.remove();
							sourceLine.write(data, 0, data.length);
						}
					}
				}
			};

			sourceThread.start();
			
		}catch(Exception e){
			e.printStackTrace();
		}

	}
}