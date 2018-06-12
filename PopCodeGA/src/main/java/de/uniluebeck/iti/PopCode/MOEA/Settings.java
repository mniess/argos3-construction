package de.uniluebeck.iti.PopCode.MOEA;

import java.io.*;
import java.util.Properties;

/**
 * Settings reader using java Properties.
 * Read variables once and safe them to final variables to protect them from changes
 */
class Settings {
    // Number of robots in each simulation
    final int numRobots;

    //Number of simulations in each generation
    final int populationSize;
    // Generations
    final int generations;
    // Number of evaluations per simulation
    final int evaluations;

    // From how many simulations the best is chosen for selection
    final int tournamentSize;
    // crossover probability
    final double crossoverProb;
    // mustatuin probaiblity
    final double mutationProb;

    // Which GENOME to use
    final GENOME gType;
    // Write finished log etc to this
    final String resultPath;

    Settings(String filename) {
        InputStream input = null;

        Properties prop = new Properties();
        try {
            input = new FileInputStream(filename);
            prop.load(input);
        } catch (IOException e) {
            System.err.println(filename + " not found! Using defaults.");
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
        mutationProb = Double.parseDouble((String) prop.getOrDefault("mutationProb", "0.01"));

        gType = GENOME.valueOf((String) prop.getOrDefault("gType", "FULLCOUNT"));
        resultPath = (String) prop.getOrDefault("resultPath", System.getProperty("user.home") + "/ArgosResults/");
    }

    /**
     * Output all settings to std::out
     */
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
