
## Beschreibung FW VR_RTOS_ATmega4809_Polling Demo Projekt
Datum: 22. Nov 2025

### 1 Allgemein
Dieses Projekt verwendet das VR-RTOS-Polling-Betriebssystem für ATMega 4809 Polling 1.0.1.
Das Projekt wurde auf dem Board „ATMega4809 Xplained Pro“ entwickelt.
Für die Firmware-Entwicklung wurde Microchip Studio 7.0.2595 verwendet.
Alle Dateien für die Demo-Applikation befinden sich im Ordner Application.


### 2 Board-Driver-Task
Die Treiber-Task für die Platine befindet sich in der Datei Task_BoardDrv.c.
- Die Benutzertaste wird in der Funktion process_Switch_Inputs() ausgewertet. Beim Drücken wird die Nachricht SIG_PLUS_LEDPROG an die LED-Task gesendet.
- Die analogen Eingänge werden in der Funktion process_AnaIns() eingelesen.
- Die PWM-Ausgangskanäle werden in der Funktion process_PwmOuts() ausgegeben.
- Ein Betriebsstundenzähler ist in der Funktion inc_VR_RTOS_WorkingTime() implementiert.


###3 Delay-Task
Die Delay-Task befindet sich in der Datei Task_Delay.c.
Diese Task führt bei jedem Tick eine Verzögerung von 1 ms aus.


###4 Error-Manager-Task
Die Error-Manager-Task befindet sich in der Datei Task_ErrorManager.c.
Diese Aufgabe empfängt und speichert Fehlermeldungen Error-Messages.


###5 Led-Task
Die Led-Task befindet sich in der Datei Task_Led.c.
Diese Task lässt die Benutzer-LED blinken.
- Beim Empfang der Nachricht SIG_PLUS_LEDPROG wird die Blinkdauer um 100 ms erhöht. Sobald sie 1000 ms erreicht, wird sie auf 100 ms zurückgesetzt.
- Beim Empfang der Nachricht SIG_MINUS_LEDPROG wird die Blinkdauer um 100 ms verringert. Sobald sie 0 ms erreicht, wird sie auf 1000 ms zurückgesetzt.
Um die LED blinken zu lassen, wird die Nachricht SIG_LED_TOGGLE zyklisch an sich selbst gesendet. Die Blinkdauer wird in der Variablen s_Blink_Time gespeichert.


###6 Null-Task
Die Null-Task befindet sich in der Datei Task_NULL.c.
In dieser Task werden die Zeiten für die einzelnen Tasks berechnet.


###7 RS232-Task
Die RS232-Task befindet sich in der Datei Task_RS232.c.
Diese Aufgabe gehört zum Scheduler-Task.
- Sie empfängt Daten über die UART und leitet sie an die Shell weiter..
- Sie liest Daten aus dem FIFO-Speicher RS232_Send_FiFo und sendet sie an die UART.
- Ausnahme: Werden die Befehle „L+“ oder „L-“ über die UART empfangen, werden die entsprechenden Meldungen SIG_PLUS_LEDPROG bzw. SIG_MINUS_LEDPROG direkt an den LED-Task gesendet.


###8 Shell-Task
Die Shell-Task befindet sich in der Datei Task_Shell.c.
Siehe Beschreibung_Shell.md.


###9 Startup-Task
Die Startup-Task befindet sich in der Datei Task_Startup.c.
Alle Tasks werden in dieser Datei nacheinander gestartet. Nachdem alle Tasks gestartet wurden, beendet sich die Startup-Task selbst.

