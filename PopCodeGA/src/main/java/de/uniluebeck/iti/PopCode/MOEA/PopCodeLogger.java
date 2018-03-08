package de.uniluebeck.iti.PopCode.MOEA;

import org.moeaframework.core.NondominatedPopulation;
import org.moeaframework.core.Population;
import org.moeaframework.core.PopulationIO;
import org.moeaframework.core.Solution;
import org.moeaframework.util.progress.ProgressEvent;
import org.moeaframework.util.progress.ProgressListener;

import java.io.*;
import java.util.Arrays;

public class PopCodeLogger implements ProgressListener {
    Writer writer;
    private String identifier = "";
    public File checkpointFile;

    public PopCodeLogger(String identifier) {
        this.identifier = identifier;

        checkpointFile = new File(getFileAppendix() + "_checkpoint.dat");
        System.out.println((checkpointFile.exists() ? "Using existing" : "NEW") + " checkpointfile: " + checkpointFile.getName());

        try {
            File logFile = new File(getFileAppendix() + "_log.csv");
            writer = new BufferedWriter(new FileWriter(logFile, checkpointFile.exists()));
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    @Override
    public void progressUpdate(ProgressEvent event) {
        System.out.println("Generation finished!");
        log(event.getCurrentNFE(), new NondominatedPopulation());
    }

    public void log(int nfe, NondominatedPopulation solutions) {
        try {
            for (Solution s : solutions) {
                writer.write(nfe + "; ");
                writer.write(s.getObjective(0) + "; ");
                writer.write(s.getObjective(1) + "; ");
                writer.write(Arrays.toString(PopCodeUtilities.varsToArray(s)) + "; ");
                writer.write("\n");
            }
            writer.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void saveResults(Population solutions) {
        String appendix = getFileAppendix();

        //Results
        try {
            PopulationIO.write(new File(appendix + "_Solutions.dat"), solutions);
            PopulationIO.writeObjectives(new File(appendix + "_Objectives.dat"), solutions);
        } catch (IOException e) {
            e.printStackTrace();
        }

        System.out.println("Saved " + solutions.size() + " solutions!");
    }

    public String getFileAppendix() {
        return PopCodeUtilities.gType + identifier;
    }

    public Population loadResults() {
        try {
            return PopulationIO.read(new File(getFileAppendix() + "_Solutions.dat"));
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }
}
