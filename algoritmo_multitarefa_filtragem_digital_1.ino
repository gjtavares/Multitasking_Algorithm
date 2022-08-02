/*
Algoritimo Multitarefa com filtro digital de 1° ordem.
Gabriel Jeronimo
2020
*/

/* Definções de constantes */
#define TRUE 1
#define FALSE 0
#define SET 1
#define RESET 0
#define Amostragem_COUNTER_MAX 30
#define Reamostragem_COUNTER_MAX 35
#define Canal_Serial_COUNTER_MAX 100

// Definições de variáveis globais 
unsigned int Amostragem_counter=0;
unsigned int Reamostragem_counter=0;
unsigned int Canal_Serial_counter=0;
unsigned char Amostragem_enable=TRUE;
unsigned char Reamostragem_enable=TRUE;
unsigned char Canal_Serial_enable=TRUE;
unsigned char Canal_Serial_go=FALSE;

float ym, ym_filt;

/* Protótipos de funções auxiliares */
void setup_multitasking(void);
float yMed(void);
float filtro(float);
float filtro1ordem(float);

 
/* Rotina de configuração inicial */
void setup()
{
  Serial.begin(9600);
  setup_multitasking();
}

/* Rotina principal - loop infinito */
void loop()
{
  
  if (Canal_Serial_go==TRUE) 
  {
     Canal_Serial_go=FALSE;
     Serial.print(ym);
     Serial.print(',');
     Serial.println(ym_filt);    
  }
}

float yMed(void)
{  
  float randNum,ruido;
  randNum=random(-100,100)/100.0;
  ruido=1+5*randNum/100; // 5% de ruído (-0.95 a +1.05)
  return(ruido*analogRead(0));
}  

float filtro1ordem(float yin)
{  
  static float Saida_Filtro = 0;
  static float yin_anterior = 0;
  
  Saida_Filtro = (float)((Saida_Filtro + 0.3333*yin + 0.3333*yin_anterior)/1.66667) ;
  yin_anterior = yin;
  return((float)Saida_Filtro);
 }
/* Configura o sistema de interrupção por timer */
void setup_multitasking(void)
{
   TCCR1A = 0x00;       // 0000 0000 CLOCK 16MHz
   TCCR1B = 0x0b;       // 0000 1011 PRESCALER 64
   OCR1A =  250;        // Tint 250*64*1/16000000 = 1ms                       
   TIMSK1 = 0x02;       // habilita específica do timer
   sei();               // habilitação geral de interrupções 
}

/* Rotina de tratamento de interrupção */
ISR(TIMER1_COMPA_vect) // interrompe a cada 1ms 
{
   /* Controla as tarrefas */
   Amostragem_counter++;
   if(Amostragem_counter>=Amostragem_COUNTER_MAX)
   {
      Amostragem_counter=0;
      if(Amostragem_enable==TRUE)
      {
          ym=yMed();
          
      }
   }
  
   Reamostragem_counter++;
   if(Reamostragem_counter>=Reamostragem_COUNTER_MAX)
   {
      Reamostragem_counter=0;
      if(Reamostragem_enable==TRUE)
      {
         ym_filt = filtro1ordem(ym); 
      }
   }

   Canal_Serial_counter++;
   if(Canal_Serial_counter>=Canal_Serial_COUNTER_MAX)
   {
      Canal_Serial_counter=0;
      if(Canal_Serial_enable==TRUE)
      {
         Canal_Serial_go=TRUE; // Habilita Canal_Serial para
                        // rodar no 'main loop'
      }
   } 
}   
