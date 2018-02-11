package de.uniluebeck.iti.PopCode.MOEA;

import org.moeaframework.core.Solution;
import org.moeaframework.core.variable.EncodingUtils;
import org.moeaframework.problem.AbstractProblem;


public class NSGA2PopCode extends AbstractProblem {

    static {
        System.loadLibrary("nsga2_construction"); // Load native library at runtime
    }

    public NSGA2PopCode() {
        super(1, 2);
    }

    public native  double LaunchArgos();

    @Override
    public void evaluate(Solution solution) {
        double x = EncodingUtils.getReal(solution.getVariable(0));
        solution.setObjective(0, Math.pow(x, 2.0));
        solution.setObjective(1, Math.pow(x - 2.0, 2.0));
        System.out.println(LaunchArgos());
    }

    @Override
    public Solution newSolution() {
        Solution solution = new Solution(1, 2);
        solution.setVariable(0, EncodingUtils.newReal(-10.0, 10.0));
        return solution;
    }
}
