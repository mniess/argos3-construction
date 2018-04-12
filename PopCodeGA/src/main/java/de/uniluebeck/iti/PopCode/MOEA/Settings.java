package de.uniluebeck.iti.PopCode.MOEA;

import java.io.*;
import java.util.Properties;

class Settings {
    // Number of robots in each simulation
    //TODO migrate to Buildingsim experiment files
    final protected int numRobots;

    //Number of simulations in each generation
    final protected int populationSize;
    // Generations
    final protected int generations;
    // Number of evaluations per simulation
    final protected int evaluations;

    // From how many simulations the best is chosen for selection
    final protected int tournamentSize;
    // crossover probability
    final protected double crossoverProb;
    // mustatuin probaiblity
    final protected double mutationProb;

    // Which GENOME to use
    final protected GENOME gType;
    // Write finished log etc to this
    final protected String resultPath;

    private Properties prop = new Properties();

    Settings(String filename) {
        InputStream input = null;

        try {
            input = new FileInputStream(filename);
            prop.load(input);
        } catch (IOException e) {
            System.err.println(filename + " not found!");
        } finally {
            if (input != null) {
                try {
                    input.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        numRobots = Integer.parseInt((String) prop.getOrDefault("numRobots", "10"));
        populationSize = Integer.parseInt((String) prop.getOrDefault("populationSize", "100"));
        generations = Integer.parseInt((String) prop.getOrDefault("generations", "1000"));
        evaluations = Integer.parseInt((String) prop.getOrDefault("evaluations", "3"));

        tournamentSize = Integer.parseInt((String) prop.getOrDefault("tournamentSize", "2"));
        crossoverProb = Double.parseDouble((String) prop.getOrDefault("crossoverProb", "1"));
        mutationProb = Double.parseDouble((String) prop.getOrDefault("mutationProb", "0.1"));

        gType = GENOME.valueOf((String) prop.getOrDefault("gType", "FULLCOUNT"));
        resultPath = (String) prop.getOrDefault("resultPath", System.getProperty("user.home") + "/ArgosResults/");
    }

    void print() {
        System.out.print("numRobots=" + numRobots +
                "\nPopulationSize=" + populationSize +
                "\ngenerations=" + generations +
                "\nevaluations=" + evaluations +
                "\ntournamentSize=" + tournamentSize +
                "\ncrossoverProb=" + crossoverProb +
                "\nmutationProb=" + mutationProb +
                "\ngType=" + gType +
                "\nresultPath=" + resultPath +
                "\n");
    }
}
