package de.uniluebeck.iti.PopCode.MOEA;

import org.moeaframework.core.Solution;
import org.moeaframework.core.Variable;
import org.moeaframework.core.variable.EncodingUtils;
import org.moeaframework.problem.AbstractProblem;

import java.util.concurrent.atomic.AtomicInteger;


public class NSGA2PopCode extends AbstractProblem {

    static {
        System.loadLibrary("nsga2_construction"); // Load native library at runtime
    }

    public NSGA2PopCode() {
        super(80, 1);
        //InitArgos();
    }

    public native double LaunchArgos(int[] genome);

    public native int InitArgos();

    public native int DestroyArgos();

    private AtomicInteger run = new AtomicInteger(1);

    public void evaluate(Solution solution) {
        int[] a = variablesToArray(solution);
        double fitness = LaunchArgos(a);
        solution.setObjective(0, fitness);
        System.out.println("Eval" + run.incrementAndGet());
    }

    private int[] variablesToArray(Solution solution) {
        int[] array = new int[solution.getNumberOfVariables()];
        for (int i = 0; i < solution.getNumberOfVariables(); i++) {
            try {
                array[i] = EncodingUtils.getInt(solution.getVariable(i));
            } catch (Exception e) {
                System.err.println("Encoding Error!");
                e.printStackTrace();
            }

        }
        return array;
    }

    public Solution newSolution() {
        int numRobots = 10;
        Solution solution = new Solution(numRobots * 8, 1);
        int counter = 0;
        for (int i = 0; i < numRobots; i++) {
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
            counter += 8;
        }
        return solution;
    }

    private void TestJNI() {
        InitArgos();
//        LaunchArgos();
//        LaunchArgos();
        DestroyArgos();
    }

    @Override
    protected void finalize() throws Throwable {
        //DestroyArgos();
        super.finalize();
    }
}
