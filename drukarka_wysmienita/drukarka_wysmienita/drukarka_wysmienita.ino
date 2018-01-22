#define X_MIN_PIN           3
#define X_MAX_PIN           2
#define Y_MIN_PIN          14
#define Y_MAX_PIN          15
#define Z_MIN_PIN          18
#define Z_MAX_PIN          19
#define X_STEP_PIN         54
#define X_DIR_PIN          55
#define X_ENABLE_PIN       38
#define Y_STEP_PIN         60
#define Y_DIR_PIN          61
#define Y_ENABLE_PIN       56
#define Z_STEP_PIN         46
#define Z_DIR_PIN          48
#define Z_ENABLE_PIN       62

void setup() 
{
    // Ustawienie pinów jako wejscia/wyjscia
    pinMode(X_MIN_PIN,INPUT);
    digitalWrite(X_MIN_PIN,HIGH);
    pinMode(X_MAX_PIN,INPUT);
    digitalWrite(X_MAX_PIN,HIGH);
    pinMode(Y_MIN_PIN,INPUT);
    digitalWrite(Y_MIN_PIN,HIGH);
    pinMode(Y_MAX_PIN,INPUT);
    digitalWrite(Y_MAX_PIN,HIGH);
    pinMode(Z_MIN_PIN,INPUT);
    digitalWrite(Z_MIN_PIN,HIGH);
    pinMode(Z_MAX_PIN,INPUT);
    digitalWrite(Z_MAX_PIN,HIGH);
    
    pinMode(X_STEP_PIN,OUTPUT); 
    pinMode(X_DIR_PIN,OUTPUT);
    pinMode(X_ENABLE_PIN,OUTPUT);
    digitalWrite(X_ENABLE_PIN,LOW);
  
    pinMode(Y_STEP_PIN,OUTPUT); 
    pinMode(Y_DIR_PIN,OUTPUT);
    pinMode(Y_ENABLE_PIN,OUTPUT);
    digitalWrite(Y_ENABLE_PIN,LOW);
  
    pinMode(Z_STEP_PIN,OUTPUT); 
    pinMode(Z_DIR_PIN,OUTPUT);
    pinMode(Z_ENABLE_PIN,OUTPUT);
    digitalWrite(Z_ENABLE_PIN,LOW);

    Serial.begin(115200); // Rozpoczęcie komunikacji z komputerem
}

// Zmienne globalne
int xmin, xmax, ymin, ymax, zmin, zmax; // Krańcówki
int x = 0, y = 0, z = 0; // Liczba kroków poszczególnych osi
const byte readBufferSize = 100; // Rozmiar odbieranej ramki danych
char readBuffer[readBufferSize]; // Dane odbierane z Matlaba w postaci <x,y,z>

boolean readData(); // Prototyp funkcji do odbierania danych z Matlaba

void loop() 
{
    // Odczytanie stanu krańcówek
    xmin = digitalRead(X_MIN_PIN);
    xmax = digitalRead(X_MAX_PIN);
    ymin = digitalRead(Y_MIN_PIN);
    ymax = digitalRead(Y_MAX_PIN);
    zmin = digitalRead(Z_MIN_PIN);
    zmax = digitalRead(Z_MAX_PIN);

    if(readData()) // Jeżeli odebrano dane
    {
        Serial.println(0); // Wysłanie 0 oznacza, że drukarka jest w trakcie jazdy.
        
        // Ustawienie kierunku (góra/dół)
        if(x > 0)
        {
            digitalWrite(X_DIR_PIN,LOW);
        }
        else
        {
            digitalWrite(X_DIR_PIN,HIGH);
            x = -x;
        }

        if(y > 0)
        {
            digitalWrite(Y_DIR_PIN,LOW);
        }
        else
        {
            digitalWrite(Y_DIR_PIN,HIGH);
            y = -y;
        }
      
        if(z > 0)
        {
            digitalWrite(Z_DIR_PIN,LOW);
        }
        else
        {
            digitalWrite(Z_DIR_PIN,HIGH);
            z = -z;
        }

        while(x > 0 or y > 0 or z > 0) // Jedź zadaną ilość kroków w każdej osi
        {
            if(xmax == 0 and x > 0)
            {
                digitalWrite(X_STEP_PIN,HIGH);
                x--;
            }
      
            if(ymax == 0 and y > 0)
            {
                digitalWrite(Y_STEP_PIN,HIGH);
                y--;
            }
      
            if(zmax == 0 and z > 0)
            {
                digitalWrite(Z_STEP_PIN,HIGH);
                z--;
            }
      
            delayMicroseconds(100);
            digitalWrite(X_STEP_PIN,LOW);
            digitalWrite(Y_STEP_PIN,LOW);
            digitalWrite(Z_STEP_PIN,LOW);
            delayMicroseconds(500);
        }

        Serial.println(1); // Wysłanie 1 oznacza, że drukarka dojechała na pozycję.
    }
    else
    {
        Serial.println(1);
    }
}

