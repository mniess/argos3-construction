package de.uniluebeck.iti.PopCode.MOEA;

import org.moeaframework.core.Solution;
import org.moeaframework.core.variable.BinaryIntegerVariable;
import org.moeaframework.core.variable.EncodingUtils;
import org.moeaframework.problem.AbstractProblem;

import java.util.Arrays;
import java.util.concurrent.atomic.AtomicInteger;

public class NSGA2PopCode extends AbstractProblem {

    private PopCodeLogger logger;
    private Settings settings;

    static {
        System.loadLibrary("nsga2_construction"); // Load native library at runtime
    }


    public NSGA2PopCode(Settings settings, PopCodeLogger logger) {
        super(PopCodeUtilities.GAgenomeSize, 2);
        this.settings = settings;
        this.logger = logger;
        InitArgos();
    }

    public native double LaunchArgos(int[] genome, int evaluations, String genomeType, int seed);

    public native int InitArgos();

    public native int DestroyArgos();

    private AtomicInteger run = new AtomicInteger(0);

    public void evaluate(Solution solution) {
        try {
            int[] genome = PopCodeUtilities.getGenome(solution);
            double fitness = -1;
            try {
                fitness = LaunchArgos(genome, settings.evaluations, settings.gType.toString(), run.get());
            } catch (Exception e) {
                logger.err("Error with getting results. Writing 1 as Fitness...");
            }

            double sparsity = PopCodeUtilities.hammingSparsity(genome);
            solution.setObjective(0, -fitness);
            solution.setObjective(1, -sparsity);
            System.out.printf("%d: fitness=%.2f, sparsity=%.2f, genome=%s%n", run.incrementAndGet(), fitness, sparsity, Arrays.toString(genome));
        } catch (Exception e) {
            logger.err(e.getMessage());
        }
    }

    public Solution newSolution() {
        Solution solution = new Solution(PopCodeUtilities.GAgenomeSize, 2);
        int counter = 0;
        switch (settings.gType) {
            case SIMPLE:
                addSimpleGenome(solution, counter);
                break;
            case SIMPLECOUNT:
                counter = addSimpleGenome(solution, counter);
                addCounts(solution, counter);
                break;
            case FULLCOUNT:
                counter = addFullGenome(solution, counter);
                addCounts(solution, counter);
                break;
        }

        return solution;
    }

    private int addFullGenome(Solution solution, int counter) {
        for (int i = 0; i < settings.numRobots; i++) { //each robot
            for (int j = 0; j < 3; j++) { // each state
                solution.setVariable(counter++, EncodingUtils.newBinaryInt(0, 4)); //Time
                solution.setVariable(counter++, EncodingUtils.newBinaryInt(-1, 1)); //Cylinder in Seen
                solution.setVariable(counter++, EncodingUtils.newBinaryInt(0, 4)); //LowerBound Light
                solution.setVariable(counter++, EncodingUtils.newBinaryInt(0, 4)); //UpperBound Light
                solution.setVariable(counter++, EncodingUtils.newBinaryInt(0, 1)); //Drop/Pickup
            }
        }
        return counter;
    }

    private int addCounts(Solution solution, int counter) {
        for (int i = 0; i < settings.numRobots; i++) {
            solution.setVariable(counter + i, new BinaryIntegerVariable(0, settings.numRobots - 1));
        }
        return counter;
    }

    private int addSimpleGenome(Solution solution, int counter) {
        for (int i = 0; i < settings.numRobots; i++) {
            //AntiPhototaxis -> Explore
            solution.setVariable(counter, EncodingUtils.newBinaryInt(0, 4)); //Time
            solution.setVariable(counter + 1, EncodingUtils.newBinaryInt(0, 1)); //Drop/Pickup
            //Explore -> Phototaxis
            solution.setVariable(counter + 2, EncodingUtils.newBinaryInt(0, 4)); //Time
            solution.setVariable(counter + 3, EncodingUtils.newBinaryInt(-1, 1)); //Cylinder in Seen
            solution.setVariable(counter + 4, EncodingUtils.newBinaryInt(0, 1)); //Drop/Pickup
            //Phototaxis->Antiphototaxis
            solution.setVariable(counter + 5, EncodingUtils.newBinaryInt(0, 4)); //LowerBound Light
            solution.setVariable(counter + 6, EncodingUtils.newBinaryInt(0, 4)); //UpperBound Light
            solution.setVariable(counter + 7, EncodingUtils.newBinaryInt(0, 1)); //Drop/Pickup
            counter += PopCodeUtilities.robGenomeSize;
        }
        return counter;
    }

    private void TestJNI() {
        InitArgos();
//        LaunchArgos();
//        LaunchArgos();
        DestroyArgos();
    }

    @Override
    protected void finalize() throws Throwable {
        DestroyArgos();
        super.finalize();
    }
}
