using Godot;
using System;

public partial class SeededRandomAccess : Node
{
    public static SeededRandomAccess staticSeededRandomAccess;

    //Used seeds:
    //155675 (20:30 => 29 | 40:50 => 49)
    //134576937
    private Random _rnd = new Random(134576937);

    public override void _Ready() {
        staticSeededRandomAccess = this;
    }

    public int getRandomWholeNumber(int limit) {
        return _rnd.Next(limit);
    }

    public int getRandomInteger(int start, int limit) {
        return _rnd.Next(start, limit);
    }
}