boolean readData() // Funkcja odbierająca dane z Matlaba. Zwraca wartość TRUE po odebraniu nowych danych.
{
    static boolean dataReadInProgress = false; // Flaga, informująca czy są aktualnie odczytywane dane (przyjmuje wartość TRUE, kiedy aktualnie odczytywany znak jest pomiędzy < >).
    static byte readBufferIndex = 0; // Zmienna do indeksowania bufora.
    const char startChar = '<'; // Znak początku ramki danych.
    const char endChar = '>'; // Znak końca ramki danych.
    char readChar; // Odczytany znak.

    char * dataPart; // Przechowuje wycięty fragment z oryginalnego bufora.
    int dataParts[3]; // Tablica do przechowywania wszystkich podzielonych fragmentów.
    int dataPartsIndex = 0; // Indeks tablicy.

    while(Serial.available() > 0) // Jeżeli są dostępne dane do odebrania
    {
        readChar = Serial.read(); // Odczytaj jeden znak.
        
        if(dataReadInProgress == true)
        {
            if(readChar == endChar) // Jeżeli odczytany znak jest znakiem końca ramki danych
            {
                readBuffer[readBufferIndex] = '\0'; // Wpisz 0 na ostatnią pozycję tablicy.
                readBufferIndex = 0; // Wyzeruj indeks bufora.
                dataReadInProgress = false; // Skasuj flagę.

                dataPart = strtok(readBuffer, ","); // Dzieli oryginalne dane na części. Fragmenty oddzielone są separatorem (przecinkiem).

                while(dataPart != NULL) // Pętla wykonuje się do momentu, aż strtok zwróci NULL (czyli nie będzie już więcej danych oddzielonych przecinkiem).
                {
                    dataParts[dataPartsIndex] = atoi(dataPart); // Przekonwertuj dane z ASCII na int i wpisz do tablicy.
                    dataPartsIndex++; // Zwiększ indeks o jeden.
                    dataPart = strtok(NULL, ","); // Szukaj kolejnego fragmentu.
                }
            
                // Przypisanie wartości z tablicy odpowiadającym im zmiennym.
                x = dataParts[0];
                y = dataParts[1];
                z = dataParts[2];

                return true; // Zwróć TRUE (odebrano wszystkie dane).
            }
            else
            {
                if(readBufferIndex >= readBufferSize) // Zabezpieczenie przed przepełnieniem bufora (znaki, które nie zmieszczą się w buforze są tracone).
                {
                    readBufferIndex = readBufferSize - 1;
                }
                else
                {
                    readBuffer[readBufferIndex] = readChar; // Jeżeli odczytano znak pomiędzy < >, wpisz go do bufora.
                    readBufferIndex++; // Zwiększ indeks bufora.
                }
            }
        }
        else if(readChar == startChar) // Jeżeli odczytany znak jest znakiem początku ramki danych
        {
            dataReadInProgress = true; // Ustaw flagę.
        }
    }

    return false; // Jeżeli nie ma nic do odebrania, zwróć FALSE.
}

