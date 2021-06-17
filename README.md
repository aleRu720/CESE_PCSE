# CESE_PCSE
Protocolos de comunicación en Sistemas Embebidos
El sistema cuenta con 3 MEF:
                        comSerial.C - MEF de comunicación serie: Decodifica los mensajes entrantes.
                        main.c      - MEF principal.
                                        - Espera datos por USB(Puerto Serie Virtual).
                                        - Configura el ESP con los datos de conexión al wifi y al servidor.
                                        - Ejecuta las rutinas del Microcontrolador.
                        wifi.c      - MEF esp8266: Configura el módulo, conecta al wifi, conecta al servidor, 
                                                   pone el módulo en transparente (Passthrough).
                                            
