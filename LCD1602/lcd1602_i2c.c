#include "lcd1602_i2c.h"

/* Private function prototypes */
void RS_Pin(lcd1602_HandleTypeDef *lcd1602_Handle, FlagStatus status);
void RW_Pin(lcd1602_HandleTypeDef *lcd1602_Handle, FlagStatus status);
void E_Pin(lcd1602_HandleTypeDef *lcd1602_Handle, FlagStatus status);
uint8_t CtrlPinsRequest(lcd1602_HandleTypeDef *lcd1602_Handle);
void Instruction_Write(lcd1602_HandleTypeDef *lcd1602_Handle, const uint8_t *instruction);
void Data_Write(lcd1602_HandleTypeDef *lcd1602_Handle, const uint8_t *data);

void lcd1602_Init(lcd1602_HandleTypeDef *lcd1602_Handle, I2C_HandleTypeDef *hi2c, uint8_t address)
{
	HAL_Delay(100);
	/* Device specific info retrieve */
	lcd1602_Handle->hi2c = hi2c;
	lcd1602_Handle->address = address;
	lcd1602_Handle->instruction = NULL;
	lcd1602_Handle->data = NULL;
	lcd1602_Handle->RS_Pin = SET;
	lcd1602_Handle->RW_Pin = SET;
	lcd1602_Handle->E_Pin = SET;
	lcd1602_Handle->LED = ENABLE;
	lcd1602_Handle->displayState = DISABLE;
	lcd1602_Handle->cursorState = DISABLE;
	lcd1602_Handle->blinkState = DISABLE;
	/* At power on, all the ports are HIGH [PCF8574 datasheet] */
	RS_Pin(lcd1602_Handle, RESET);
	RW_Pin(lcd1602_Handle, RESET);
	E_Pin(lcd1602_Handle, RESET);
	lcd1602_LED(lcd1602_Handle, DISABLE);
	/* Set 4-Bit Interface */
	HAL_Delay(50);
	lcd1602_Handle->instruction = 0b00110000;
	Instruction_Write(lcd1602_Handle, &lcd1602_Handle->instruction);
	HAL_Delay(5);
	Instruction_Write(lcd1602_Handle, &lcd1602_Handle->instruction);
	HAL_Delay(1);
	Instruction_Write(lcd1602_Handle, &lcd1602_Handle->instruction);
	lcd1602_Handle->instruction = 0b00100000;
	Instruction_Write(lcd1602_Handle, &lcd1602_Handle->instruction);
	/* Specify the number of display lines and character font */
	lcd1602_Handle->instruction = 0b00100000;
	Instruction_Write(lcd1602_Handle, &lcd1602_Handle->instruction);
	lcd1602_Handle->instruction = 0b10000000;
	Instruction_Write(lcd1602_Handle, &lcd1602_Handle->instruction);
	/* Display off */
	lcd1602_Display(lcd1602_Handle, DISABLE);
	/* Display clear */
	lcd1602_Clear(lcd1602_Handle);
	/* Entry mode set */
	lcd1602_Handle->instruction = 0b00000000;
	Instruction_Write(lcd1602_Handle, &lcd1602_Handle->instruction);
	lcd1602_Handle->instruction = 0b01100000;
	Instruction_Write(lcd1602_Handle, &lcd1602_Handle->instruction);
	/* Display on */
	lcd1602_Display(lcd1602_Handle, ENABLE);
	/* LED backlight on */
	HAL_Delay(500);
	lcd1602_LED(lcd1602_Handle, ENABLE);
	HAL_Delay(500);
}

void lcd1602_Print(lcd1602_HandleTypeDef *lcd1602_Handle, uint8_t *pString)
{
	do
	{
		lcd1602_Handle->data = *pString++;
		Data_Write(lcd1602_Handle, &lcd1602_Handle->data);
		lcd1602_Handle->data = lcd1602_Handle->data << 4;
		Data_Write(lcd1602_Handle, &lcd1602_Handle->data);
	} while (*pString != '\0');
}

void lcd1602_SetCursor(lcd1602_HandleTypeDef *lcd1602_Handle, uint8_t col, uint8_t row)
{
	uint8_t DDRAM_address = col + (0x40 * row);
	lcd1602_Handle->instruction = 0b10000000 | DDRAM_address;
	Instruction_Write(lcd1602_Handle, &lcd1602_Handle->instruction);
	lcd1602_Handle->instruction = lcd1602_Handle->instruction << 4;
	Instruction_Write(lcd1602_Handle, &lcd1602_Handle->instruction);
}

