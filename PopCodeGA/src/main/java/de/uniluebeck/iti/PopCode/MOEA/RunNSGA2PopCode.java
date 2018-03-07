package de.uniluebeck.iti.PopCode.MOEA;

import org.moeaframework.Analyzer;
import org.moeaframework.Executor;
import org.moeaframework.Instrumenter;
import org.moeaframework.analysis.collector.Accumulator;
import org.moeaframework.analysis.plot.Plot;
import org.moeaframework.core.NondominatedPopulation;
import org.moeaframework.core.Population;
import org.moeaframework.core.PopulationIO;
import org.moeaframework.core.Solution;

import java.io.File;
import java.io.IOException;

public class RunNSGA2PopCode {

    private static String identifier = "";

    public static void main(String[] args) {
        if (args.length == 1) {
            identifier = "_" + args[0];
        }

        //evalSaved();
        Instrumenter instrumenter = getInstrumenter();

        Population solutions = evaluate(instrumenter);

        try {
            saveResults(instrumenter, solutions);
        } catch (IOException e) {
            e.printStackTrace();
        }

        for (Solution solution : solutions) {
            System.out.printf("=> %.5f ; %.5f\n", solution.getObjective(0), solution.getObjective(1));
        }

        plot(instrumenter, solutions);

    }

    private static Instrumenter getInstrumenter() {
        return new Instrumenter()
                .withProblemClass(NSGA2PopCode.class)
                .withFrequency(1)
                .attachElapsedTimeCollector()
                .attachAdaptiveMultimethodVariationCollector()
                .attachAdaptiveTimeContinuationCollector()
                .attachApproximationSetCollector()
                .attachPopulationSizeCollector()
                //needs reference set:
                //.attachHypervolumeCollector()
                //.attachGenerationalDistanceCollector()
                //.attachInvertedGenerationalDistanceCollector()
                //.attachSpacingCollector()
                //.attachAdditiveEpsilonIndicatorCollector()
                //.attachContributionCollector()
                //.attachR1Collector()
                //.attachR2Collector()
                //.attachR3Collector()
                ;

    }

    private static Population evaluate(Instrumenter instrumenter) {

        File checkpointFile = new File(getFileAppendix() + "_checkpoint.dat");
        System.out.println((checkpointFile.exists() ? "Using existing" : "NEW") + " checkpointfile: " + checkpointFile.getName());

        Executor executor = new Executor()
                .withAlgorithm("NSGAII")
                .withSameProblemAs(instrumenter)
                //.withProperty("populationSize", 1)
                //.distributeOnAllCores()//ERR!!!
                //.withMaxTime(3*60*1000)
                //.withMaxEvaluations(20)
                .withCheckpointFile(checkpointFile)
                .checkpointEveryIteration()
                .withInstrumenter(instrumenter)
                .withProgressListener(new PopCodeLogger(checkpointFile.exists()))
                ;
        return executor.run();

    }

    private static void evalSaved() {
        String appendix = getFileAppendix();
        try {
            Population pop = PopulationIO.read(new File(appendix + "_Solutions.dat"));
            plot(null, pop);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private static void saveResults(Instrumenter instrumenter, Population solutions) throws IOException {
        String appendix = getFileAppendix();
        //Runtime data
        Accumulator accumulator = instrumenter.getLastAccumulator();
        accumulator.saveCSV(new File(appendix + "_accumulated.csv"));

        //Results
        PopulationIO.write(new File(appendix + "_Solutions.dat"), solutions);
        PopulationIO.writeObjectives(new File(appendix + "_Objectives.dat"), solutions);

        System.out.println("Saved " + solutions.size() + " solutions!");
    }


    private static void plot(Instrumenter instrumenter, Population solutions) {
        if (solutions != null) {
            new Plot()
                    .add("Pareto optimal Solutions", solutions)
                    .show();
        }

        if (instrumenter != null) {
            Accumulator acc = instrumenter.getLastAccumulator();
            if (acc != null) {
                new Plot().add(acc).show();
            } else {
                System.out.println("Accumulator null!");
            }
        }

    }


    private static void AnalyzerStuff(Instrumenter instrumenter, NondominatedPopulation solutions) {
        Analyzer analyzer = new Analyzer()
                .withSameProblemAs(instrumenter)
                .add("NSGAII", solutions)
                .includeHypervolume()
                .showStatisticalSignificance()
                .includeGenerationalDistance()
                .includeSpacing()
                .includeAllMetrics()
                .showStatisticalSignificance()
                //.showAll()
                ;
        new Plot()
                .add(analyzer)
                .show();
    }

    public static String getFileAppendix() {
        return PopCodeUtilities.gType + identifier;
    }
}
