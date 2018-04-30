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
import java.util.Collections;
import java.util.Date;

class Logger {
    private Writer writer;
    private String identifier = "";
    File checkpointFile;
    private File logFile;
    private File statsFile;
    private File errFile;
    private Settings settings;


    Logger() {
        this(null);
    }

    Logger(String identifier) {
        this.identifier = identifier;
        this.settings = Run.settings;

        //Generate identifier if none is given
        if (identifier == null || identifier.equals("")) {
            this.identifier = settings.gType.toString() + settings.generations;
        }

        try {
            checkpointFile = new File(identifier + "_checkpoint.dat");
            System.out.println((checkpointFile.exists() ? "Using existing" : "NEW") + " checkpointfile: " + checkpointFile.getName());
            logFile = new File(identifier + "_log.csv");
            writer = new BufferedWriter(new FileWriter(logFile, checkpointFile.exists()));
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    /**
     * Log current performance of Algorithm including current solution
     *
     * @param nfe         Current evaluation (number of evaluations)
     * @param solutions   current pareto front
     * @param hypervolume current hypervolume of solutions
     */
    void log(int nfe, NondominatedPopulation solutions, double hypervolume) {
        try {
            for (Solution s : solutions) {
                writer.write(nfe + "; ");
                writer.write(hypervolume + "; ");
                writer.write(s.getObjective(0) + "; "); //Cylinder Coverage
                writer.write(s.getObjective(1) + "; "); //Light amount
                writer.write(s.getObjective(2) + "; "); //Sparsity
                writer.write(Arrays.toString(Utilities.getGenome(s)) + "; ");
                writer.write("\n");
            }
            writer.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * Write to error log. File is only created, if method called
     *
     * @param message error line
     */
    void err(String message) {
        try {
            System.err.println(message);
            if (errFile == null || !errFile.exists()) {
                errFile = new File(identifier + "_err.log");
                errFile.createNewFile();
            }
            Files.write(Paths.get(identifier + "_err.log"), Collections.singletonList(message), StandardOpenOption.APPEND);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * Write some info about the current run into a stats file
     */
    void safeStats() {
        try {
            statsFile = new File(identifier + "_stats.txt");
            Writer w = new BufferedWriter(new FileWriter(statsFile, false));
            w.write("StartTime=" + new Date().toString() + "\n");
            w.write("GenomeType=" + settings.gType + "\n");
            w.write("Sim:\n");
            w.write("numRobots=" + settings.numRobots + "\n");
            w.write("robGenomeSize=" + Utilities.robGenomeSize + "\n");
            w.write("evaluations=" + settings.evaluations + "\n");
            w.write("GA:\n");
            w.write("populationSize=" + settings.populationSize + "\n");
            w.write("generations=" + settings.generations + "\n");
            w.write("tournamentSize=" + settings.tournamentSize + "\n");
            w.write("crossoverProb=" + settings.crossoverProb + "\n");
            w.write("mutationProb=" + settings.mutationProb + "\n");
            w.write("GAgenomeSize=" + Utilities.GAgenomeSize + "\n");
            w.write("PopCodegenomeSize=" + Utilities.PopCodegenomeSize + "\n");
            w.flush();
            w.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    /**
     * Use MOEA function to safe the final solutions of the pareto front
     *
     * @param solutions
     */
    void saveResults(Population solutions) {
        //Results
        try {
            PopulationIO.write(new File(settings.resultPath + identifier + "_Solutions.dat"), solutions);
            PopulationIO.writeObjectives(new File(settings.resultPath + identifier + "_Objectives.dat"), solutions);
        } catch (IOException e) {
            e.printStackTrace();
        }

        System.out.println("Saved " + solutions.size() + " solutions!");
    }

    Population loadResults() {
        try {
            return PopulationIO.read(new File(identifier + "_Solutions.dat"));
        } catch (IOException e) {
            e.printStackTrace();
        }
        return null;
    }

    void cleanUp() {
        new File(settings.resultPath).mkdirs();

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
                File dst = new File(settings.resultPath + f.getName());
                Files.move(f.toPath(), dst.toPath(), StandardCopyOption.REPLACE_EXISTING);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
