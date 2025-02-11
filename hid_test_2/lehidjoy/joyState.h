#ifndef JOYSTATE_H
#define JOYSTATE_H

#define DS5W_ISTATE_BTX_SQUARE 0x10
#define DS5W_ISTATE_BTX_CROSS 0x20
#define DS5W_ISTATE_BTX_CIRCLE 0x40
#define DS5W_ISTATE_BTX_TRIANGLE 0x80
#define DS5W_ISTATE_DPAD_LEFT 0x01
#define DS5W_ISTATE_DPAD_DOWN 0x02
#define DS5W_ISTATE_DPAD_RIGHT 0x04
#define DS5W_ISTATE_DPAD_UP 0x08

#define DS5W_ISTATE_BTN_A_LEFT_BUMPER 0x01
#define DS5W_ISTATE_BTN_A_RIGHT_BUMPER 0x02
#define DS5W_ISTATE_BTN_A_LEFT_TRIGGER 0x04
#define DS5W_ISTATE_BTN_A_RIGHT_TRIGGER 0x08
#define DS5W_ISTATE_BTN_A_SELECT 0x10
#define DS5W_ISTATE_BTN_A_MENU 0x20
#define DS5W_ISTATE_BTN_A_LEFT_STICK 0x40
#define DS5W_ISTATE_BTN_A_RIGHT_STICK 0x80

#define DS5W_ISTATE_BTN_B_PLAYSTATION_LOGO 0x01
#define DS5W_ISTATE_BTN_B_PAD_BUTTON 0x02
#define DS5W_ISTATE_BTN_B_MIC_BUTTON 0x04

#define DS5W_OSTATE_PLAYER_LED_LEFT 0x01
#define DS5W_OSTATE_PLAYER_LED_MIDDLE_LEFT 0x02
#define DS5W_OSTATE_PLAYER_LED_MIDDLE 0x04
#define DS5W_OSTATE_PLAYER_LED_MIDDLE_RIGHT 0x08
#define DS5W_OSTATE_PLAYER_LED_RIGHT 0x10


/* Representação para um stick analógico */
typedef struct _AnalogStick 
{
    char x;  /* Posição X do stick (0 = Centro) */
    char y;  /* Posição Y do stick (0 = Centro) */
} AnalogStick;

/* Representação de um vetor 3D (3 componentes) */
typedef struct _Vec3 
{
    short x;
    short y;
    short z;
} Vec3;

/* Representação de uma cor RGB */
typedef struct _Color 
{
    unsigned char r;  /* Componente Red */
    unsigned char g;  /* Componente Green */
    unsigned char b;  /* Componente Blue */
    unsigned char a; // adicionado 
} Color;

/* Representação do estado do touchpad */
typedef struct _Touch 
{
    unsigned int x;   /* Posição X do toque (~ 0 - 2000) */
    unsigned int y;   /* Posição Y do toque (~ 0 - 2048) */
    int down;         /* 1 se o toque está ativo, 0 caso contrário */
    unsigned char id; /* ID de 7 bits do toque */
} Touch;

/* Representação do estado da bateria */
typedef struct _Battery 
{
    int chargin;      /* Estado de carregamento da bateria (0 = Não carregando, 1 = Carregando) */
    int fullyCharged; /* Indica se a bateria está completamente carregada */
    unsigned char level;  /* Nível de carga da bateria (0x0 - 0xFF) */
} Battery;

    /// <summary>
    /// State of the mic led
    /// </summary>
typedef enum _MicLed
{
    /// <summary>
    /// Lef is off
    /// </summary>
    OFF = 0x00,

    /// <summary>
    /// Led is on
    /// </summary>
    ON = 0x01,

    /// <summary>
    /// Led is pulsing
    /// </summary>
    PULSE = 0x02,
} MicLed;

/* Códigos possíveis para o led do microfone */
#define MIC_LED_OFF  0x00
#define MIC_LED_ON   0x01
#define MIC_LED_PULSE  0x02

/* Tipos possíveis de efeito no gatilho */
#define NO_RESISTANCE        0x00
#define CONTINUOUS_RESISTANCE 0x01
#define SECTION_RESISTANCE   0x02
#define EFFECT_EX            0x26
#define CALIBRATE            0xFC

