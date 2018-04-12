package de.uniluebeck.iti.PopCode.MOEA;

import org.moeaframework.algorithm.Checkpoints;
import org.moeaframework.algorithm.NSGAII;
import org.moeaframework.analysis.plot.Plot;
import org.moeaframework.core.*;
import org.moeaframework.core.comparator.ChainedComparator;
import org.moeaframework.core.comparator.CrowdingComparator;
import org.moeaframework.core.comparator.ParetoDominanceComparator;
import org.moeaframework.core.indicator.Hypervolume;
import org.moeaframework.core.operator.GAVariation;
import org.moeaframework.core.operator.RandomInitialization;
import org.moeaframework.core.operator.TournamentSelection;
import org.moeaframework.core.operator.binary.BitFlip;
import org.moeaframework.core.operator.binary.HUX;
import org.moeaframework.core.operator.real.PM;
import org.moeaframework.core.operator.real.SBX;

public class RunNSGA2PopCode {

    public static Settings settings;

    public static void main(String[] args) {
        //Set up
        settings = new Settings("GA.properties");
        PopCodeLogger logger;
        if (args.length == 1) {
            logger = new PopCodeLogger(args[0], settings);
        } else {
            logger = new PopCodeLogger("", settings);
        }
        logger.safeStats();

        //Run
        Population solutions = evaluate(logger, settings);

        //Results
        logger.saveResults(solutions);

        for (Solution solution : solutions) {
            System.out.printf("=> %.5f ; %.5f\n", solution.getObjective(0), solution.getObjective(1));
        }

        plot(solutions);

    }

    private static Population evaluate(PopCodeLogger logger, Settings s) {

        Problem problem = new NSGA2PopCode(s, logger);
        double[] idealPoint = {-1, -1};
        double[] referencePoint = {1, 1}; //some distance from worst case {0,0}
        Hypervolume hypervolume = new Hypervolume(problem, idealPoint, referencePoint);
        Initialization initialization = new RandomInitialization(
                problem,
                s.populationSize);

        TournamentSelection selection = new TournamentSelection(s.tournamentSize,
                new ChainedComparator(
                        new ParetoDominanceComparator(), //Try first
                        new CrowdingComparator()));     // Backup

        Variation variation = new GAVariation(
                new HUX(1.0),
                new BitFlip(0.01));
                //new SBX(1.0, 25.0),
                //new PM(1.0 / problem.getNumberOfVariables(), 30.0));

        Algorithm algorithm = new NSGAII(
                problem,
                new NondominatedSortingPopulation(),
                null,
                selection,
                variation,
                initialization);

        // Make sure you delete the checkpoints file after a successful run
        algorithm = new Checkpoints(algorithm, logger.checkpointFile, 1);

        while (algorithm.getNumberOfEvaluations() < s.generations * s.populationSize) {
            algorithm.step();
            logger.log(algorithm.getNumberOfEvaluations(), algorithm.getResult(), hypervolume.evaluate(algorithm.getResult()));
        }
        logger.cleanUp();
        return algorithm.getResult();
    }

    private static void evalSaved(PopCodeLogger logger) {
        Population pop = logger.loadResults();
        plot(pop);
    }

    private static void plot(Population solutions) {
        if (solutions != null) {
            new Plot()
                    .add("Pareto optimal Solutions", solutions)
                    .show();
        }
    }
}
