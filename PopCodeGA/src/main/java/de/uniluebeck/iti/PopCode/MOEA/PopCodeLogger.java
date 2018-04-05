package de.uniluebeck.iti.PopCode.MOEA;

import org.moeaframework.core.NondominatedPopulation;
import org.moeaframework.core.Population;
import org.moeaframework.core.PopulationIO;
import org.moeaframework.core.Solution;

import java.io.*;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.file.StandardCopyOption;
import java.nio.file.StandardOpenOption;
import java.util.Arrays;
import java.util.Date;

public class PopCodeLogger {
    private Writer writer;
    private String identifier = "";
    private String resultPath = System.getProperty("user.home") + "/ArgosResults/";
    public File checkpointFile;
    private File logFile;
    private File statsFile;
    private File errFile;

    PopCodeLogger(String identifier) {
        this.identifier = identifier;

        checkpointFile = new File(getFileAppendix() + "_checkpoint.dat");
        System.out.println((checkpointFile.exists() ? "Using existing" : "NEW") + " checkpointfile: " + checkpointFile.getName());

        try {
            logFile = new File(getFileAppendix() + "_log.csv");
            writer = new BufferedWriter(new FileWriter(logFile, checkpointFile.exists()));
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    public void log(int nfe, NondominatedPopulation solutions, double hypervolume) {
        try {
            for (Solution s : solutions) {
                writer.write(nfe + "; ");
                writer.write(hypervolume + "; ");
                writer.write(s.getObjective(0) + "; ");
                writer.write(s.getObjective(1) + "; ");
                writer.write(Arrays.toString(PopCodeUtilities.getGenome(s)) + "; ");
                writer.write("\n");
            }
            writer.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void err(String message) {
        try {
            if (errFile == null || !errFile.exists()) {
                errFile = new File(getFileAppendix() + "_err.log");
                errFile.createNewFile();
            }
            Files.write(Paths.get(getFileAppendix() + "_err.log"), Arrays.asList(message), StandardOpenOption.APPEND);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void safeStats() {
        try {
            statsFile = new File(getFileAppendix() + "_stats.txt");
            Writer w = new BufferedWriter(new FileWriter(statsFile, false));
            w.write("StartTime=" + new Date().toString() + "\n");
            w.write("GenomeType=" + PopCodeUtilities.gType + "\n");
            w.write("Sim:\n");
            w.write("numRobots=" + PopCodeUtilities.numRobots + "\n");
            w.write("robGenomeSize=" + PopCodeUtilities.robGenomeSize + "\n");
            w.write("evaluations=" + RunNSGA2PopCode.evaluations + "\n");
            w.write("GA:\n");
            w.write("populationSize=" + RunNSGA2PopCode.populationSize + "\n");
            w.write("elitism=" + RunNSGA2PopCode.elitism + "\n");
            w.write("generations=" + RunNSGA2PopCode.generations + "\n");
            w.write("GAgenomeSize=" + PopCodeUtilities.GAgenomeSize + "\n");
            w.write("PopCodegenomeSize=" + PopCodeUtilities.PopCodegenomeSize + "\n");
            w.flush();
            w.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void saveResults(Population solutions) {
        String appendix = getFileAppendix();

        //Results
        try {
            PopulationIO.write(new File(resultPath + appendix + "_Solutions.dat"), solutions);
            PopulationIO.writeObjectives(new File(resultPath + appendix + "_Objectives.dat"), solutions);
        } catch (IOException e) {
            e.printStackTrace();
        }

        System.out.println("Saved " + solutions.size() + " solutions!");
    }

    private String getFileAppendix() {
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

    public void cleanUp() {
        new File(resultPath).mkdirs();

        if (checkpointFile.exists()) {
            checkpointFile.delete();
        }

        moveToResultPath(logFile);
        moveToResultPath(statsFile);
        moveToResultPath(errFile);

    }

    private void moveToResultPath(File f) {
        try {
            if (f != null && f.exists()) {
                File dst = new File(resultPath + f.getName());
                Files.move(f.toPath(), dst.toPath(), StandardCopyOption.REPLACE_EXISTING);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
