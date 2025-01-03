using Godot;

/* Service that handles the storage, accumulation, and usage of currency for the player
 *
 */

public partial class CurrencyService : Node
{
    public static CurrencyService Instance { get; private set; }

    private int m_CurrentCurrency = 0;
    private Callable m_IncrementCurrencyCallableFunction;
    private Callable m_DecrementCurrencyCallableFunction;
    private DisplayCurrency m_CurrencyTextbox;

    public override void _EnterTree()
    {
        Instance = this;
        Engine.RegisterSingleton("CurrencyService", this);
        m_IncrementCurrencyCallableFunction = new Callable(this, "IncrementCurrency");
        m_DecrementCurrencyCallableFunction = new Callable(this, "DecrementCurrency");
    }

    public override void _Ready()
    {
        m_CurrencyTextbox = GetTree().GetNodesInGroup("CurrencyDisplay")[0] as DisplayCurrency;
    }

    public Callable GetIncrementCurrencyCallable()
    {
        return m_IncrementCurrencyCallableFunction;
    }

    public Callable GetDecrementCurrencyCallable()
    {
        return m_DecrementCurrencyCallableFunction;
    }

    public void ResetCurrency()
    {
        m_CurrentCurrency = 0;
        m_CurrencyTextbox.UpdateCurrentCurrency(m_CurrentCurrency);
    }

    public int GetCurrentCurrencyAmount()
    {
        return m_CurrentCurrency;
    }

    private void IncrementCurrency(int incrementorAmount)
    {
        m_CurrentCurrency += incrementorAmount;
        m_CurrencyTextbox.UpdateCurrentCurrency(m_CurrentCurrency);
    }

    public bool HasRequiredAmount(int amount)
    {
        return m_CurrentCurrency >= amount;
    }

    private void DecrementCurrency(int decrementorAmount)
    {
        m_CurrentCurrency -= decrementorAmount;
        m_CurrencyTextbox.UpdateCurrentCurrency(m_CurrentCurrency);
    }

}
