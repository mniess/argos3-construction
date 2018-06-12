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
import org.moeaframework.core.operator.OnePointCrossover;
import org.moeaframework.core.operator.RandomInitialization;
import org.moeaframework.core.operator.TournamentSelection;
import org.moeaframework.core.operator.binary.BitFlip;

public class Run {

    static Settings settings;

    public static void main(String[] args) {
        //Set up
        settings = new Settings("GA.properties");
        settings.print();
        Logger logger;
        if (args.length == 1) {
            logger = new Logger(args[0]);
        } else {
            logger = new Logger();
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

    /*
     * Setup and run the GeneticAlgorithm
     */
    private static Population evaluate(Logger logger, Settings s) {

        Problem problem = new PopCode(s, logger);

        //Create HyperVolume
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
                new OnePointCrossover(s.crossoverProb),
                new BitFlip(s.mutationProb));

        Algorithm algorithm = new NSGAII(
                problem,
                new NondominatedSortingPopulation(),
                null,
                selection,
                variation,
                initialization);

        //Create checkpointfile, it will be deleted at the end of the run by logger.cleanup();
        algorithm = new Checkpoints(algorithm, logger.checkpointFile, 1);

        //run
        while (algorithm.getNumberOfEvaluations() < s.generations * s.populationSize) {
            algorithm.step();
            logger.log(algorithm.getNumberOfEvaluations(), algorithm.getResult(), hypervolume.evaluate(algorithm.getResult()));
        }

        logger.cleanUp();
        return algorithm.getResult();
    }


    //Use MOEA Plot function for instant visualization
    private static void plot(Population solutions) {
        if (solutions != null) {
            new Plot()
                    .add("Pareto optimal Solutions", solutions)
                    .show();
        }
    }
}
