#ifndef JOYSTATE_H
#define JOYSTATE_H

/*
    DS5State.h is part of DualSenseWindows
    https://github.com/Ohjurot/DualSense-Windows

    Contributors of this file:
    11.2020 Ludwig Füchsl

    Licensed under the MIT License (To be found in repository root directory)

    MODC89: PHIKILL
*/

#ifdef __WATCOMC__
    #include<stdbool.h>;
#elif __GNUC__
    #include<stdbool.h>
#elif _MSC_VER
    #include<stdbool.h>
#else /* Compilers dont have stdbool */
    unsigned char bool;
    unsigned char BOOL;
    #define TRUE 1
    #define true 1
    #define FALSE 0
    #define false 0
#endif

#define DS5W_ISTATE_BTX_SQUARE   0x10
#define DS5W_ISTATE_BTX_CROSS    0x20
#define DS5W_ISTATE_BTX_CIRCLE   0x40
#define DS5W_ISTATE_BTX_TRIANGLE 0x80
#define DS5W_ISTATE_DPAD_LEFT    0x01
#define DS5W_ISTATE_DPAD_DOWN    0x02
#define DS5W_ISTATE_DPAD_RIGHT   0x04
#define DS5W_ISTATE_DPAD_UP      0x08

#define DS5W_ISTATE_BTN_A_LEFT_BUMPER   0x01
#define DS5W_ISTATE_BTN_A_RIGHT_BUMPER  0x02
#define DS5W_ISTATE_BTN_A_LEFT_TRIGGER  0x04
#define DS5W_ISTATE_BTN_A_RIGHT_TRIGGER 0x08
#define DS5W_ISTATE_BTN_A_SELECT        0x10
#define DS5W_ISTATE_BTN_A_MENU          0x20
#define DS5W_ISTATE_BTN_A_LEFT_STICK    0x40
#define DS5W_ISTATE_BTN_A_RIGHT_STICK   0x80

#define DS5W_ISTATE_BTN_B_PLAYSTATION_LOGO  0x01
#define DS5W_ISTATE_BTN_B_PAD_BUTTON        0x02
#define DS5W_ISTATE_BTN_B_MIC_BUTTON        0x04

#define DS5W_OSTATE_PLAYER_LED_LEFT         0x01
#define DS5W_OSTATE_PLAYER_LED_MIDDLE_LEFT  0x02 /* Middle Player 2 */
#define DS5W_OSTATE_PLAYER_LED_MIDDLE       0x04 /* Middle Player 1 */
#define DS5W_OSTATE_PLAYER_LED_MIDDLE_RIGHT 0x08
#define DS5W_OSTATE_PLAYER_LED_RIGHT        0x10

#define HIDJOY_LED_MIDDLE_THREE     0x06
#define HIDJOY_LED_ALL              0x0F /* All Leds */
#define HIDJOY_LED_PLAYER_FOUR_02   0x09

#define HIDJOY_LED_PLAYER_ONE       0x04
#define HIDJOY_LED_PLAYER_TWO       0x02
#define HIDJOY_LED_PLAYER_THREE     0x05
#define HIDJOY_LED_PLAYER_FOUR      0x03
#define HIDJOY_LED_PLAYER_FIVE      0x07

#define HIDJOY_LED_PLAYER_1 0x04
#define HIDJOY_LED_PLAYER_2 0x02
#define HIDJOY_LED_PLAYER_3 0x05
#define HIDJOY_LED_PLAYER_4 0x03
#define HIDJOY_LED_PLAYER_5 0x07

/* Representation for an analog stick */
typedef struct _AnalogStick 
{
    char x;  /* X position of the stick (0 = Center) */
    char y;  /* Y position of the stick (0 = Center) */
} AnalogStick;

/* Representation of a 3D vector (3 components) */
typedef struct _Vec3 
{
    short x;
    short y;
    short z;
} Vector3, Vec3;

/* Representation of an RGB color */
typedef struct _Color 
{
    unsigned char r;  /* Component Red   */
    unsigned char g;  /* Component Green */
    unsigned char b;  /* Component Blue  */
} Color;

/* Touchpad state representation */
typedef struct _Touch 
{
    unsigned int x;   /* Touch X Position (~0 - 2000) */
    unsigned int y;   /* Ring  Y Position (~0 - 2048) */
    bool down;        /* 1 if ringing is active, 0 otherwise */ // BOOL dont work... , INT work
    unsigned char id; /* 7-bit touch ID */
} Touch;