void lcd1602_Clear(lcd1602_HandleTypeDef *lcd1602_Handle)
{
	lcd1602_Handle->instruction = 0b00000001;
	Instruction_Write(lcd1602_Handle, &lcd1602_Handle->instruction);
	lcd1602_Handle->instruction = lcd1602_Handle->instruction << 4;
	Instruction_Write(lcd1602_Handle, &lcd1602_Handle->instruction);
	HAL_Delay(2);
}

void lcd1602_Home(lcd1602_HandleTypeDef *lcd1602_Handle)
{
	lcd1602_Handle->instruction = 0b00000010;
	Instruction_Write(lcd1602_Handle, &lcd1602_Handle->instruction);
	lcd1602_Handle->instruction = lcd1602_Handle->instruction << 4;
	Instruction_Write(lcd1602_Handle, &lcd1602_Handle->instruction);
	HAL_Delay(2);
}

void lcd1602_Display(lcd1602_HandleTypeDef *lcd1602_Handle, FunctionalState state)
{
	lcd1602_Handle->instruction = 0b00001000;
	lcd1602_Handle->displayState = state;
	if (lcd1602_Handle->displayState == ENABLE)
		SET_BIT(lcd1602_Handle->instruction, 1 << 2);
	if (lcd1602_Handle->cursorState == ENABLE)
		SET_BIT(lcd1602_Handle->instruction, 1 << 1);
	if (lcd1602_Handle->blinkState == ENABLE)
		SET_BIT(lcd1602_Handle->instruction, 1 << 0);
	Instruction_Write(lcd1602_Handle, &lcd1602_Handle->instruction);
	lcd1602_Handle->instruction = lcd1602_Handle->instruction << 4;
	Instruction_Write(lcd1602_Handle, &lcd1602_Handle->instruction);
}

void lcd1602_Cursor(lcd1602_HandleTypeDef *lcd1602_Handle, FunctionalState state)
{
	lcd1602_Handle->instruction = 0b00001000;
	lcd1602_Handle->cursorState = state;
	if (lcd1602_Handle->displayState == ENABLE)
		SET_BIT(lcd1602_Handle->instruction, 1 << 2);
	if (lcd1602_Handle->cursorState == ENABLE)
		SET_BIT(lcd1602_Handle->instruction, 1 << 1);
	if (lcd1602_Handle->blinkState == ENABLE)
		SET_BIT(lcd1602_Handle->instruction, 1 << 0);
	Instruction_Write(lcd1602_Handle, &lcd1602_Handle->instruction);
	lcd1602_Handle->instruction = lcd1602_Handle->instruction << 4;
	Instruction_Write(lcd1602_Handle, &lcd1602_Handle->instruction);
}

void lcd1602_Blink(lcd1602_HandleTypeDef *lcd1602_Handle, FunctionalState state)
{
	lcd1602_Handle->instruction = 0b00001000;
	lcd1602_Handle->blinkState = state;
	if (lcd1602_Handle->displayState == ENABLE)
		SET_BIT(lcd1602_Handle->instruction, 1 << 2);
	if (lcd1602_Handle->cursorState == ENABLE)
		SET_BIT(lcd1602_Handle->instruction, 1 << 1);
	if (lcd1602_Handle->blinkState == ENABLE)
		SET_BIT(lcd1602_Handle->instruction, 1 << 0);
	Instruction_Write(lcd1602_Handle, &lcd1602_Handle->instruction);
	lcd1602_Handle->instruction = lcd1602_Handle->instruction << 4;
	Instruction_Write(lcd1602_Handle, &lcd1602_Handle->instruction);
}

void lcd1602_LED(lcd1602_HandleTypeDef *lcd1602_Handle, FunctionalState state)
{
	lcd1602_Handle->LED = state;
	uint8_t ctrlPins = CtrlPinsRequest(lcd1602_Handle);
	HAL_I2C_Master_Transmit(lcd1602_Handle->hi2c, lcd1602_Handle->address, &ctrlPins, 1, 1000);
}

