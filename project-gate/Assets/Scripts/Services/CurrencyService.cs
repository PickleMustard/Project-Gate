using Godot;
using System;

/* Service that handles the storage, accumulation, and usage of currency for the player
 *
 */
public partial class CurrencyService : Node
{
  private int m_CurrentCurrency = 0;

  public void ResetCurrency()
  {
    m_CurrentCurrency = 0;
  }

  public void IncrementCurrency(int incrementorAmount)
  {
    m_CurrentCurrency += incrementorAmount;
  }

  public bool HasRequiredAmount(int amount)
  {
    return m_CurrentCurrency >= amount;
  }

  public void DecrementCurrency(int decrementorAmount)
  {
    m_CurrentCurrency -= decrementorAmount;
  }

}
