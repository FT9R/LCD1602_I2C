#ifndef LCD1602_I2C_H
#define LCD1602_I2C_H

#include "main.h"
#include "stdbool.h"

/* Device constants */
#define PCF8574_ADDRESS 0b01001110
#define PCF8574A_ADDRESS 0b01111110

typedef struct
{
	I2C_HandleTypeDef *hi2c;
	uint8_t address;
	uint8_t instruction;
	uint8_t data;
	FlagStatus RS_Pin;
	FlagStatus RW_Pin;
	FlagStatus E_Pin;
	FunctionalState LED;
	FunctionalState displayState;
	FunctionalState cursorState;
	FunctionalState blinkState;
} lcd1602_HandleTypeDef;

typedef enum
{
	ShiftRight,
	ShiftLeft
} ShiftDirection;

/**
 * @brief	Binds lcd1602 handle to the target device and initializes it
 * @param	lcd1602_Handle: pointer to the user-declared lcd1602 descriptor
 * @param	hi2c: pointer to HAL I2C descriptor
 * @param	address: PCF8574 I/O expander I2C address
 * @retval	none
 */
void lcd1602_Init(lcd1602_HandleTypeDef *lcd1602_Handle, I2C_HandleTypeDef *hi2c, uint8_t address);

/**
 * @brief	Prints string on the display
 * @param	lcd1602_Handle: pointer to the user-declared lcd1602 descriptor
 * @param	pString: pointer to the null-terminated char sequence
 * @retval	none
 */
void lcd1602_Print(lcd1602_HandleTypeDef *lcd1602_Handle, uint8_t *pString);

/**
 * @brief	Sets cursor to certain place on the display
 * @param	lcd1602_Handle: pointer to the user-declared lcd1602 descriptor
 * @param	col: column number started from 0
 * @param	row: row number started from 0
 * @retval	none
 */
void lcd1602_SetCursor(lcd1602_HandleTypeDef *lcd1602_Handle, uint8_t col, uint8_t row);

/**
 * @brief	Clears entire display and sets DDRAM address 0 in address counter
 * @param	lcd1602_Handle: pointer to the user-declared lcd1602 descriptor
 * @retval	none
 */
void lcd1602_Clear(lcd1602_HandleTypeDef *lcd1602_Handle);

/**
 * @brief	Sets DDRAM address 0 in address counter
 * @param	lcd1602_Handle: pointer to the user-declared lcd1602 descriptor
 * @retval	none
 */
void lcd1602_Home(lcd1602_HandleTypeDef *lcd1602_Handle);

/**
 * @brief	Display on/off control
 * @param	lcd1602_Handle: pointer to the user-declared lcd1602 descriptor
 * @param	state: new state
 * @retval	none
 */
void lcd1602_Display(lcd1602_HandleTypeDef *lcd1602_Handle, FunctionalState state);

/**
 * @brief	Cursor on/off control
 * @param	lcd1602_Handle: pointer to the user-declared lcd1602 descriptor
 * @param	state: new state
 * @retval	none
 */
void lcd1602_Cursor(lcd1602_HandleTypeDef *lcd1602_Handle, FunctionalState state);

/**
 * @brief	Blink on/off control
 * @param	lcd1602_Handle: pointer to the user-declared lcd1602 descriptor
 * @param	state: new state
 * @retval	none
 */
void lcd1602_Blink(lcd1602_HandleTypeDef *lcd1602_Handle, FunctionalState state);

/**
 * @brief	LED backlight on/off control
 * @param	lcd1602_Handle: pointer to the user-declared lcd1602 descriptor
 * @param	state: new state
 * @retval	none
 */
void lcd1602_LED(lcd1602_HandleTypeDef *lcd1602_Handle, FunctionalState state);

/**
 * @brief	Shifts display without changing DDRAM contents
 * @param	lcd1602_Handle: pointer to the user-declared lcd1602 descriptor
 * @param	direction: shift direction
 * ------	@arg ShiftLeft
 * ------	@arg ShiftRight
 * @retval	none
 */
void lcd1602_DisplayShift(lcd1602_HandleTypeDef *lcd1602_Handle, ShiftDirection direction);

/**
 * @brief	Shifts cursor without changing DDRAM contents
 * @param	lcd1602_Handle: pointer to the user-declared lcd1602 descriptor
 * @param	direction: shift direction
 * ------	@arg ShiftLeft
 * ------	@arg ShiftRight
 * @retval	none
 */
void lcd1602_CursorShift(lcd1602_HandleTypeDef *lcd1602_Handle, ShiftDirection direction);
#endif