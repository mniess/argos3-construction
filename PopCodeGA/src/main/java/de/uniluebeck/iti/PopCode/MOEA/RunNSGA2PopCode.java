package de.uniluebeck.iti.PopCode.MOEA;

import org.moeaframework.Executor;
import org.moeaframework.analysis.plot.Plot;
import org.moeaframework.core.NondominatedPopulation;
import org.moeaframework.core.PopulationIO;
import org.moeaframework.core.Solution;
import org.moeaframework.core.variable.EncodingUtils;

import java.io.File;
import java.io.IOException;


public class RunNSGA2PopCode {
    public static void main(String[] args) {
        File checkpointFile = new File("checkpoint.dat");

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