void lcd1602_DisplayShift(lcd1602_HandleTypeDef *lcd1602_Handle, ShiftDirection direction)
{
	lcd1602_Handle->instruction = 0b00011000;
	if (direction == ShiftRight)
		SET_BIT(lcd1602_Handle->instruction, 1 << 2);
	Instruction_Write(lcd1602_Handle, &lcd1602_Handle->instruction);
	lcd1602_Handle->instruction = lcd1602_Handle->instruction << 4;
	Instruction_Write(lcd1602_Handle, &lcd1602_Handle->instruction);
}

void lcd1602_CursorShift(lcd1602_HandleTypeDef *lcd1602_Handle, ShiftDirection direction)
{
	lcd1602_Handle->instruction = 0b00010000;
	if (direction == ShiftRight)
		SET_BIT(lcd1602_Handle->instruction, 1 << 2);
	Instruction_Write(lcd1602_Handle, &lcd1602_Handle->instruction);
	lcd1602_Handle->instruction = lcd1602_Handle->instruction << 4;
	Instruction_Write(lcd1602_Handle, &lcd1602_Handle->instruction);
}

/**
 *@section Private functions
 */
void RS_Pin(lcd1602_HandleTypeDef *lcd1602_Handle, FlagStatus status)
{
	if (lcd1602_Handle->RS_Pin != status)
	{
		lcd1602_Handle->RS_Pin = status;
		uint8_t ctrlPins = CtrlPinsRequest(lcd1602_Handle);
		HAL_I2C_Master_Transmit(lcd1602_Handle->hi2c, lcd1602_Handle->address, &ctrlPins, 1, 1000);
	}
}

void RW_Pin(lcd1602_HandleTypeDef *lcd1602_Handle, FlagStatus status)
{
	if (lcd1602_Handle->RW_Pin != status)
	{
		lcd1602_Handle->RW_Pin = status;
		uint8_t ctrlPins = CtrlPinsRequest(lcd1602_Handle);
		HAL_I2C_Master_Transmit(lcd1602_Handle->hi2c, lcd1602_Handle->address, &ctrlPins, 1, 1000);
	}
}

void E_Pin(lcd1602_HandleTypeDef *lcd1602_Handle, FlagStatus status)
{
	if (lcd1602_Handle->E_Pin != status)
	{
		lcd1602_Handle->E_Pin = status;
		uint8_t ctrlPins = CtrlPinsRequest(lcd1602_Handle);
		HAL_I2C_Master_Transmit(lcd1602_Handle->hi2c, lcd1602_Handle->address, &ctrlPins, 1, 1000);
	}
}

uint8_t CtrlPinsRequest(lcd1602_HandleTypeDef *lcd1602_Handle)
{
	return (uint8_t)(lcd1602_Handle->RS_Pin << 0 | lcd1602_Handle->RW_Pin << 1 | lcd1602_Handle->E_Pin << 2 | lcd1602_Handle->LED << 3);
}

void Instruction_Write(lcd1602_HandleTypeDef *lcd1602_Handle, const uint8_t *instruction)
{
	uint8_t ctrlPinsAndInstruction;
	RS_Pin(lcd1602_Handle, RESET);
	RW_Pin(lcd1602_Handle, RESET);
	E_Pin(lcd1602_Handle, SET);
	MODIFY_REG(ctrlPinsAndInstruction, 0xF, CtrlPinsRequest(lcd1602_Handle));
	MODIFY_REG(ctrlPinsAndInstruction, 0xF0, (*instruction) & 0xF0);
	HAL_I2C_Master_Transmit(lcd1602_Handle->hi2c, lcd1602_Handle->address, &ctrlPinsAndInstruction, 1, 1000);
	E_Pin(lcd1602_Handle, RESET);
}

void Data_Write(lcd1602_HandleTypeDef *lcd1602_Handle, const uint8_t *data)
{
	uint8_t ctrlPinsAndData;
	RS_Pin(lcd1602_Handle, SET);
	RW_Pin(lcd1602_Handle, RESET);
	E_Pin(lcd1602_Handle, SET);
	MODIFY_REG(ctrlPinsAndData, 0xF, CtrlPinsRequest(lcd1602_Handle));
	MODIFY_REG(ctrlPinsAndData, 0xF0, (*data) & 0xF0);
	HAL_I2C_Master_Transmit(lcd1602_Handle->hi2c, lcd1602_Handle->address, &ctrlPinsAndData, 1, 1000);
	E_Pin(lcd1602_Handle, RESET);
}