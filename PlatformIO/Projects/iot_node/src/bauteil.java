package bauteile.data;

public class Bauteil {
  // Datenelemente
  private final int anzahl;
  private final Bauteiltyp typ;
  private final double wert;

  public Bauteil(int anzahl, Bauteiltyp bauteiltyp, double wert) 
    throws Exception
  {
    this.anzahl = anzahl;
    this.typ = bauteiltyp;
    this.wert = wert;
    
    if(anzahl < 0)
      throw new Exception("Anzahl darf nicht kleiner 0 sein");
  }

  public int getAnzahl() {
    return anzahl;
  }

  public Bauteiltyp getBauteiltyp() {
    return typ;
  }

  public double getWert() {
    return wert;
  }  
}
