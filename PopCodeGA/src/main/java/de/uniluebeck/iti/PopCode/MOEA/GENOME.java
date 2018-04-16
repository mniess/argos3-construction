package de.uniluebeck.iti.PopCode.MOEA;

/*
 * The GENOME Type
 * Simple(count) Genome describes a manually created subset of full Genome
 * Full(Count) Genome is has variables for all possible transitions in Robot FSM
 * Count indicates that the robots are selected by extra variables
 */
public enum GENOME {
    SIMPLE("simple", false),
    SIMPLECOUNT("simpleCount", true),
    FULLCOUNT("fullCount", true),
    FULL("full", false);

    private final String desc;
    protected final boolean hasCount;

    GENOME(final String text, final boolean hasCount) {
        this.desc = text;
        this.hasCount = hasCount;
    }

    @Override
    public String toString() {
        return desc;
    }
}
