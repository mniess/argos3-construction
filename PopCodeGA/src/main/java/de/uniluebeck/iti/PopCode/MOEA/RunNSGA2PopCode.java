package de.uniluebeck.iti.PopCode.MOEA;

import org.moeaframework.Executor;
import org.moeaframework.analysis.plot.Plot;
import org.moeaframework.core.NondominatedPopulation;
import org.moeaframework.core.PopulationIO;
import org.moeaframework.core.Solution;
import org.moeaframework.core.variable.EncodingUtils;

import java.io.File;
import java.io.IOException;


enum GENOME {
    SIMPLE, SIMPLECOUNT
}

public class RunNSGA2PopCode {

    public static void main(String[] args) throws Exception {
        File checkpointFile = new File("checkpoint.dat");
        if (PopCodeUtilities.gType == GENOME.SIMPLE) {
            checkpointFile = new File("checkpoint_simple.dat");
        } else if (PopCodeUtilities.gType == GENOME.SIMPLECOUNT) {
            checkpointFile = new File("checkpoint_simplecount.dat");
        } else {
            throw new Exception("Unknown genome type, don't know which checkpoint file to use");
        }

        if (checkpointFile.exists()) {
            System.out.print("Using existing checkpointfile: ");
        } else {
            System.out.print("NEW checkpointfile: ");
        }
        System.out.println(checkpointFile.getName());

        NondominatedPopulation result = new Executor()
                .withAlgorithm("NSGAII")
                .withProblemClass(NSGA2PopCode.class)
                //.distributeOnAllCores()//ERR!!!
                //.withMaxTime(1*60*1000)
                //.withMaxEvaluations(30)
                .withCheckpointFile(checkpointFile)
                .withCheckpointFrequency(3)
                .run();

        for (Solution solution : result) {
            System.out.printf("=> %.5f\n", solution.getObjective(0));
        }

        try {
            PopulationIO.write(new File("solutions.dat"), result);
            System.out.println("Saved " + result.size() + " solutions!");
        } catch (IOException e) {
            e.printStackTrace();
        }

        new Plot()
                .add("NSGAII", result)
                .show();
    }
}
