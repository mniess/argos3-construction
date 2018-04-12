package de.uniluebeck.iti.PopCode.MOEA;

import java.io.*;
import java.util.Properties;

class Settings {

    final protected int populationSize;
    final protected int tournamentSize;
    final protected int generations;
    final protected int evaluations;
    final protected int numRobots;
    final protected GENOME gType;
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

        populationSize = Integer.parseInt((String) prop.getOrDefault("populationSize", "100"));
        tournamentSize = Integer.parseInt((String) prop.getOrDefault("tournamentSize", "2"));
        generations = Integer.parseInt((String) prop.getOrDefault("generations", "1000"));
        evaluations = Integer.parseInt((String) prop.getOrDefault("evaluations", "3"));
        numRobots = Integer.parseInt((String) prop.getOrDefault("numRobots", "10"));
        gType = GENOME.valueOf((String) prop.getOrDefault("gType", "FULLCOUNT"));
        resultPath = (String) prop.getOrDefault("resultPath", System.getProperty("user.home") + "/ArgosResults/");

    }

    void print() {
        System.out.printf("PopulationSize=%d\ntournamentSize=%d\ngenerations=%d\nevaluations=%d\nnumRobots=%d\ngType=%s\nresultPath=%s\n", populationSize, tournamentSize, generations, evaluations, numRobots, gType, resultPath);
    }
}
