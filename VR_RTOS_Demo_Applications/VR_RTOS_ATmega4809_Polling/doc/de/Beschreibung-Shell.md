
# Beschreibung der Shell (ATMega 4809)

## Allgemeines

Die Shell ist eine Task mit deren Hilfe auf die HAL (Hardware Abstraction Layer) 
und auf gewisse Funktionen des Schedulers von VR-RTOS zugegriffen werden kann.

Beispielsweise können alle digitalen und analogen IO gelesen und verändert werden.
Die Shell kommuniziert über die RS232-Task mit dem Benutzer.
Schnittstellenparameter sind : 38400, 8, N, 1

### Modi

Es stehen verschiedene Modi zur Verfügung.
1. Normalmodus
2. Produktionsmodus (nicht implementiert)
3. Testmodus
4. Manueller Modus
5. Expert-Modus
6. EMV-Modus (nicht implementiert)

##### Normalmodus
In diesem Modus wird die Shell gestartet. Es stehen nur wenige Befehle zur Verfügung.
Nur von diesem Modus aus kann in einen anderen Modus gewechselt werden. Ist man in einem 
anderen Modus muss in diesen Modus gewechselt werden um in einen anderen Modus zu wechseln.
Befehle um in einen anderen Modus zu wechseln :
- facton
- teston
- manuon
- expton

um wieder in den Normalmodus zu wechseln :
- factoff
- testoff
- manuoff
- exptoff

Alle Befehle im Normalmodus stehen auch in allen anderen Modi zur Verfügung.
- help  - listet alle Befehle auf, die in dem aktiven Modus unterstützt werden
- sys   - zeigt verschiedene Parameter des Betriebssystems an
- ver   - zeigt verschiedene Versions-Daten an

##### Manueller Modus
In diesem Modus Werden alle Ausgänge (analog und digital) ausschließlich von der Shell beeinflusst.
Das normale Programm kann keine Ausgänge beeinflussen.
Dies ist der typische Modus für Inbetriebnahme und Reparatur.

##### Testmodus
Dieser Modus ist das Gegenteil vom manuellen Modus
Alle Ausgänge (analog und digital) werden vom normalen Programm beeinflusst.
Die Shell hat keinen Einfluss auf die Ausgänge.
Die Shell hat ausschließlich Einfluss auf die Eingänge, das normale Programm kann 
keine Eingänge lesen.

##### Expertmodus
In diesem Modus läuft das normale Programm weiter, der Benutzer kann aber gewisse
Eingriffe vornehmen.

##### Produktionsmodus
Ist im Moment nicht implementiert.
Vorgesehen für speichern von Produktionsdaten oder Kalibrieren in Fertigung etc.

##### EMV-Modus
Ist im Moment nicht implementiert.
Vorgesehen für spezielle Eigenschaften des Programms während EMV-Messungen, 
z.B. verkürzte Zykluszeit etc.

### Auflistung aller Shell-Befehle AnaIn
| Befehl   | Modus                  | Erklärung                              |
|----------|------------------------|----------------------------------------|
| ai       | Expert, Manuell, Test  | Misst alle Spannungen u. Ströme und zeigt sie an |
| ais      | Test                   | Setzt einen AanIn-Wert |
| repai    | Expert, Manuell       |  Zeigt alle AnaIn Werte ZYKLISCH an |

### Auflistung aller Shell-Befehle DigIn
| Befehl   | Modus       | Erklärung                              |
|----------|-------------|----------------------------------------|
| di       | Expert, Manuell, Test | Liest alle DigIn und zeigt sie an |
| dir      | Test                  | Setzt einen DigIn-Wert auf 0 |
| dis      | Test                  | Setzt einen DigIn-Wert auf 1 |
| dit      | Test                  | Toggelt einen DigIn-Wert |
| repdi    | Expert, Manuell       | Liest alle DigIn und zeigt sie ZYKLISCH an |

### Auflistung aller Shell-Befehle DigOut
| Befehl   | Modus       | Erklärung                              |
|----------|-------------|----------------------------------------|
| do       | Expert, Manuell, Test | Liest alle DigOut und zeigt sie an |
| dor      | Expert, Manuell       | Setzt einen DigOut-Wert auf 0 |
| dos      | Expert, Manuell       | Setzt einen DigOut-Wert auf 1 |
| dot      | Expert, Manuell       | Toggelt einen DigOut-Wert |
| repdo    | Expert                | Liest alle DigOut und zeigt sie ZYKLISCH an |

### Auflistung aller Shell-Befehle PwmOut 
| Befehl   | Modus       | Erklärung                              |
|----------|-------------|----------------------------------------|
| po       | Manuell     | Liest alle PwmOut und zeigt sie an     |
| pos      | Manuell     | setzt PWM-Kanal auf angegebenen Wert   |

### Auflistung aller sonstigen Shell-Befehle
| Befehl   | Modus       | Erklärung                              |
|----------|-------------|----------------------------------------|
| help      | Alle | listet alle Befehle auf die in dem aktuellen Modus zur Verfügung stehen |
| repall    | Expert, Manuell       | Liest alle AnaIn, DigIn, Digout und PwmOut und zeigt sie ZYKLISCH an |
| rst       | Expert | Führt einen Neustart der MCU aus |
| start     | Expert | Befel zum starten einer Task |
| stop      | Expert | Befel zum stoppen (=beenden) einer Task |
| sys       | Alle | zeigt verschiedene Parameter des Betriebssystems an |
| trace     | Expert | Befel zum starten und stoppen von Trace-Ausgaben |
| ver       | Alle | zeigt verschiedene Versions-Daten an |
| wdog      | Expert | Befel zum blockieren des Programms. Führt zum auslösen des Watchdog |
| x         | Alle | Beendet einen Rep(eat)-Befehl (PuTTY akzeptiert auch ESC) |
| ENTER     | Alle | Wiederholt den letzten Befehl |
