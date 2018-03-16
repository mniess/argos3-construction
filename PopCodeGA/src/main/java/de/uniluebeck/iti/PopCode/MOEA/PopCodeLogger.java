package de.uniluebeck.iti.PopCode.MOEA;

import org.moeaframework.core.NondominatedPopulation;
import org.moeaframework.core.Population;
import org.moeaframework.core.PopulationIO;
import org.moeaframework.core.Solution;
import org.moeaframework.util.progress.ProgressEvent;
import org.moeaframework.util.progress.ProgressListener;

import java.io.*;
import java.nio.file.Files;
import java.nio.file.StandardCopyOption;
import java.util.Arrays;
import java.util.Date;

public class PopCodeLogger implements ProgressListener {
    private Writer writer;
    private String identifier = "";
    public File checkpointFile;
    private File logFile;
    private File statsFile;

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

    @Override
    public void progressUpdate(ProgressEvent event) {
        System.out.println("Generation finished!");
    }

    public void log(int nfe, NondominatedPopulation solutions, double hypervolume) {
        try {
            for (Solution s : solutions) {
                writer.write(nfe + "; ");
                writer.write(hypervolume + "; ");
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

    public void safeStats(){
        try {
            statsFile = new File(getFileAppendix() + "_stats.txt");
            Writer w = new BufferedWriter(new FileWriter(statsFile, false));
            w.write("StartTime="+new Date().toString()+"\n");
            w.write("GenomType="+PopCodeUtilities.gType+"\n");
            w.write("Sim:\n");
            w.write("numRobots="+PopCodeUtilities.numRobots+"\n");
            w.write("robGenomeSize="+PopCodeUtilities.robGenomeSize+"\n");
            w.write("evaluations="+RunNSGA2PopCode.evaluations+"\n");
            w.write("GA:\n");
            w.write("populationSize="+RunNSGA2PopCode.populationSize+"\n");
            w.write("elitism="+RunNSGA2PopCode.elitism+"\n");
            w.write("generations="+RunNSGA2PopCode.generations+"\n");
            w.write("GAgenomeSize="+PopCodeUtilities.GAgenomeSize+"\n");
            w.write("PopCodegenomeSize="+PopCodeUtilities.PopCodegenomeSize+"\n");
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
            PopulationIO.write(new File(appendix + "_Solutions.dat"), solutions);
            PopulationIO.writeObjectives(new File(appendix + "_Objectives.dat"), solutions);
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
        if(checkpointFile.exists()){
            checkpointFile.delete();
        }
        try {
            if(logFile.exists()){
                File dst = new File(System.getProperty("user.home")+"/ArgosResults/"+logFile.getName());
                dst.getParentFile().mkdirs();
                Files.copy(logFile.toPath(),dst.toPath(), StandardCopyOption.REPLACE_EXISTING);
            }
            if(statsFile.exists()){
                File dst = new File(System.getProperty("user.home")+"/ArgosResults/"+statsFile.getName());
                Files.copy(statsFile.toPath(),dst.toPath(), StandardCopyOption.REPLACE_EXISTING);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