/* Battery status representation */
typedef struct _Battery 
{
    bool chargin;      /* Battery charging status (0 = Not charging, 1 = Charging) */
    bool fullyCharged; /* Indicates whether the battery is fully charged */
    unsigned char level;  /* Battery charge level (0x0 - 0xFF) */
} Battery;

/* State of the mic led */
typedef unsigned char _MicLed;
typedef enum _MicLed
{
    /* Lef is off */
    OFF = 0x00,

    /* Led is on */
    ON = 0x01,

    /* Led is pulsing */
    PULSE = 0x02,
} MicLed;

typedef unsigned char _TriggerEffectType;
typedef enum  _TriggerEffectType
{
	/* No resistance is applied */
	NoResitance = 0x00,

	/* Continuous Resitance is applied */
	ContinuousResitance = 0x01,

	/* Seciton resistance is appleyed */
	SectionResitance = 0x02,

	/* Extended trigger effect */
	EffectEx = 0x26,

	/* Calibrate triggers */
	Calibrate = 0xFC,

} TriggerEffectType;

/* Effect of resistance applied to the trigger */
typedef struct _TriggerEffect 
{

    TriggerEffectType effectType; /* Type of trigger effect */

    // unsigned char _pad1[1];    /* Padding manual para alinhamento */

    /* Effect parameters */
    union 
    {
        unsigned char _u1_raw[6];  /* Raw data for this purpose */

        /* For CONTINUOUS_RESISTANCE type */
        struct 
        {
            unsigned char startPosition;  /* Starting position of resistance */
            unsigned char force;          /* Resistance strength */
            unsigned char _pad[4];        /* Padding for alignment */
        } Continuous;

        /* For SECTION_RESISTANCE type */
        struct 
        {
            unsigned char startPosition;  /* Starting position of resistance */
            unsigned char endPosition;    /* End position of resistance */
            unsigned char _pad[4];        /* Padding for alignment */
        } Section;

        /* For EFFECT_EX type */
        struct 
        {
            unsigned char startPosition;  /* For EFFECT EX type */
            bool keepEffect;              /* 1 to maintain the effect */
            unsigned char beginForce;     /* Strength when trigger >= (255 / 2) */
            unsigned char middleForce;    /* Strength when trigger <= (255 / 2) */
            unsigned char endForce;       /* Strength when trigger > 255 */
            unsigned char frequency;      /* Trigger vibration frequency */
        } EffectEx;
    };
} TriggerEffect;


typedef unsigned char _LedBrightness;
typedef enum _LedBrightness
{
    LOW = 0x02,

    MEDIUM = 0x01,

    HIGH = 0x00,

} LedBrightness;

/* Player LEDs */
typedef struct _PlayerLeds 
{
    unsigned char bitmask;          /* Player LED bitmask */
    bool playerLedFade;             /* Player LED fade (1 = fade, 0 = no) */ // BOOL Dont Work
    LedBrightness brightness;
} PlayerLeds;

/* DS5 Controller Input State */
typedef struct _DS5InputState 
{
    AnalogStick leftStick;              /* Left analog stick position */
    AnalogStick rightStick;             /* Right analog stick position */
    unsigned char leftTrigger;          /* Left trigger position */
    unsigned char rightTrigger;         /* Right trigger position */
    unsigned char buttonsAndDpad;       /* Button and dpad mask */
    unsigned char buttonsA;             /* A Button Mask  */
    unsigned char buttonsB;             /* B Buttons Mask */
    Vec3 accelerometer;                 /* Accelerometer (3D vector) */
    Vec3 gyroscope;                     /* Gyroscope (3D vector) */
    Touch touchPoint1;                  /* First touch point */
    Touch touchPoint2;                  /* Second touch point */
    Battery battery;                    /* Battery Information */
    bool headPhoneConnected;             /* 1 whether the headset is connected */
    unsigned char leftTriggerFeedback;  /* Left trigger feedback */
    unsigned char rightTriggerFeedback; /* Right trigger feedback */
} DS5InputState;


/* DS5 controller output state */
typedef struct _DS5OutputState 
{
    unsigned char leftRumble;           /* Left vibration motor */
    unsigned char rightRumble;          /* Right vibration motor */
    MicLed microphoneLed;               /* Microphone LED status */
    bool disableLeds;                    /* Disables all LEDs (1 = off) */ // BOOL dont work , INT work
    PlayerLeds playerLeds;              /* Player LEDs */
    Color lightbar;                     /* Light bar color */
    TriggerEffect leftTriggerEffect;    /* Left trigger effect */
    TriggerEffect rightTriggerEffect;   /* Right trigger effect */
} DS5OutputState;




#endif /* JOYSTATE_H */
