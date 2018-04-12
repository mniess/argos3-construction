package de.uniluebeck.iti.PopCode.MOEA;

/*
 * The GENOME Type
 * Simple(count) Genome describes a manually created subset of full Genome
 * Full(Count) Genome is has variables for all possible transitions in Robot FSM
 * Count indicates that the robots are selected by extra variables
 */
public enum GENOME {
    SIMPLE("simple"), SIMPLECOUNT("simpleCount"), FULLCOUNT("fullCount");

    private final String desc;

    GENOME(final String text) {
        this.desc = text;
    }

    @Override
    public String toString() {
        return desc;
    }
}
