package de.uniluebeck.iti.PopCode.MOEA;

import org.moeaframework.util.progress.ProgressEvent;
import org.moeaframework.util.progress.ProgressListener;

import java.io.*;

public class PopCodeLogger implements ProgressListener {
    Writer writer;
    long oldTime;

    public PopCodeLogger(boolean keepFile){
        try {
            File logFile = new File(RunNSGA2PopCode.getFileAppendix() + "_log.dat");
            writer = new BufferedWriter(new FileWriter(logFile,keepFile));
            oldTime = System.currentTimeMillis();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    @Override
    public void progressUpdate(ProgressEvent event) {
        System.out.println("Writing NFE to log file!");
        try {
            writer.write(event.getCurrentNFE()+"; ");
            writer.write(event.getElapsedTime()+"; ");
            writer.write("\n");
            writer.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
