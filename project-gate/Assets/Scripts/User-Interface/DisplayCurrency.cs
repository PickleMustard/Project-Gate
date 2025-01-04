using Godot;
using System;

public partial class DisplayCurrency : Label
{
    private int m_CurrentCurrency = 0;

    public override void _EnterTree()
    {
        this.Text = String.Format("Currency: 0$");
    }
    public void UpdateCurrentCurrency(int NewValue)
    {
        m_CurrentCurrency = NewValue;
        this.Text = String.Format("Currency: {0}$", m_CurrentCurrency);
    }

}