typedef enum  _TriggerEffectType
{
		/// <summary>
		/// No resistance is applied
		/// </summary>
		NoResitance = 0x00,

		/// <summary>
		/// Continuous Resitance is applied
		/// </summary>
		ContinuousResitance = 0x01,

		/// <summary>
		/// Seciton resistance is appleyed
		/// </summary>
		SectionResitance = 0x02,

		/// <summary>
		/// Extended trigger effect
		/// </summary>
		EffectEx = 0x26,

		/// <summary>
		/// Calibrate triggers
		/// </summary>
		Calibrate = 0xFC,
} TriggerEffectType;

/* Efeito de resistência aplicada ao gatilho */
typedef struct _TriggerEffect 
{
    unsigned char effectType;  /* Tipo de efeito no gatilho */

    /* Parâmetros do efeito */
    union 
    {
        unsigned char _u1_raw[6];  /* Dados brutos para o efeito */

        /* Para tipo CONTINUOUS_RESISTANCE */
        struct 
        {
            unsigned char startPosition;  /* Posição inicial da resistência */
            unsigned char force;          /* Força da resistência */
            unsigned char _pad[4];        /* Padding para alinhamento */
        } Continuous;

        /* Para tipo SECTION_RESISTANCE */
        struct 
        {
            unsigned char startPosition;  /* Posição inicial da resistência */
            unsigned char endPosition;    /* Posição final da resistência */
            unsigned char _pad[4];        /* Padding para alinhamento */
        } Section;

        /* Para tipo EFFECT_EX */
        struct 
        {
            unsigned char startPosition;  /* Posição inicial do efeito */
            int keepEffect;               /* 1 para manter o efeito */
            unsigned char beginForce;     /* Força quando o trigger >= (255 / 2) */
            unsigned char middleForce;    /* Força quando o trigger <= (255 / 2) */
            unsigned char endForce;       /* Força quando o trigger > 255 */
            unsigned char frequency;      /* Frequência de vibração do gatilho */
        } EffectEx;
    };
} TriggerEffect;

/* Níveis de brilho dos LEDs */
#define LED_BRIGHTNESS_LOW    0x02
#define LED_BRIGHTNESS_MEDIUM 0x01
#define LED_BRIGHTNESS_HIGH   0x00

/* Leds do jogador */
typedef struct _PlayerLeds 
{
    unsigned char bitmask; /* Máscara de bits dos LEDs de jogador */
    int playerLedFade;     /* Fade do LED do jogador (1 = fade, 0 = não) */
    unsigned char brightness;  /* Brilho dos LEDs */
} PlayerLeds;

/* Estado de entrada do controlador DS5 */
typedef struct _DS5InputState 
{
    AnalogStick leftStick;   /* Posição do stick analógico esquerdo */
    AnalogStick rightStick;  /* Posição do stick analógico direito */
    unsigned char leftTrigger;  /* Posição do gatilho esquerdo */
    unsigned char rightTrigger; /* Posição do gatilho direito */
    unsigned char buttonsAndDpad;  /* Máscara de botões e dpad */
    unsigned char buttonsA;   /* Máscara de botões A */
    unsigned char buttonsB;   /* Máscara de botões B */
    Vec3 accelerometer;  /* Acelerômetro (vetor 3D) */
    Vec3 gyroscope;      /* Giroscópio (vetor 3D) */
    Touch touchPoint1;    /* Primeiro ponto de toque */
    Touch touchPoint2;    /* Segundo ponto de toque */
    Battery battery;      /* Informações sobre a bateria */
    int headPhoneConnected;   /* 1 se o fone de ouvido está conectado */
    unsigned char leftTriggerFeedback;  /* Feedback do gatilho esquerdo */
    unsigned char rightTriggerFeedback; /* Feedback do gatilho direito */
} DS5InputState;

/* Estado de saída do controlador DS5 */
typedef struct _DS5OutputState 
{
    unsigned char leftRumble;  /* Motor de vibração esquerdo */
    unsigned char rightRumble; /* Motor de vibração direito */
    unsigned char microphoneLed;   /* Estado do LED do microfone */
    int disableLeds;         /* Desativa todos os LEDs (1 = desativado) */
    PlayerLeds playerLeds;   /* LEDs do jogador */
    Color lightbar;          /* Cor da barra de luz */
    TriggerEffect leftTriggerEffect;  /* Efeito do gatilho esquerdo */
    TriggerEffect rightTriggerEffect; /* Efeito do gatilho direito */
} DS5OutputState;






















#endif /* JOYSTATE_H */
